#include <WiFiNINA.h>
#include <BH1750.h>
#include <Wire.h>

// WiFi credentials 
char ssid[] = "iPhone";
char pass[] = "9416306106";

// WiFi client to connect to IFTTT server
WiFiClient client;

// BH1750 light sensor instance
BH1750 lightMeter; 

// LED pin to indicate connection 
int ledCheck = 2;

String queryString = "?value1=57&value2=25";

// IFTTT webhook server details
char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME = "https://maker.ifttt.com/trigger/Sunlight/with/key/gGi5lX65RsckYaoBo3yI3L7bTpujJWBOdDY4lrp0eLF";

void setup() 
{
  WiFi.begin(ssid, pass);
  
  Serial.begin(9600); 
  
  // Set LED pin as output 
  pinMode(ledCheck, OUTPUT);

  // Initialize I2C communication and light sensor
  Wire.begin();
  lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));

  // Attempt to connect to the WiFi
  connectWiFi();
}


void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Lux: ");
  Serial.println(lux);
  
  // If the light level exceeds 550 lux, trigger the "Sunlight" webhook
  if (lux >= 550) {
    triggerWebhook("Sunlight");
  } 
  // If the light level is below 60 lux, trigger the "Sundown" webhook
  else if (lux < 60) {
    triggerWebhook("Sundown");
  }
  
  // Wait for 1 second 
  delay(1000); 
}

// Function to handle WiFi connection
void connectWiFi() {
  Serial.println("Attempting to connect to WiFi...");
  
  // Keep trying to connect until connected
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass); 
    delay(5000); 
    Serial.print("."); 
  }
  
  Serial.println("\nConnected to WiFi");
}

// Function to trigger the IFTTT webhook
void triggerWebhook(String eventName) 
{
  // If WiFi is disconnected, reconnect
  if (WiFi.status() != WL_CONNECTED) 
  {
    connectWiFi();
  }
  
  // If the client successfully connects to the IFTTT server
  if (client.connect("maker.ifttt.com", 80)) {
    client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); 
    
    delay(500); 
    client.stop(); // Close the connection
    Serial.println("Webhook triggered successfully");
  } 
  else {
    // If connection fails, print an error message
    Serial.println("Failed to connect to webhook server");
  }
}
