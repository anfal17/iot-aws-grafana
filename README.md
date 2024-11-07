# IoT Sensor Data Pipeline with AWS IoT Core, MongoDB, and Grafana <br>

This project demonstrates an end-to-end IoT data pipeline that collects simulated sensor data from [Wokwi IoT Simulator](https://wokwi.com/), routes it through AWS IoT Core using MQTT, triggers AWS Lambda to process and store the data in MongoDB, and visualizes the data on Grafana.<br>

## Project Overview<br>

- **Data Source**: Wokwi IoT Simulator generates temperature and humidity sensor data.<br>
- **Message Broker**: AWS IoT Core serves as an MQTT broker to receive data from the simulator.<br>
- **Data Processing**: AWS Lambda processes incoming data and stores it in MongoDB.<br>
- **Database**: MongoDB stores sensor data.<br>
- **Visualization**: Grafana connects to MongoDB to visualize temperature and humidity trends.<br>

---

## Project Architecture<br>

1. **Wokwi IoT Simulator**: Simulates sensor data (e.g., DHT11 or DHT22) and publishes data to AWS IoT Core over MQTT.<br>
2. **AWS IoT Core**: Receives MQTT messages from Wokwi and triggers AWS Lambda to process and store data in MongoDB.<br>
3. **AWS Lambda**: Processes incoming IoT data, validates the data fields, and inserts records into MongoDB.<br>
4. **MongoDB**: Stores sensor data.<br>
5. **Grafana**: Visualizes data stored in MongoDB for monitoring.<br>

---

## Project Setup<br>

### 1. Wokwi IoT Simulator<br>

1. Create a new project on [Wokwi IoT Simulator](https://wokwi.com/).<br>
2. Set up a virtual sensor (e.g., DHT11 or DHT22) to generate temperature and humidity data.<br>
3. Configure the sensor to publish data to an AWS IoT Core MQTT topic.<br>

### 2. AWS IoT Core<br>

1. **Create an IoT Thing** in AWS IoT Core to represent the virtual sensor.<br>
2. **Set up MQTT**: Use the AWS IoT Core MQTT broker endpoint and configure AWS IoT policies to allow data publishing.<br>
3. **Create an IoT Rule**: Write an SQL statement to filter messages from the topic (e.g., `SELECT * FROM 'sensor/weather'`). Set the rule action to trigger AWS Lambda.<br>

### 3. AWS Lambda<br>

1. **Create a Lambda function** using Node.js runtime.<br>
2. **Code Sample**:
   ```javascript
   const { MongoClient } = require('mongodb');
   const uri = "your-mongodb-uri"; // Replace with your MongoDB URI
   const client = new MongoClient(uri);

   exports.handler = async (event) => {
     try {
       // Parse and validate data
       const data = JSON.parse(event.body || '{}');
       const { temperature, humidity, timestamp = new Date().toISOString() } = data;

       if (temperature === undefined || humidity === undefined) {
         throw new Error("Missing required fields");
       }

       // Connect to MongoDB
       await client.connect();
       const db = client.db("iot-data");
       const collection = db.collection("sensor-data");

       // Insert data into MongoDB
       await collection.insertOne({ temperature, humidity, timestamp });
       return { statusCode: 200, body: JSON.stringify({ message: "Data inserted successfully" }) };

     } catch (error) {
       return { statusCode: 500, body: JSON.stringify({ error: error.message }) };
     } finally {
       await client.close();
     }
   };

# IoT Sensor Data Pipeline with AWS IoT Core, MongoDB, and Grafana<br>

This project demonstrates an end-to-end IoT data pipeline that collects simulated sensor data from [Wokwi IoT Simulator](https://wokwi.com/), routes it through AWS IoT Core using MQTT, triggers AWS Lambda to process and store the data in MongoDB, and visualizes the data on Grafana.<br>

## Project Overview<br>

- **Data Source**: Wokwi IoT Simulator generates temperature and humidity sensor data.<br>
- **Message Broker**: AWS IoT Core serves as an MQTT broker to receive data from the simulator.<br>
- **Data Processing**: AWS Lambda processes incoming data and stores it in MongoDB.<br>
- **Database**: MongoDB stores sensor data.<br>
- **Visualization**: Grafana connects to MongoDB to visualize temperature and humidity trends.<br>

---

## Project Setup<br>

### 1. Deploy the Lambda Function and Test It Using Sample Data from the IoT Simulator<br>

1. Create a Lambda function using Node.js runtime.<br>
2. In your Lambda function code, connect to MongoDB Atlas and handle incoming sensor data.<br>
3. Use sample data from the Wokwi IoT Simulator to test your Lambda function.<br>
4. Ensure the IoT Core rule triggers the Lambda function when new data is received.<br>

### 2. MongoDB Setup<br>

1. **Create a MongoDB Atlas Account**: Sign up on [MongoDB Atlas](https://www.mongodb.com/cloud/atlas) and set up a free-tier cluster.<br>
2. **Create a Database and Collection**: 
   - Database name: `iot-data`
   - Collection name: `sensor-data`<br>
3. **Connect MongoDB Atlas to AWS Lambda**:
   - Obtain the MongoDB connection string from MongoDB Atlas.
   - Store the MongoDB URI securely in AWS Lambda environment variables.<br>

### 3. Grafana Setup<br>

1. **Install Grafana** or use the hosted version from [Grafana Cloud](https://grafana.com/products/cloud/).<br>
2. **Add MongoDB as a Data Source**: Install the [MongoDB plugin](https://grafana.com/grafana/plugins/grafana-mongodb-datasource) for Grafana and configure it to connect to your MongoDB Atlas database.<br>
3. **Create Dashboards**: Create visualizations like time-series, bar graph graphs to display temperature and humidity data.<br>

---

## Testing the Workflow<br>

1. **Run the Wokwi Simulator** to start sending data.<br>
2. **Monitor AWS IoT Core** for incoming MQTT messages.<br>
3. **Verify Lambda Execution**: Check AWS CloudWatch Logs for Lambda to debug any issues.<br>
4. **Check MongoDB Atlas**: Ensure the data is stored correctly in MongoDB.<br>
5. **View Data in Grafana**: Open Grafana to see real-time visualizations of sensor data.<br>

---

## Sample Output<br>

- **MongoDB Record**:
  ```json
  {
    "_id": "unique-object-id",
    "temperature": 22.5,
    "humidity": 65,
    "timestamp": "2024-11-07T12:00:00Z"
  }


## Problems Faced<br>

In the initial stages of the project, we started by directly showing the data from the MQTT Pub/Sub system in AWS IoT Core. However, this approach turned out to be unreliable due to several factors such as data loss, delays in message delivery, and difficulty in ensuring the accuracy of the data over time. This lack of reliability made it challenging to scale and manage the IoT data effectively.<br>

### 1. **Direct MQTT Pub/Sub Data Handling**:
   - We initially attempted to handle the incoming data directly from the MQTT topics by subscribing to them and displaying the data. While this approach worked in the beginning, we encountered the following issues:
     - **Data Loss**: Some data messages were missed due to intermittent connectivity or disconnections between the MQTT client and the broker.
     - **Lack of Persistence**: The data was not being stored persistently, making it difficult to retain the information for later analysis or visualization.
     - **Real-time Issues**: While the data was coming in real-time, there was no mechanism to process, analyze, or store it efficiently for long-term use.<br>

### 2. **Solution: Introducing AWS Lambda and MongoDB**:
   - To address these issues, we decided to implement a more robust solution using **AWS Lambda** and **MongoDB**. We restructured the flow to ensure reliable data storage and better scalability:
     - **Lambda Function**: We created an AWS Lambda function to process and store the incoming IoT data. The Lambda function is triggered by an **IoT Core Rule** that listens to specific MQTT topics (e.g., sensor data like temperature, humidity, etc.). This allowed us to handle data in a more controlled environment and ensured that each data message is processed as it arrives.
     - **Storing Data in MongoDB**: We connected the Lambda function to a **MongoDB Atlas** database, which serves as the storage for all incoming sensor data. MongoDB was chosen due to its scalability, flexibility, and ease of integration with AWS Lambda. Each IoT message received from the MQTT topic is inserted into MongoDB with fields such as temperature, humidity, and timestamp for future analysis.<br>
   
### 3. **Visualizing Data in Grafana**:
   - Once the data was reliably stored in MongoDB, we needed a way to visualize it. We set up **Grafana** as the data visualization tool. Using the MongoDB plugin for Grafana, we connected Grafana to MongoDB and created dashboards that show real-time graphs of sensor data (temperature and humidity). This allowed us to monitor the IoT data visually and make data-driven decisions based on trends over time.<br>
   
### 4. **Final Workflow**:
   - The final workflow is as follows:
     - **Wokwi IoT Simulator** sends simulated data to **AWS IoT Core** via MQTT.
     - An **IoT Core Rule** is configured to trigger the **Lambda function** whenever new data is published.
     - The **Lambda function** processes and stores the data in **MongoDB**.
     - **Grafana** is used to visualize the data stored in MongoDB, creating dynamic dashboards that update in real-time.<br>

This new solution provided a reliable, scalable, and easily maintainable architecture that solved the initial issues we faced with direct MQTT data handling.

## Project Screenshots

### Wokwi IoT Sensor (Temperature & Humidity)
Simulated weather data from **Wokwi IoT Sensor**:
![Wokwi IoT Sensor](https://github.com/user-attachments/assets/ff4a78e6-9566-4ba2-85d7-30d1a2872b09)
![Wokwi IoT Sensor](https://github.com/user-attachments/assets/968fdc21-f6e5-4a85-b0a9-4cf4b2302f10)

### Grafana Dashboard
Real-time data visualization in **Grafana**:
![Grafana Dashboard](https://github.com/user-attachments/assets/e9c8d140-af17-44a7-a4f6-8ecacd0c3c8b)

### AWS IoT Core (MQTT)
AWS IoT Core handling MQTT communication:
![AWS IoT Core](https://github.com/user-attachments/assets/29c0957e-32d2-4272-8feb-23fda7eaa812)

### MQTT Pub/Sub
Data published and subscribed using **MQTT**:
![Pub/Sub](https://github.com/user-attachments/assets/f8395473-b658-49b4-8c62-163aa46d68ba)
![Pub/Sub](https://github.com/user-attachments/assets/fe479036-212a-4d1f-9e47-50830d23f50b)

### AWS Lambda
Processing the data with **AWS Lambda**:
![AWS Lambda](https://github.com/user-attachments/assets/e18fee9e-0351-485b-9032-8dc7fdab2404)
![AWS Lambda](https://github.com/user-attachments/assets/db57739b-1c21-4621-b4a3-ac62893570ab)

### MongoDB Database
Storing data in **MongoDB Atlas**:
![MongoDB Database](https://github.com/user-attachments/assets/ef3b1d33-aa32-4cd7-b82f-afcc20486273)

## Conclusion
The project demonstrated the integration of **IoT simulation**, **cloud services**, and **real-time visualization** for weather data. We learned how to connect multiple services and create an end-to-end solution for real-time monitoring.


