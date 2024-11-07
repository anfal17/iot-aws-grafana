#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
//download these packages before running
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// WiFi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";


// AWS IoT Core credentials
const char* mqtt_server = "yourid.region.amazonaws.com";  // AWS IoT Endpoint
const int mqtt_port = 8883;
const char* mqtt_client_id = "ESP32Client";
const char* mqtt_topic = "iot/topic";  //iot/topic 

// Certificate and private key (string literals)
const char* certificate = R"(
-----BEGIN CERTIFICATE-----
... what reaa?
-----END CERTIFICATE-----
)";




const char* private_key = R"(
-----BEGIN RSA PRIVATE KEY-----
... what paa?
-----END RSA PRIVATE KEY-----
)";

const char* ca_root = R"(
-----BEGIN CERTIFICATE-----
... what do u want cerficate aaa?
-----END CERTIFICATE-----
)";

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

void setupWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void connectToAWS() {
  secureClient.setCACert(ca_root);
  secureClient.setCertificate(certificate);
  secureClient.setPrivateKey(private_key);

  mqttClient.setServer(mqtt_server, mqtt_port);
  while (!mqttClient.connected()) {
    Serial.print("Connecting to AWS IoT...");
    if (mqttClient.connect(mqtt_client_id)) {
      Serial.println("Connected to AWS IoT");
    } else {
      Serial.print("Failed with state ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

void publishSensorData() {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Prepare JSON payload
  String payload = "{\"temperature\":";
  payload += temp;
  payload += ", \"humidity\":";
  payload += humidity;
  payload += "}";

  // Publish to AWS IoT Core topic
  mqttClient.publish(mqtt_topic, payload.c_str());
  Serial.print("Published message: ");
  Serial.println(payload);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setupWiFi();
  connectToAWS();
}

void loop() {
  if (!mqttClient.connected()) {
    connectToAWS();
  }

  mqttClient.loop();
  publishSensorData();
  delay(5000);
}