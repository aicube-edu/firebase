#include "FirebaseESP8266.h" // 반드시 파이어베이스 라이브러리를 먼저 추가해야 합니다.
#include <ESP8266WiFi.h>
 
#define FIREBASE_HOST "실시간 데이터베이스 주소 (http:// 빼고, 마지막 / 빼고)" 
#define FIREBASE_AUTH "실시간 데이터베이스 비밀번호"
#define WIFI_SSID "와이파이 SSID" // 연결 가능한 wifi의 ssid
#define WIFI_PASSWORD "와이파이비밀번호" // wifi 비밀번호

#define pinLED1 D2
#define pinLED2 D3
#define pinTemp A0

bool MODE = false; // AutoMode(true), MannualMode(false)
float refTemp = 35.0;
 
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
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

void loop()
{
  /// ----- 온도센서 업로드(->파이어베이스) 부분 -----
  float valTemp = analogRead(pinTemp);
  valTemp = valTemp * 330/1023-50;
  Firebase.setInt(firebaseData, "Temperature", valTemp);
  
  /// ----- 자동/수동모드 수신(<-파이어베이스) 부분 -----  
  if(Firebase.getString(firebaseData, "MODE"))
  {
    String valStr = firebaseData.stringData();
    if (valStr == "0") MODE = false;
    else if  (valStr == "1") MODE = true;
  }

  /// ----- 수동모드일때 -----  
  if (MODE == false) // Mannual Mode
  {
    if(Firebase.getString(firebaseData, "LED1"))
    {
      String valStr = firebaseData.stringData();
      if (valStr == "true") digitalWrite(pinLED1, HIGH);
      else if (valStr == "false") digitalWrite(pinLED1, LOW);
    }
    if(Firebase.getString(firebaseData, "LED2"))
    {
      String valStr = firebaseData.stringData();
      if (valStr == "true") digitalWrite(pinLED2, HIGH);
      else if (valStr == "false") digitalWrite(pinLED2, LOW);
    }
  }
  /// ----- 자동모드일때 (기준온도(refTemp)에 따라 구분) -----
  else if (MODE = true)
  {
    if(valTemp<refTemp) // cold
    {
      digitalWrite(pinLED1, LOW); // red led off
      digitalWrite(pinLED2, HIGH); // blue led on
      Firebase.setString(firebaseData, "LED1", "false");
      Firebase.setString(firebaseData, "LED2", "true");
    }
    else // hot
    {
      digitalWrite(pinLED1, HIGH); // red led on
      digitalWrite(pinLED2, LOW); // blue led off
      Firebase.setString(firebaseData, "LED1", "true");
      Firebase.setString(firebaseData, "LED2", "false");
    }
  }
 delay(1000); // 1초마다 반복
}
