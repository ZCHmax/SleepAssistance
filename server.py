from flask import Flask, jsonify, render_template, Response
from flask import request
import boto3
from datetime import datetime
import json

app = Flask(__name__)

@app.route("/")
def hello():
    print("Temperature: " + request.args.get("temperature") + " Humidity: " + request.args.get("humidity") + " Light: " + request.args.get("light"))
    # return "Temperature: " + str(request.args.get("temperature")) + " humidity: " + str(request.args.get("humidity") ) + "light: " + str(request.args.get("light"))

    # read data
    # dynamo_client = boto3.client('dynamodb')
    # return jsonify(dynamo_client.scan(TableName = 'CS147'))

    # write data
    db = boto3.resource('dynamodb')
    table = db.Table("CS147")
    response = table.put_item(
            Item = {
                'ID': str(datetime.now().strftime('%Y-%m-%d %H:%M:%S')),
                'Temperature': request.args.get("temperature"),
                'Humidity': request.args.get("humidity"),
                'Light': request.args.get("light")

            }
    )
    return response

@app.route("/dashboard")
def dashboard():
    return render_template("index.html")

@app.route("/chart-data")
def data():
    def collect():
        db_client = boto3.client("dynamodb")
        data = db_client.scan(TableName="CS147")
        items = data['Items']
        for obj in items:
            temp = dict()
            for field in obj:
                temp[field] = obj[field]['S']
            json_data = json.dumps(temp)
            # print(json_data)
            yield f"data:{json_data}\n\n"
    return Response(collect(), mimetype='text/event-stream')
