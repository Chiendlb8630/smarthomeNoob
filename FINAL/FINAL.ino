#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <DHT.h>
#include <DHT_U.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

const char* ssid = "chien";
const char* password = "ngochai97";
const char* firebaseHost = "crud-test-e6735-default-rtdb.asia-southeast1.firebasedatabase.app";
const char* apiKey = "2t5hXQh3f5eFPNuiFthpykASxAeyy805qbcE1cT1";
const char* databasePath = "/LCD.json";

Servo myServo;
#define SS_PIN 27    
#define RST_PIN 26   
#define SPI_CLOCK 1000000  // 1MHz cho giao tiếp SPI
#define DHTPIN 4  
#define DHTTYPE DHT11  
#define BUTTON_PIN 25 
#define servoPin 15

<<<<<<< HEAD
=======

>>>>>>> e40df3b5072cb068f6d982cd4a13bb94c071e4cf
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
HTTPClient http;
String url;
int httpResponseCode;
int sensor;
MFRC522 rfid(SS_PIN, RST_PIN);

<<<<<<< HEAD

=======
>>>>>>> e40df3b5072cb068f6d982cd4a13bb94c071e4cf
unsigned long lastMillis = 0;   
const unsigned long interval = 1500;  // Khoảng thời gian giữa các lần gửi dữ liệu (ms)


int lastButtonState = HIGH ; 

void setup() {
  myServo.attach(servoPin);  
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  dht.begin();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pinMode(34,INPUT);  
  Serial.begin(115200);    
  pinMode(5 , OUTPUT);  
  pinMode(13,OUTPUT); 
  pinMode(17,OUTPUT);

  SPI.begin();  // SCK = GPIO 18, MOSI = GPIO 23, MISO = GPIO 19
  SPI.setClockDivider(SPI_CLOCK);  // Đặt tốc độ SPI thấp hơn để đảm bảo ổn định
  rfid.PCD_Init();
  Serial.println("Đang chờ thẻ RFID...");
  myServo.write(0);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}
void loop() {
  unsigned long currentMillis = millis();  // Lấy thời gian hiện tại
  int buttonState = digitalRead(BUTTON_PIN);  
  int sensor = digitalRead(34);

  // Kiểm tra kết nối WiFi
  if (WiFi.status() == WL_CONNECTED) {

    // Kiểm tra cảm biến
    if (sensor == LOW) {
      sendFire();  // Gửi dữ liệu về tình trạng lửa
    }

    // Kiểm tra nút bấm
    if (buttonState == LOW && lastButtonState == HIGH ) {
      sendStatus();  // Gửi status lên Firebase
    }

    // Cập nhật trạng thái của nút
    lastButtonState = buttonState;  
    RFID();

    // Kiểm tra thời gian gửi dữ liệu DHT và cập nhật LCD
    if (currentMillis - lastMillis >= interval) {
      sendDHT();    // Gửi dữ liệu DHT
      LCD_write();  // Cập nhật LCD
      LiGHT_SWITCH();
      LiGHT_SWITCH2();
      getDoor();
<<<<<<< HEAD
      
      lastMillis = currentMillis;  // Cập nhật thời gian cuối cùng gửi dữ liệu
    }
  }

=======
      lastMillis = currentMillis;  // Cập nhật thời gian cuối cùng gửi dữ liệu
    }
  }
>>>>>>> e40df3b5072cb068f6d982cd4a13bb94c071e4cf
}

