# AI Health Companion – Real-Time Health Risk Predictor

AI Health Companion is an IoT and Machine Learning based health monitoring system that collects vital health parameters using ESP32 and sensors, stores the data in Firebase, and uses a Random Forest model to classify health risk.

## Features

- Real-time health monitoring
- Heart rate monitoring
- SpO2 monitoring
- Temperature monitoring
- IoT-based data collection using ESP32
- Firebase real-time data storage
- Machine Learning based health-risk classification
- Web-based dashboard
- Data visualization using charts

## Hardware Used

- ESP32 Dev Module
- MAX30102 – Heart Rate and SpO2 sensor
- BMP180 – Temperature and atmospheric pressure sensor

## Software and Technologies

- Arduino IDE
- ESP32
- Python
- Flask
- Pandas
- NumPy
- Scikit-learn
- Random Forest Classifier
- Firebase
- HTML
- CSS
- JavaScript
- Chart.js

## Machine Learning

A Random Forest Classifier is used to classify the collected health data into risk categories such as High Risk and Low Risk.

The model was trained using a human vital signs dataset.

## System Workflow

```text
MAX30102 + BMP180
        ↓
      ESP32
        ↓
      Wi-Fi
        ↓
     Firebase
        ↓
Machine Learning Model
        ↓
 High Risk / Low Risk
        ↓
   Web Dashboard