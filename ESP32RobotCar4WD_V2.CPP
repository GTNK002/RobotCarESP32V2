#include <Servo.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "MyWiFiCar";//Airtel_shob_7000_EXT
const char* password = "12345678";//air21747

int sw1_val = 0;
int ultra_stop_cm = 20;//cm 
int motor_delay = 0;

int inbuild_Led = 2;
int touch_Sensor = 13;
int left_IR = 14;
int right_IR = 15;
int servo_1 = 4;
int trig = 26;
int echo = 25;
int motor_t1 = 19;
int motor_t2 = 32;
int motor_t3 = 33;
int motor_t4 = 18;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>NK Smart Home</h1>
  </div>
  <div class="content">
    <div class="card">
      <table>
        <tr>
          <td><button class="button" onmousedown="sendws('leftF');" ontouchstart="sendws('leftF');" onmouseup="sendws('stop');" ontouchend="sendws('stop');">F Left</button></td>
          <td><button class="button" onmousedown="sendws('forward');" ontouchstart="sendws('forward');" onmouseup="sendws('stop');" ontouchend="sendws('stop');">Forward</button></td>
          <td><button class="button" onmousedown="sendws('rightF');" ontouchstart="sendws('rightF');" onmouseup="sendws('stop');" ontouchend="sendws('stop');">F Right</button></td>
        </tr>
        <tr>
          <td><button class="button" onmousedown="sendws('left');" ontouchstart="sendws('left');" onmouseup="sendws('stop');" ontouchend="sendws('stop');">Left</button></td>
          <td><button class="button" onmousedown="sendws('stop');" ontouchstart="sendws('stop');" onmouseup="sendws('stop');" ontouchend="sendws('stop');">Stop</button></td>
          <td><button class="button" onmousedown="sendws('right');" ontouchstart="sendws('right');" onmouseup="sendws('stop');" ontouchend="sendws('stop');">Right</button></td>
        </tr>
        <tr>
          <td><button class="button" onmousedown="sendws('leftB');" ontouchstart="sendws('leftB');" onmouseup="sendws('stop');" ontouchend="sendws('stop');">B Left</button></td>
          <td><button class="button" onmousedown="sendws('backward');" ontouchstart="sendws('backward');" onmouseup="sendws('stop');" ontouchend="sendws('stop');">Backward</button></td>
          <td><button class="button" onmousedown="sendws('rightB');" ontouchstart="sendws('rightB');" onmouseup="sendws('stop');" ontouchend="sendws('stop');">B Right</button></td>
        </tr>
      </table>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onLoad(event) {
    initWebSocket();
  }
  function sendws(x) {
    websocket.send(x);
  }
</script>
</body>
</html>
)rawliteral";

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;

    if (strcmp((char*)data, "leftF") == 0) {
      m_left();
    }
    else if (strcmp((char*)data, "forward") == 0) {
      m_forward();
    }
    else if (strcmp((char*)data, "rightF") == 0) {
      m_left();
    }
    else if (strcmp((char*)data, "left") == 0) {
      m_left();
    }
    else if (strcmp((char*)data, "stop") == 0) {
      m_stop();
    }
    else if (strcmp((char*)data, "right") == 0) {
      m_right();
    }
    else if (strcmp((char*)data, "leftB") == 0) {
      m_right();
    }
    else if (strcmp((char*)data, "backward") == 0) {
      m_backward();
    }
    else if (strcmp((char*)data, "rightB") == 0) {
      m_right();
    }
  }
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  return String();
}

Servo servo1;

long timeInMicro;

long distanceInCm;

void m_stop () {
  digitalWrite(motor_t1, LOW);
  digitalWrite(motor_t2, LOW);
  digitalWrite(motor_t3, LOW);
  digitalWrite(motor_t4, LOW);
  delay(motor_delay);
}