void displayMessage(String message) {
  lcd.clear();  // Xóa màn hình

  String line1 = message.substring(0, 16);  // Lấy 16 ký tự đầu tiên
  String line2 = "";

 
  if (message.length() > 16) {
    line2 = message.substring(16, 32);  
  }
  lcd.setCursor(0, 0);
  lcd.print(line1);

  if (line2.length() > 0) {
    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
}


void sendDHT(){
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
   
  url = String("https://") + firebaseHost +"/sensor/dht11.json?auth=" + apiKey; // url den firebase/sensor/dht11
  // goi tin json 
  String jsonPayload = "{\"Do am\":";
  jsonPayload += hum;
  jsonPayload += ", \" nhiet do\":"; 
  jsonPayload += temp;
  jsonPayload += "}";

  // Gui nhiet do, do am len firebase 
  http.begin(url); 
  http.addHeader("Content-Type", "application/json"); 
  httpResponseCode = http.PUT(jsonPayload); 
  if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println("Gửi thành công");
    } else {
      Serial.print("Lỗi khi gửi dữ liệu: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Kết thúc PUT request 
}


void LCD_write(){

    url = String("https://") + firebaseHost + databasePath + "?auth=" + apiKey;
    
    http.begin(url); // Khởi tạo kết nối HTTP
    httpResponseCode = http.GET(); // Gửi yêu cầu GET

    if (httpResponseCode > 0) {
      String payload2 = http.getString(); // Nhận dữ liệu JSON từ Firebase
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Payload: " + payload2);

      // Phân tích chuỗi JSON từ Firebase
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload2);

      if (!error) {
        if (doc.containsKey("String")) {
          String chuoi = doc["String"].as<String>(); // Lấy giá trị từ JSON
          displayMessage(chuoi);
        } else {
          Serial.println("Trường 'String' không tồn tại trong JSON.");
        }
      } else {
        Serial.print("Lỗi phân tích JSON: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("Lỗi HTTP: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Kết thúc kết nối HTTP
}
void sendStatus(){
  url = String("https://") + firebaseHost +"/POST/data.json?auth=" + apiKey;
  String payload9 = "{\"status\": 1}";

  http.begin(url);
      // URL của Firebase Realtime Database
      http.addHeader("Content-Type", "application/json");  
      // Đặt tiêu đề Content-Type

  httpResponseCode = http.PUT(payload9);  // Gửi PUT request
      digitalWrite(5,1);
      delay(300);
      payload9 = "{\"status\": 0}";


     http.PUT(payload9);

     if (httpResponseCode > 0) {
        String response = http.getString();
        delay(100);
        digitalWrite(5,0);
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        Serial.println("Response: " + response);
     }
     else{
      Serial.println(" loi khi PUT status");
     }

     http.end();
}

void sendFire(){

  url = String("https://") + firebaseHost + "/POST/chay.json?auth=" + apiKey;
  String payload = "{\"Fire\": 1}"; 
  http.begin(url);
  http.addHeader("Content-Type", "application/json"); 
  http.PUT(payload);

  payload = "{\"Fire\": 0}"; 
  http.PUT(payload);
  http.end();
  ///******************************///
}

<<<<<<< HEAD

void LiGHT_SWITCH(){
  url = String("https://") + firebaseHost + "/LIGHT/Light1.json?auth=" + apiKey;
  
=======
void LiGHT_SWITCH(){
  url = String("https://") + firebaseHost + "/LIGHT/Light1.json?auth=" + apiKey;

>>>>>>> e40df3b5072cb068f6d982cd4a13bb94c071e4cf
  http.begin(url);
  //***************************// request for TURN LIGHT ON/OFF  
  http.GET();
  String data = http.getString();  

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, data);

<<<<<<< HEAD
=======

>>>>>>> e40df3b5072cb068f6d982cd4a13bb94c071e4cf
    if (!error) {
        int ledState = doc["status"];  // Lấy giá trị từ trường "state" trong Firebase

        // Điều khiển đèn LED
        if (ledState == 1) {
          digitalWrite(13 ,  HIGH );  // Bật đèn
          Serial.println("LED ON");

        } else {
          digitalWrite(13 , LOW);  // Tắt đèn
          Serial.println("LED OFF");
        }
      }
    http.end(); 
}
<<<<<<< HEAD
=======

>>>>>>> e40df3b5072cb068f6d982cd4a13bb94c071e4cf
void LiGHT_SWITCH2(){
  url = String("https://") + firebaseHost + "/LIGHT/Light2.json?auth=" + apiKey;
  
  http.begin(url);
  //***************************// request 2 for TURN LIGHT ON/OFF  
  http.GET();
  String data = http.getString();  

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, data);

    if (!error) {
        int ledState1 = doc["status1"];  

        // Điều khiển đèn LED
        if (ledState1 == 1) {
          digitalWrite(17 ,  HIGH);  
          Serial.println("LED2 ON");

        } else {
          digitalWrite(17 , LOW);  
          Serial.println("LED2 OFF");
        }
      }
    http.end(); 
}
void RFID() {
  if (!rfid.PICC_IsNewCardPresent()) {
    delay(50);  // Đợi 50ms để tránh làm tốn CPU
    return;
  }
  if (!rfid.PICC_ReadCardSerial()) {
    delay(50);  // Đợi 50ms trước khi thử lại
    return;
  }
   
  // Khi thẻ được đọc thành công, in ra UID của thẻ
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.trim();
  Serial.print("UID quét được: ");
  Serial.println(uid);

  
  url = String("https://") + firebaseHost + "/RFID.json?auth=" + apiKey;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload10 = http.getString();
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload10);
   if (!error) {
      
      String code1 = doc["code1"];
      String code2 = doc["code2"];
      String code3 = doc["code3"];
      String code4 = doc["code4"];
      String code5 = doc["code5"];

      if (uid.equalsIgnoreCase(code1) || uid.equalsIgnoreCase(code2) || uid.equalsIgnoreCase(code3) || uid.equalsIgnoreCase(code4) || uid.equalsIgnoreCase(code5)) {
        Serial.println("Mã RFID hợp lệ, mở cửa");
        myServo.write(90); 

        delay(5000);
        myServo.write(0); 
      } else {
        Serial.println("Mã RFID không hợp lệ.");
      }
    }
  } else {
    Serial.print("Lỗi kết nối Firebase: ");
    Serial.println(httpCode);
  }
  http.end();
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1000);
}

<<<<<<< HEAD


void getDoor() {
  
=======
void getDoor() {
>>>>>>> e40df3b5072cb068f6d982cd4a13bb94c071e4cf
    url = String("https://") + firebaseHost + "/POST/Door.json?auth=" + apiKey; 
    http.begin(url);
    int httpCode = http.GET();  // Thực hiện GET request

    if (httpCode > 0) {  // Nếu kết nối thành công
      String payload3 = http.getString();  // Lấy dữ liệu trả về dưới dạng String
      Serial.println(payload3);

      // Phân tích JSON
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload3);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
        return;
      }

      // Đọc giá trị của status từ JSON
      int status = doc["status"];  // Truy xuất trường "status" từ JSON

      if (status == 1) {
        Serial.println("Status is 1, turning servo to 90 degrees");
        myServo.write(0); 
      } else {
        Serial.println("Status is not 1, keeping servo at 0 degrees");
        myServo.write(90);  // Đặt servo về 0 độ
      }
    } else {
      Serial.println("Error on HTTP request");
    }
<<<<<<< HEAD

    http.end();  // Đóng kết nối HTTP
  
=======
    http.end();  // Đóng kết nối HTTP
>>>>>>> e40df3b5072cb068f6d982cd4a13bb94c071e4cf
}



 









