#include <WiFi.h>
#include <WiFiClient.h>
#include <Pushsafer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFiMulti.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

// Initialize Wifi connection to the router
char ssid[] = "Telenor6950ovn";        
char password[] = "onicpjozepykk"; 

// Pushsafer private or alias key
#define PushsaferKey "qD43bQRbxCc0GD3U9cKX"

/*WiFiClientSecure client;*/
WiFiClient client;
Pushsafer pushsafer(PushsaferKey, client);
const char * hostDomain = "10.0.0.8"; //!!!
const int hostPort = 3001;

const int LED_PIN = LED_BUILTIN;

void connectToWiFi(const char * ssid, const char * pwd)
{
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi.begin(ssid, pwd); // start connecting to the wifi network

  while (WiFi.status() != WL_CONNECTED) 
  {
    // Blink LED while we're connecting:
    digitalWrite( LED_PIN, !digitalRead(LED_PIN) );
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void requestURL(const char * host, int port, int x, int y)
{
  Serial.println("Connecting to domain: " + String(host));

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("connection failed");
    return;
  }
  Serial.println("Connected!\n");

  // This will send the POST request to the server
  String dataToSendx ="X="+ String(x);
  String dataToSendy ="&Y="+ String(y);
  String dataToSend = dataToSendx + dataToSendy;

  int dataStringLength = dataToSend.length();
  client.print((String)"POST /api HTTP/1.1\r\n" +
               "Host: " + String(host) + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Content-Length: "+dataStringLength+"\r\n"+
               "Connection: close\r\n\r\n"+
               dataToSend);

// If something goes wrong, we need a timeout
  unsigned long timeout = millis();
  while (client.available() == 0)
  {
    if (millis() - timeout > 5000) 
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) 
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  // When we are finished, we close the connection
  Serial.println("closing connection");
  client.stop();
}


void setup() {

  Serial.begin(115200);
  connectToWiFi(ssid, password);
  delay(4000);
  
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  lis.setDataRate(LIS3DH_DATARATE_50_HZ);


  pushsafer.debug = true;
}

void loop() {
  lis.read();    

  // Serial.print("X:  "); Serial.print(lis.x);
  // Serial.print("  \tY:  "); Serial.print(lis.y);
  // Serial.print("  \tZ:  "); Serial.println(lis.z);

  // Check if both x and y values are over 5000
  if (lis.x > 5000 || lis.y > 5000) {
    requestURL(hostDomain,hostPort, lis.x, lis.y);
    // Create a Pushsafer input struct with the desired notification parameters
    struct PushSaferInput input;
    input.message = "!Bevegelse i heisen!";
    input.title = "Alert!";
    input.sound = "8";
    input.vibration = "1";
    input.icon = "1";
    input.iconcolor = "#FFCCCC";
    input.priority = "1";
    input.device = "a";
    input.url = "";
    input.urlTitle = "";
    input.picture = "";
    input.picture2 = "";
    input.picture3 = "";
    input.time2live = "";
    input.retry = "";
    input.expire = "";
    input.confirm = "";
    input.answer = "";
    input.answeroptions = "";
    input.answerforce = "";

    // Send the notification using the Pushsafer library
    Serial.println(pushsafer.sendEvent(input));
    Serial.println("Sent");
  }

  delay(1000);
}
