import pandas as pd
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
from sklearn.preprocessing import LabelEncoder

# Load and clean dataset
df = pd.read_excel("/content/adjusted_water_pump_data.xlsx")
df.columns = df.columns.str.strip().str.replace(" ", "_")

# Features and target
features = ['Inflow_Rate_Lpm', 'Outflow_Rate_Lpm']
target_reg = 'Water_Level_L'
target_clf = 'Pump_Status'

# Encode classification target
label_encoder = LabelEncoder()
df['Pump_Status_Encoded'] = label_encoder.fit_transform(df[target_clf])

# Prepare data
X = df[features]
y_reg = df[target_reg]
y_clf = df['Pump_Status_Encoded']

X_train_reg, X_test_reg, y_train_reg, y_test_reg = train_test_split(X, y_reg, test_size=0.2, random_state=42)
X_train_clf, X_test_clf, y_train_clf, y_test_clf = train_test_split(X, y_clf, test_size=0.2, stratify=y_clf, random_state=42)

# Train models
lin_model = LinearRegression().fit(X_train_reg, y_train_reg)
y_pred_reg = lin_model.predict(X_test_reg)
threshold = y_reg.median()
y_pred_lin_class = (y_pred_reg >= threshold).astype(int)

rf_model = RandomForestClassifier(random_state=42).fit(X_train_clf, y_train_clf)
y_pred_rf = rf_model.predict(X_test_clf)

# Accuracy scores
acc_lin = accuracy_score(y_test_clf, y_pred_lin_class)
acc_rf = accuracy_score(y_test_clf, y_pred_rf)

# Print Metrics for Linear Regression as classifier
print("=== Linear Regression (as Classifier) ===")
print("Accuracy:", acc_lin)
print("Confusion Matrix:\n", confusion_matrix(y_test_clf, y_pred_lin_class))
print("Classification Report:\n", classification_report(y_test_clf, y_pred_lin_class, target_names=['OFF', 'ON'], zero_division=0))

# Print Metrics for Random Forest Classifier
print("\n=== Random Forest Classifier ===")
print("Accuracy:", acc_rf)
print("Confusion Matrix:\n", confusion_matrix(y_test_clf, y_pred_rf))
print("Classification Report:\n", classification_report(y_test_clf, y_pred_rf, target_names=['OFF', 'ON'], zero_division=0))

# Plotting
models = ['Linear Regression (as Classifier)', 'Random Forest Classifier']
accuracies = [acc_lin, acc_rf]
colors = ['skyblue', 'lightgreen']

plt.figure(figsize=(8, 5))
bars = plt.bar(models, accuracies, color=colors, edgecolor='black')
plt.ylim(0, 1.1)
plt.ylabel('Accuracy')
plt.title('Model Accuracy Comparison')

# Annotate bars
for bar in bars:
    height = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2, height + 0.02, f'{height:.2f}', ha='center', va='bottom', fontsize=11)

# Show performance gap
plt.plot([0, 1], [acc_lin, acc_rf], color='red', linestyle='--', linewidth=1.5, label='Performance Gap')
plt.legend()
plt.tight_layout()
plt.show()
