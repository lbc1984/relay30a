import json
import firebase_admin
from firebase_admin import auth, credentials

# Khởi tạo Firebase Admin SDK với serviceAccountKey.json
cred = credentials.Certificate("serviceAccountKey.json")
firebase_admin.initialize_app(cred)

# Danh sách email được phép truy cập
ALLOWED_USERS = ["lybaocuong@gmail.com", "ngochavina@gmail.com"]


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
        uid = decoded_token["uid"]
        email = decoded_token.get("email")

        if email not in ALLOWED_USERS:
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
