#include <Time.h>
#include <TimeLib.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>

//*****Pin layout*****//
// D1 - GPIO5 - SCL
// D2 - GPIO4 - SDA

// Server WiFi name
const char *ssid = "ESP8266_server";
// Server Wifi password
const char *password = "no_access";

//BMP280 object
Adafruit_BMP280 bmp;

// Server and data
File data;
ESP8266WebServer server(80);

unsigned long previousMillis = 0;
const unsigned long intervalToUpdate = 10000; // 10 seconds

void webpage()
{
  // Redirect to our txt file
  server.sendHeader("Location", "/index.html", true);
  server.send(302, "text/plane", "");
}

void dataHandler()
{
  // Redirect to our txt file
  server.sendHeader("Location", "/database.txt", true);
  server.send(302, "text/plane", "");
}

// Handle data file types
bool loadFromSpiffs(String path)
{
  String dataType = "text/plain";
  if (path.endsWith("/"))
    path += "index.htm";
  if (path.endsWith(".src"))
    path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html"))
    dataType = "text/html";
  else if (path.endsWith(".htm"))
    dataType = "text/html";
  else if (path.endsWith(".css"))
    dataType = "text/css";
  else if (path.endsWith(".js"))
    dataType = "application/javascript";
  else if (path.endsWith(".png"))
    dataType = "image/png";
  else if (path.endsWith(".gif"))
    dataType = "image/gif";
  else if (path.endsWith(".jpg"))
    dataType = "image/jpeg";
  else if (path.endsWith(".ico"))
    dataType = "image/x-icon";
  else if (path.endsWith(".xml"))
    dataType = "text/xml";
  else if (path.endsWith(".pdf"))
    dataType = "application/pdf";
  else if (path.endsWith(".zip"))
    dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download"))
    dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
  }
  dataFile.close();
  return true;
}

// Function to handle all web requests
void handleWebRequests()
{
  if (loadFromSpiffs(server.uri()))
    return;
  if (server.uri() == "\value")
  {
    String message = "File Not Detected\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
      message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
    Serial.println(message);
  }
}

// Initial setup functions
void setup()
{
  // Init serial
  Serial.begin(9600);
  SPIFFS.begin();

  SPIFFS.remove("/database.txt");
  data = SPIFFS.open("/database.txt", "a+");

  // Open server
  server.on("/", webpage);
  server.on("/data", dataHandler);
  server.onNotFound(handleWebRequests);

  // Start in AP mode
  WiFi.softAP(ssid, password);

  // Print the AP name
  Serial.print("AP name: ");
  Serial.println(ssid);

  // Print the IP address
  Serial.print("IP address is: ");
  Serial.println(WiFi.softAPIP());

  // BMP280 init
  bmp.begin(0x76);  // I2C

  server.begin();
}

// Infinite function
void loop()
{
  unsigned long currentMillis = millis();
  
  server.handleClient();

  if (currentMillis - previousMillis >= intervalToUpdate)
  {
    previousMillis = currentMillis;
    
    data.print("\nT = ");
    data.print(bmp.readTemperature());
    data.print(" *C");
    data.print(" at ");
    data.print(hour());
    data.print(":");
    data.print(minute());
    data.print(":");
    data.print(second());
  }
}
