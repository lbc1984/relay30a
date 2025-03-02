import json
import firebase_admin
from firebase_admin import auth, credentials, firestore
import boto3
import os

BUCKET_NAME = os.environ['BUCKET_NAME']
FILE_NAME = os.environ['FILE_NAME']

def get_allowed_users():
    db = firestore.client()
    doc_ref = db.collection("allowed_users").document("list")
    doc = doc_ref.get()
    if doc.exists:
        return doc.to_dict().get("emails", [])
    return []

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

def lambda_handler(event, context):

    headers = event.get("headers", {})
    auth_header = headers.get("Authorization")

    if not auth_header or not auth_header.startswith("Bearer "):
        return {
            "statusCode": 200,
            "headers": {
                "Access-Control-Allow-Origin": "*",
                "Access-Control-Allow-Headers": "Authorization, Content-Type",
                "Access-Control-Allow-Methods": "OPTIONS, GET"
            },
            "body": json.dumps({
                "url": "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud",
                "port": "8883",
                "user": "lybaocuong",
                "password": "1234@Abcd",
                "wss": "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud:8884/mqtt"
            }),
        }

    id_token = auth_header.split("Bearer ")[1]

    try:
        decoded_token = auth.verify_id_token(id_token)
        email = decoded_token.get("email")
        allowed_emails = get_allowed_users()
        
        if email not in allowed_emails:
            return {
                "statusCode": 403,
                "body": json.dumps({"error": "Forbidden", "message": "Email không được phép truy cập"})
            }

        return {
            "statusCode": 200,
            "headers": {
                "Access-Control-Allow-Origin": "*",
                "Access-Control-Allow-Headers": "Authorization, Content-Type",
                "Access-Control-Allow-Methods": "OPTIONS, GET"
            },
            "body": json.dumps({
                "url": "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud",
                "port": "8883",
                "user": "lybaocuong",
                "password": "1234@Abcd",
                "wss": "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud:8884/mqtt"
            }),
        }

    except Exception as e:
        return {
            "statusCode": 401,
            "body": json.dumps({"error": "Invalid token", "details": str(e)})
        }