void m_forward () {
  digitalWrite(motor_t1, HIGH);
  digitalWrite(motor_t2, HIGH);
  digitalWrite(motor_t3, LOW);
  digitalWrite(motor_t4, LOW);
  delay(motor_delay);
}

void m_left () {
  digitalWrite(motor_t1, HIGH);
  digitalWrite(motor_t2, LOW);
  digitalWrite(motor_t3, LOW);
  digitalWrite(motor_t4, LOW);
  delay(motor_delay);
}

void m_right () {
  digitalWrite(motor_t1, LOW);
  digitalWrite(motor_t2, HIGH);
  digitalWrite(motor_t3, LOW);
  digitalWrite(motor_t4, LOW);
  delay(motor_delay);
}

void m_backward () {
  digitalWrite(motor_t1, LOW);
  digitalWrite(motor_t2, LOW);
  digitalWrite(motor_t3, HIGH);
  digitalWrite(motor_t4, HIGH);
  delay(motor_delay);
}

void setup() {
  Serial.begin(115200);
  pinMode(motor_t1,OUTPUT);
  pinMode(motor_t2,OUTPUT);
  pinMode(motor_t3,OUTPUT);
  pinMode(motor_t4,OUTPUT);
  pinMode(inbuild_Led, OUTPUT);
  pinMode(touch_Sensor, INPUT);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);

  /*WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  */

  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  initWebSocket();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.begin();

  servo1.attach(servo_1);
  servo1.write(90);
}

void loop() {
  ws.cleanupClients();
  if(digitalRead(touch_Sensor) == 1) {
    if(sw1_val == 0) {
      sw1_val = 1;
      digitalWrite(inbuild_Led, HIGH);
    }
    else if(sw1_val == 1) {
      sw1_val = 2;
      for(int i=1; i<=5; i++) {
        digitalWrite(inbuild_Led, LOW);
        delay(500);
        digitalWrite(inbuild_Led, HIGH);
        delay(500);
      }
    }
    else if(sw1_val == 2) {
      sw1_val = 0;
      digitalWrite(inbuild_Led, LOW);
    }
    Serial.println("Sensing");
    delay(1000);
  }
  else if (sw1_val == 0) {
    if(digitalRead(left_IR) == 1 && digitalRead(right_IR) == 1) {
      //Serial.println("Stop");
      m_stop();
    }
    else if (digitalRead(left_IR) == 0 && digitalRead(right_IR) == 1) {
      //Serial.println("left");
      m_left();
    }
    else if (digitalRead(left_IR) == 1 && digitalRead(right_IR) == 0) {
      //Serial.println("Right");
      m_right();
    }
    else if (digitalRead(left_IR) == 0 && digitalRead(right_IR) == 0) {
      //Serial.println("Forward");
      m_forward();
    }
  }
  else if (sw1_val == 1) {
    digitalWrite(trig,LOW);
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(trig,LOW);
    timeInMicro= pulseIn(echo,HIGH);
    distanceInCm = ((timeInMicro/29)/2);
    Serial.println(distanceInCm);
    delay(100);
    if (distanceInCm <= ultra_stop_cm) {
      m_stop();
      delay(500);
      servo1.write(0);
      delay(2000);
      servo1.write(180);
      delay(2000);
      servo1.write(90);
      delay(2000);
      m_backward();
      delay(380);
      //Serial.println("left");
      m_left();
      delay(380);
    }
    else if (digitalRead(left_IR) == 0 && digitalRead(right_IR) == 1) {
      //Serial.println("left");
      m_backward();
      delay(380);
      m_right();
      delay(380);
      m_stop();
    }
    else if (digitalRead(left_IR) == 1 && digitalRead(right_IR) == 0) {
      //Serial.println("Right");
      m_backward();
      delay(380);
      m_left();
      delay(380);
      m_stop();
    }
    else {
      //Serial.println("forward");
      m_forward();
      delay(92);
      m_stop();
    }
  }
}
