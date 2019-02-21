#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef STASSID
#define STASSID "DKW@Home"
#define STAPSK "@yxgq6403sjb8706001"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = D4;
const int swh = D1;
const int sig = D8;

void handleRoot() {
  digitalWrite(led, 0);
  server.send(200, "text/html", "<html><head><title>Remote Power Supply Control</title><meta content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0\" name=\"viewport\"><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><style>body{background-color:#222}center{height:326px;margin-top:200px}@font-face{font-family:neon;src:url(\"http://static2.dingstudio.cn/fonts/neon.ttf\")}.btn{width:150px;border-radius:5px;padding:15px 25px;font-size:22px;text-decoration:none;margin:20px;color:#fff;position:relative;display:inline-block}.btn:active{transform:translate(0,5px);-webkit-transform:translate(0,5px);box-shadow:0 1px 0 0}.blue{background-color:#55acee;box-shadow:0 5px 0 0 #3c93d5}.blue:hover{background-color:#6fc6ff}.red{background-color:#e74c3c;box-shadow:0 5px 0 0 #ce3323}.red:hover{background-color:#ff6656}.neon{font-size:64px;font-family:neon;color:#fb4264;font-size:9vw;line-height:9vw;text-shadow:0 0 3vw #f40a35}.neon{animation:neon 1s ease infinite;-moz-animation:neon 1s ease infinite;-webkit-animation:neon 1s ease infinite}@keyframes neon{0%,100%{text-shadow:0 0 1vw #fa1c16,0 0 3vw #fa1c16,0 0 10vw #fa1c16,0 0 10vw #fa1c16,0 0 .4vw #fed128,.5vw .5vw .1vw #806914;color:#fed128}50%{text-shadow:0 0 .5vw #800e0b,0 0 1.5vw #800e0b,0 0 5vw #800e0b,0 0 5vw #800e0b,0 0 .2vw #800e0b,.5vw .5vw .1vw #40340a;color:#806914}}</style></head><body><center><h1 class=\"neon\" style=\"font-size:64px\">RPSC</h1><a class=\"btn blue\" onclick=\"touch()\">短按电源键</a><br><a class=\"btn red\" onclick=\"press()\">长按电源键</a></center><script>function touch(){var xhr=new XMLHttpRequest();xhr.onreadystatechange=()=>{if(xhr.readyState==4){if(xhr.status==200){alert(\"已点击电源按钮！\")}else{alert(\"侦测到异常发生!\")}}};xhr.open(\"GET\",\"/toggle\",true);xhr.send()}function press(){var truthBeTold=confirm(\"确定要长按吗？\");if(truthBeTold==true){var xhr=new XMLHttpRequest();xhr.onreadystatechange=()=>{if(xhr.readyState==4){if(xhr.status==200){alert(\"已长按电源按钮！\")}else{alert(\"侦测到异常发生!\")}}};xhr.open(\"GET\",\"/forceshutdown\",true);xhr.send()}else{alert(\"已取消！\")}}</script></body></html>");
  digitalWrite(led, 1);
}

void handleToggle() {
  digitalWrite(led, 0);
  digitalWrite(swh, 1);
  delay(100);
  digitalWrite(swh, 0);
  server.send(200, "text/plain", "Power Toggled!");
  digitalWrite(led, 1);
}

void handleForceShutdown() {
  digitalWrite(led, 0);
  digitalWrite(swh, 1);
  delay(5000);
  digitalWrite(swh, 0);
  server.send(200, "text/plain", "Force Shutdowned!");
  digitalWrite(led, 1);
}

void handleStatus() {
  digitalWrite(led, 0);
  if (digitalRead(sig)==HIGH){
    server.send(200, "text/plain", "1");
  }else{
    server.send(200, "text/plain", "0");
  }
  digitalWrite(led, 1);
}

void handleNotFound() {
  digitalWrite(led, 0);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 1);
}

void setup(void) {
  pinMode(led, OUTPUT);
  pinMode(swh, OUTPUT);
  pinMode(sig, INPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/toggle", handleToggle);

  server.on("/forceshutdown", handleForceShutdown);

  server.on("/status", handleStatus);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(led, 1);
}

void loop(void) {
  server.handleClient();
}
