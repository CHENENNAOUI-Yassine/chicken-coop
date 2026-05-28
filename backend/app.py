from flask import Flask, jsonify, request, send_from_directory
from flask_cors import CORS
from datetime import datetime
import json
import os

app = Flask(__name__, static_folder='static', static_url_path='')
CORS(app)

DATA_FILE = "temperature_data.json"


# -----------------------
# Prevent HTTPS redirect on Render
# -----------------------
@app.before_request
def allow_http():
    """Allow HTTP connections without redirect (for ESP8266 compatibility)"""
    # Do NOT redirect HTTP to HTTPS
    # ESP8266 uses HTTP over SSL, so we need to accept both
    pass


# -----------------------
# Helpers
# -----------------------
def load_data():
    if os.path.exists(DATA_FILE):
        try:
            with open(DATA_FILE, 'r') as f:
                return json.load(f)
        except:
            pass
    return {"readings": [], "current_temp": None, "status": "unknown"}


def save_data(data):
    with open(DATA_FILE, 'w') as f:
        json.dump(data, f, indent=2)


def get_status(temp):
    if temp > 26:
        return "ALERT"
    elif 19 <= temp <= 26:
        return "GOOD"
    elif temp < 19:
        return "FREEZING"


# -----------------------
# Routes
# -----------------------

@app.route('/api/temperature', methods=['POST'])
def receive_temperature():
    data = request.get_json()

    if not data or 'temperature' not in data:
        return jsonify({"success": False, "message": "Missing temperature"}), 400

    try:
        temp = float(data['temperature'])
    except:
        return jsonify({"success": False, "message": "Invalid value"}), 400

    file_data = load_data()

    timestamp = datetime.now().isoformat()

    reading = {
        "temperature": temp,
        "timestamp": timestamp,
        "status": get_status(temp)
    }

    file_data["readings"].append(reading)

    # keep last 100
    file_data["readings"] = file_data["readings"][-100:]

    file_data["current_temp"] = temp
    file_data["status"] = get_status(temp)
    file_data["last_updated"] = timestamp

    save_data(file_data)

    return jsonify({
        "success": True,
        "temperature": temp,
        "status": file_data["status"]
    })


@app.route('/api/status', methods=['GET'])
def get_status_route():
    file_data = load_data()

    return jsonify({
        "current_temp": file_data.get("current_temp"),
        "status": file_data.get("status"),
        "last_updated": file_data.get("last_updated"),
        "readings": file_data.get("readings", [])[-24:]
    })


@app.route('/api/status-http', methods=['GET'])
def status_http():
    file_data = load_data()
    return jsonify({
        "current_temp": file_data.get("current_temp"),
        "status": file_data.get("status"),
        "last_updated": file_data.get("last_updated")
    })


@app.route('/api/reset', methods=['POST'])
def reset_data():
    empty = {"readings": [], "current_temp": None, "status": "unknown"}
    save_data(empty)
    return jsonify({"success": True})


@app.route('/')
def root():
    return send_from_directory('static', 'index.html')


# -----------------------
# Run
# -----------------------
if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8000))
    app.run(host="0.0.0.0", port=port, debug=False)