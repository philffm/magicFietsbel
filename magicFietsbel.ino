
#include <dummy.h>
#include <WiFi.h>

#include <HTTPClient.h>
#include <WiFiManager.h>         

WiFiManager wifiManager;

// Everything slack
#include "slackSecrets.h"
#include "cyclingPrompts.h"

String MESSAGE = "Someone rang the fietsbel woohoo.";


// ESP32 Touch Test
// Just test touch pin - Touch0 is T0 which is on GPIO 4.


// https://hooks.slack.com/services/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
// LED will blink when in config mode

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

//for LED status
#include <Ticker.h>
Ticker ticker;


#endif

int LED = LED_BUILTIN;

void tick()
{
  //toggle state
  digitalWrite(LED, !digitalRead(LED));     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void slack() {
  HTTPClient http;
  http.begin(SlackHookURL); 
  http.addHeader("Content-Type", "application/json");

//  char* currentPrompt = cyclingPrompts[random(sizeof(cyclingPrompts) - 1)];
  String currentPrompt = "Test prompt";

  int httpCode = http.POST("{'username':'magicFietsbel', 'text':':bike::bell: DING DING - Ga lekker fietsen ! " + currentPrompt + "'}");
  
  
  if(httpCode > 0) {
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  //http.end();
  Serial.println ("Slack done");
}


void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);

  Serial.println(SlackHookURL);
  //set led pin as output
  pinMode(LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - for testing
  // wm.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"

  wifiManager.autoConnect("magicFietsbel");

  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(LED, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  int touchFietsbel = touchRead(T0);
  if (touchFietsbel<30){
    slack();
  }

  Serial.println(touchRead(T0));  // get value using T0
  delay(1000 );
}


// void post() {
//   Serial.println("Connecting to host...");
//   WiFiClientSecure client;
//   if (!client.connect(HOST, 443)) {
//     Serial.println("Connection failed");
//     client.stop();
//     return;
//   }
//   Serial.println("Connected to host");

//   // Send a POST request to your Slack webhook:
//   // {"text": "Someone pushed the big red button."}

//   String request = "";
//   request += "POST ";
//   request += URL;
//   request += " HTTP/1.1\r\n";

//   request += "Host: ";
//   request += HOST;
//   request += "\r\n";

//   int len = MESSAGE.length() + 12;  // JSON wrapper length
//   request += "Content-Length: ";
//   request += len;
//   request += "\r\n";

//   request += "Accept: application/json\r\n";
//   request += "Connection: close\r\n";
//   request += "Content-Type: application/json\r\n";

//   request += "\r\n";
  
//   request += "{\"text\": \"";
//   request += MESSAGE;
//   request += "\"}";

//   Serial.print(request);
//   Serial.println();
//   client.print(request);

//   long timeout = millis() + 5000;
//   while (!client.available()) {
//     if (millis() > timeout) {
//       Serial.println("Request timed out");
//       client.stop();
//       return;
//     }
//   }
//   Serial.println("Response:");
//   while (client.available()) {
//     Serial.write(client.read());
//   }
//   Serial.println();
//   Serial.println("Request complete");
// }
