#include "FirebaseESP8266.h" // 반드시 파이어베이스 라이브러리를 먼저 추가해야 합니다.
#include <ESP8266WiFi.h>
 
#define FIREBASE_HOST "실시간 데이터베이스 주소 (http:// 빼고, 마지막 / 빼고)" 
#define FIREBASE_AUTH "실시간 데이터베이스 비밀번호"
#define WIFI_SSID "와이파이 SSID" // 연결 가능한 wifi의 ssid
#define WIFI_PASSWORD "와이파이비밀번호" // wifi 비밀번호

#define pinSW1  D2
#define pinSW2  D3
#define pinLED1 D4
#define pinLED2 D7
#define pinCDS  A0
 
FirebaseData firebaseData;
FirebaseJson json;
 
void printResult(FirebaseData &data);
void setup() // wifi 접속 과정.
{
  Serial.begin(9600);
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println();
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  pinMode(pinSW1, INPUT);
  pinMode(pinSW2, INPUT);
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

void loop()
{
  Firebase.setBool(firebaseData, "Switch Left", !digitalRead(pinSW1));
  digitalWrite(pinLED1,firebaseData.boolData());
  
  Firebase.setBool(firebaseData, "Switch Right", !digitalRead(pinSW2));
  digitalWrite(pinLED2,firebaseData.boolData());

  Firebase.setInt(firebaseData, "Brightness", analogRead(pinCDS));
  delay(1000); // 1초마다 반복
}
