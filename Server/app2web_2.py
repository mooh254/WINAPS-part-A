from flask import Flask, request, jsonify, render_template
import mysql.connector
from flask_cors import CORS
from datetime import datetime

app = Flask(__name__)
# allow cors
CORS(app, resources={r"/*": {"origins": ["http://localhost:3000", "http://127.0.0.1:3000"]}})

# Connect to MySQL
db_connection = mysql.connector.connect(
    host="127.0.0.1",
    port=3306,
    user="root",  # your_mysql_user
    password="Database_2",  # your_mysql_password
    database="sensor_data_db"
)
ssl_disabled = True
cursor = db_connection.cursor()

# Create table if it doesn't exist
cursor.execute('''
CREATE TABLE IF NOT EXISTS sensor_data (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp DATETIME NOT NULL,
    temperature FLOAT,
    humidity INT,
    heat_index FLOAT,
    noise INT,
    CO2 INT,
    methane FLOAT,
    NO2 FLOAT,
    dust FLOAT
)
''')
db_connection.commit()

# Template routes
@app.route('/', methods=['GET'])
def index_page():
    return render_template('index.html')

@app.route('/register', methods=['GET'])
def register_page():
    return render_template('register.html')

@app.route('/login', methods=['GET'])
def login_page():
    return render_template('login.html')

@app.route('/contact', methods=['GET'])
def contact_page():
    return render_template('contact.html')

@app.route('/userprofile', methods=['GET'])
def userprofile_page():
    return render_template('userprofile.html')

# @app.route('/monitoringmaps', methods=['GET'])
# def monitoringmaps_page():
#     return render_template('index.html')

@app.errorhandler(404)
def page_not_found(e):
    return render_template('404.html', error=e), 404

@app.route('/data', methods=['GET'])
def data_page():
    # Prepare SQL query for retrieving data
    query = 'SELECT * FROM sensor_data ORDER BY timestamp DESC'
    cursor.execute(query)
    results = cursor.fetchall()

    sensory_data = []
    for row in results:
        sensory_data.append({
            "id": row[0],
            "timestamp": row[1],
            "temperature": row[2],
            "humidity": row[3],
            "heat_index": row[4],  # heatIndex
            "noise": row[5],
            "CO2": row[6],  # mq2_ppm
            "methane": row[7],  # mq2_ppm
            "NO2": row[8],  # mq135_ppm
            "dust": row[9]  # dustDensityStr
        })
    return render_template('data.html', sensory_data=sensory_data)

# Endpoint to receive data
@app.route('/api/sensor_data', methods=['POST'])
def receive_sensor_data():
    data = request.json
    timestamp = datetime.now()

    # Prepare SQL query
    query = '''
    INSERT INTO sensor_data (timestamp, temperature, humidity, heat_index, noise, CO2, methane, NO2, dust)
    VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)
    '''
    values = (
        timestamp,
        float(data['temperature']),
        int(data['humidity']),
        float(data['heatIndex']),
        int(data['noise']),
        int(data['mq2_ppm']),
        float(data['mq3_ppm']),
        float(data['mq135_ppm']),
        float(data['dustDensityStr'])
    )

    cursor.execute(query, values)
    db_connection.commit()
    return jsonify({"message": "Data received successfully"}), 200

# Run the server
if __name__ == '__main__':
    app.run(debug=True)
    app.run(host='127.0.0.1', port=5000)