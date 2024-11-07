//name file as index.js in aws lambda as your handler would be index.handler
//and also include node modules mongodb , this is just example of an index file

const { MongoClient } = require('mongodb');

const uri = "your mongodb connection string idiot";
const client = new MongoClient(uri);

exports.handler = async (event) => {
  let result;
  
  // Log the raw event to verify the incoming structure
  console.log("Received event:", JSON.stringify(event));

  try {
    // Determine where the data is located
    let data = event.body ? JSON.parse(event.body) : event;

    // Extract fields with fallback defaults
    const temperature = data.temperature;
    const humidity = data.humidity;
    const timestamp = data.timestamp || new Date().toISOString(); // Use current time if timestamp is missing

    // Validate fields
    if (temperature === undefined || humidity === undefined) {
      throw new Error("Missing required data fields: temperature or humidity");
    }

    await client.connect();
    const database = client.db("iot-aws");
    const collection = database.collection("data");

    // Insert data into MongoDB
    const insertResult = await collection.insertOne({ temperature, humidity, timestamp });
    
    result = {
      statusCode: 200,
      body: JSON.stringify({ message: "Data inserted", insertedId: insertResult.insertedId })
    };
  } catch (error) {
    console.error("Error:", error.message);
    result = {
      statusCode: 500,
      body: JSON.stringify({ message: "Error inserting data", error: error.message })
    };
  } finally {
    await client.close();
  }
  
  return result;
};


