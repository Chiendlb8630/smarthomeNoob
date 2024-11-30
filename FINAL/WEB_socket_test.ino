#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "chien";
const char* password = "ngochai97";

const char* websocket_server = "192.168.52.243";
const int websocket_port = 8080;

WebSocketsClient webSocket;

const int trigPin = 13;  // Chân Trig của cảm biến
const int echoPin = 12; // Chân Echo của cảm biến, đổi sang D12

int dist; 


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected from WebSocket server");
      break;
    case WStype_CONNECTED:
      Serial.println("Connected to WebSocket server");
      break;
    case WStype_TEXT:
      Serial.printf("Message from server: %s\n", payload);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Kết nối WebSocket server
  webSocket.begin(websocket_server, websocket_port, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();  // Duy trì kết nối WebSocket
  for(int i = 0; i <= 360; i++) {
    
    delay(10);            // Chờ 15ms để servo có thời gian quay đến vị trí mới
    dis();
  
    Serial.print(i); // Sends the current degree into the Serial Port
    Serial.print(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    Serial.print(dist); // Sends the distance value into the Serial Port
    Serial.print("."); // Sends addition character right next to the previo
    
    String message =  String(i) + ", " + String(dist) ;
    webSocket.sendTXT(message);  // Gửi giá trị qua WebSocket
    // Serial.println(message);  // Hiển thị giá trị trên Serial Monitor
    webSocket.loop();  // Duy trì kết nối WebSocket trong quá trình gửi
    
  }

  for (int i = 360 ; i >= 0; i--) {
     

                // Chờ 15ms để servo có thời gian quay đến vị trí mới
    dis();
  
    Serial.print(i); // Sends the current degree into the Serial Port
    Serial.print(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    Serial.print(dist); // Sends the distance value into the Serial Port
    Serial.print("."); // Sends addition character right next to the previo
    
    
    String message =  String(i) + ", " + String(dist) ;
    webSocket.sendTXT(message);  // Gửi giá trị qua WebSocket
    
    webSocket.loop();  // Duy trì kết nối WebSocket trong quá trình gửi
     
  }


}

void dis() {
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
   dist = duration * 0.034 / 2;
  delay(500); // Đợi nửa giây trước khi đo lại
}







