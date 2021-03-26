#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";

String mySsid = "LEDSSSSSS";
bool apMode = true;

String ssid     = "";
String password = "";

// HTML web page to handle 2 input fields (input1, input2)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h1>
  <form action="/get">
    wifi ssid: <input type="text" name="input1"><br/>
    password: <input type="text" name="input2"> <br/>
    <input type="submit" value="Submit">
  </form><br></h1>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// switch to station mode and connect to wifi with submitted parameters
void connectToNetwork() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
  Serial.println("Connected to network");
  apMode = false;
}

void setup() {
  Serial.begin(115200);
  AP();
}

void loop() {
  if (!apMode) {
    Serial.println("in loop");
    delay(300);
  }
}

// setup access point, then deploy config page
void AP() {
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(mySsid.c_str());
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
  configPage();
}

// deploy server to handle config page
void configPage() {
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String p1;
    String p2;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      ssid = request->getParam(PARAM_INPUT_1)->value();
      p1 = PARAM_INPUT_1;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    if (request->hasParam(PARAM_INPUT_2)) {
      password = request->getParam(PARAM_INPUT_2)->value();
      p2 = PARAM_INPUT_2;
    }

    Serial.println(ssid);
    Serial.println(password);
    request->send(200, "text/html", "<h1>Connecting to " + ssid + "</h1>");
    connectToNetwork(); // PARAMS SUBMITTED! switch to station mode and connect to wifi with them
  });
  server.onNotFound(notFound);
  server.begin();
}
