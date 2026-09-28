import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score
import joblib
import os

# Load dataset
data = pd.read_csv("data/human_vital_signs_dataset_2024.csv", low_memory=False)

# Select input features
X = data[
    [
        'Heart Rate',
        'Body Temperature',
        'Oxygen Saturation',
        'Systolic Blood Pressure',
        'Diastolic Blood Pressure'
    ]
]

# Output label
y = data['Risk Category'].map({
    'Low Risk': 0,
    'High Risk': 1
})

# Split dataset
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

# Train model
model = RandomForestClassifier(
    n_estimators=100,
    random_state=42
)
model.fit(X_train, y_train)

# Test model
y_pred = model.predict(X_test)
accuracy = accuracy_score(y_test, y_pred)

print("Model Accuracy:", accuracy)

# Save model
os.makedirs("model", exist_ok=True)
joblib.dump(model, "model/health_model.pkl")

print("Model trained and saved successfully")