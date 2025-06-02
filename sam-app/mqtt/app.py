import json
import firebase_admin
from firebase_admin import auth, credentials, firestore
import boto3
import os
import requests
import jwt
import time
from datetime import datetime

BUCKET_NAME = os.environ['BUCKET_NAME']
FILE_NAME = os.environ['FILE_NAME']

def get_service_account_key():
    s3 = boto3.client("s3")
    file_obj = s3.get_object(Bucket=BUCKET_NAME, Key=FILE_NAME)
    file_content = file_obj["Body"].read().decode("utf-8")
    return json.loads(file_content)


# Khởi tạo Firebase Admin SDK với serviceAccountKey từ S3
if not firebase_admin._apps:
    service_account_json = get_service_account_key()
    cred = credentials.Certificate(service_account_json)
    firebase_admin.initialize_app(cred)

def get_allowed_users(document_name="list"):
    db = firestore.client()
    doc_ref = db.collection("allowed_users").document(document_name)
    doc = doc_ref.get()
    if doc.exists:
        return doc.to_dict().get("emails", [])
    return []


def get_service_mqtt(group_name="list"):
    return {
        "statusCode": 200,
        "headers": {
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Headers": "Authorization, Content-Type",
            "Access-Control-Allow-Methods": "OPTIONS, GET"
        },
        "body": json.dumps(
            get_service_mqtt_web() if group_name == "list" else get_service_mqtt_web_paulatim()
        ),
    }


def get_service_mqtt_web():
    return {
        "url": "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud",
        "port": "8883",
        "user": "lybaocuong",
        "password": "1234@Abcd",
        "wss": "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud:8884/mqtt"
    }

def get_service_mqtt_web_paulatim():
    return {
        "url": "6e1f75e607c949759fdbc09134f46c3a.s1.eu.hivemq.cloud",
        "port": "8883",
        "user": "lybaocuong",
        "password": "1234@Abcd",
        "wss": "6e1f75e607c949759fdbc09134f46c3a.s1.eu.hivemq.cloud:8884/mqtt"
    }

def send_viber(message_text):
    lambda_url = "https://dbl7hxnfzt5jjfpbbnd4lk3mgy0viejv.lambda-url.ap-south-1.on.aws/"
    payload = {
        "receiverId": "rc+eiS+JAFLl3CxRpznfIg==",
        "messageText": message_text,
        "botName": "Manager Device"
    }
    headers = {
        "Content-Type": "application/json"
    }

    try:
        response = requests.post(
            lambda_url, data=json.dumps(payload), headers=headers)
        return response.json()
    except requests.exceptions.RequestException as e:
        return {"error": str(e)}

def generate_mqtt_token(payload):
    secret = datetime.utcnow().strftime("%H%M")
    payload["exp"] = int(time.time()) + 60  # Hết hạn sau 1 phút

    token = jwt.encode(payload, secret, algorithm="HS256")
    return token


def lambda_handler(event, context):
    headers = event.get("headers", {})
    auth_header = headers.get("Authorization")
    mac_address = headers.get("X-Device-MAC")
    group_name = headers.get("X-Group-Name") or "list"

    result = None

    if not auth_header and not mac_address:
        result = {
            "statusCode": 401,
            "body": json.dumps({"error": "Unauthorized", "message": "Authorization header is missing"})
        }
    elif mac_address:
        result = get_service_mqtt(group_name)
        send_viber(f"MAC Address: {mac_address} đã truy cập vào hệ thống")
    elif auth_header:
        id_token = auth_header.split("Bearer ")[1]

        try:
            decoded_token = auth.verify_id_token(id_token)
            email = decoded_token.get("email")
            host = headers.get("Host")
            
            if "rubicon" in host:
                group_name = "list"
            elif "paulatim" in host: 
                group_name == "paulatim"

            allowed_emails = get_allowed_users(group_name)

            if email not in allowed_emails:
                result = {
                    "statusCode": 403,
                    "body": json.dumps({"error": "Forbidden", "message": "Email không được phép truy cập"})
                }
            else:
                payload = get_service_mqtt_web() if group_name == "list" else get_service_mqtt_web_paulatim()
                token = generate_mqtt_token(payload)
                result = {
                    "statusCode": 200,
                    "headers": {
                        "Access-Control-Allow-Origin": "*",
                        "Access-Control-Allow-Headers": "Authorization, Content-Type, X-Data",
                        "Access-Control-Expose-Headers": "X-Data",
                        "Access-Control-Allow-Methods": "OPTIONS, GET",
                        "X-Data": f"{token}"
                    }
                }
        except Exception as e:
            result = {
                "statusCode": 401,
                "body": json.dumps({"error": "Invalid token", "details": str(e)})
            }

    return result
