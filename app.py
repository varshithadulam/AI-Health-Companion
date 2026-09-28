from flask import Flask, jsonify, render_template
import firebase_admin
from firebase_admin import credentials, db
import pandas as pd
import joblib

app = Flask(__name__)

# ---------- Firebase ----------
cred = credentials.Certificate("firebase/serviceAccountKey.json")
firebase_admin.initialize_app(cred, {
    "databaseURL": "https://health-ccfd6-default-rtdb.firebaseio.com/"
})

# ---------- Load ML model ----------
model = joblib.load("model/health_model.pkl")

def get_latest_sensor_data():
    ref = db.reference("health_data")
    data = ref.get()

    if data is None:
        raise ValueError("No data in Firebase")

    if isinstance(data, dict):
        first_value = next(iter(data.values()))

        # Case: collection (dict of dicts)

        if isinstance(first_value, dict):
            df = pd.DataFrame.from_dict(data, orient="index")
            return df.iloc[-1]

        # Case: single record
        return pd.Series(data)

    raise TypeError(f"Unexpected Firebase data type: {type(data)}")

@app.route("/")
def home():
    return render_template("index.html")

@app.route("/predict")
def predict():
    latest = get_latest_sensor_data()

    # Prepare ML input (safe defaults)
    input_data = pd.DataFrame([{
        "Heart Rate": latest.get("heart_rate", 75),
        "Body Temperature": latest.get("temperature", 36.5),
        "Oxygen Saturation": latest.get("spo2", 98),
        "Systolic Blood Pressure": 120,   # placeholder
        "Diastolic Blood Pressure": 80    # placeholder
    }])

    # ML prediction
    prediction = model.predict(input_data)[0]

    # Final response (NO confidence)
    result = {
        "risk": "High Risk" if prediction == 1 else "Low Risk",
        "heart_rate": latest.get("heart_rate"),
        "spo2": latest.get("spo2"),
        "temperature": latest.get("temperature"),
        "pressure": latest.get("pressure")  # from BMP180
    }

    return jsonify(result)


if __name__ == "__main__":
    app.run(debug=True)