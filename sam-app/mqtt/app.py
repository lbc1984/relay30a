import json

def lambda_handler(event, context):
    return {
        "statusCode": 200,
        "headers": {
            "Access-Control-Allow-Origin": "*"
        },
        "body": json.dumps({
            "url": "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud",
            "port": "8883",
            "user": "lybaocuong",
            "password": "1234@Abcd",
            "wss": "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud:8884/mqtt"
        }),
    }
