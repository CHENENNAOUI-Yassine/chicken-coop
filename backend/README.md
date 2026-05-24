# 🐔 Chicken Coop Temperature Monitoring System

A real-time IoT-based temperature monitoring system for a chicken coop, featuring a responsive web dashboard, REST API, and ESP integration.

---

## 🚀 Features

* 📡 Real-time temperature ingestion via REST API
* 📊 Interactive dashboard with live updates (Chart.js)
* 🔥 Smart status detection (HOT / GOOD / FREEZING)
* 🕒 Historical data tracking (last 100 readings)
* 🔄 Auto-refresh every 5 seconds
* 🧹 Reset endpoint for testing
* 🌐 Accessible from any device on the network

---

## 🏗️ Tech Stack

* **Backend:** Flask (Python)
* **Frontend:** HTML, CSS, JavaScript
* **Charts:** Chart.js
* **Storage:** JSON file (lightweight persistence)
* **CORS:** Enabled for IoT devices (ESP8266 / ESP32)

---

## 📂 Project Structure

```
backend/
│── app.py
│── temperature_data.json
│── requirements.txt
└── static/
    └── index.html
```

---

## ⚙️ Installation

```bash
git clone <your-repo>
cd backend
pip install -r requirements.txt
```

---

## ▶️ Run the Server

```bash
python app.py
```

Server runs on:

```
http://localhost:8000
```

---

## 🔌 API Endpoints

### 📥 POST Temperature

```
POST /api/temperature
```

Body:

```json
{
  "temperature": 22.5
}
```

Example:

```bash
curl.exe -X POST http://localhost:8000/api/temperature ^
-H "Content-Type: application/json" ^
-d "{\"temperature\": 22.5}"
```

---

### 📊 Get Status

```
GET /api/status
```

---

### 📈 Get History

```
GET /api/history?limit=100
```

---

### 🧹 Reset Data

```
GET /api/reset
```

---

## 🌡️ Temperature Logic

| Range       | Status      |
| ----------- | ----------- |
| > 27°C      | 🔥 ALERT    |
| 19°C – 24°C | 😊 GOOD     |
| < 19°C      | ❄️ FREEZING |

---

## 🤖 ESP Integration

Example (pseudo-code):

```c
HTTP_POST("http://192.168.X.X:8000/api/temperature",
          "{\"temperature\": 22.5}");
```

---

## 💡 Future Improvements

* WebSocket real-time updates
* Database (SQLite / PostgreSQL)
* Authentication (JWT)
* Mobile-friendly UI enhancements
* Notifications (Telegram / Email)

---

## 👨‍💻 Author

Yassine Chenennaoui
Electrical Engineering Student – ENSIT
Focus: IoT, Embedded Systems, Smart Energy Systems
