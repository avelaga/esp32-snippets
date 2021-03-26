# modified from https://audreybetsy.medium.com/get-to-know-esp32-11-html-form-as-input-to-esp32-b24351935826

#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h> // https://github.com/me-no-dev/AsyncTCP
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h> // https://github.com/me-no-dev/ESPAsyncWebServer

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "mywifi";
const char* password = "superstrongpassword";

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";

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

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    String newSsid;
    String newPassword;

    String p1;
    String p2;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      newSsid = request->getParam(PARAM_INPUT_1)->value();
      p1 = PARAM_INPUT_1;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    if (request->hasParam(PARAM_INPUT_2)) {
      newPassword = request->getParam(PARAM_INPUT_2)->value();
      p2 = PARAM_INPUT_2;
    }

    Serial.println(newSsid);
    Serial.println(newPassword);
    request->send(200, "text/html", "<h1>Connecting to " + newSsid+"</h1>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  
}
