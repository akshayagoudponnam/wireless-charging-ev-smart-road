#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <stdlib.h>
#include <dht.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

#define dht_dpin A0

double voltdiv;
int i, j;
dht DHT;

long t1 = 0, t2 = 0, t3 = 0, t4 = 0;
char buf1[16], buf2[16], buf3[16], buf4[16];

String strvoltdiv;
String stri;
String strj;
String strper;

/*
  Credentials are intentionally not included in this public repository version.
  Set them locally before uploading the sketch.
*/
String apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";
String wifiSSID = "YOUR_WIFI_SSID";
String wifiPassword = "YOUR_WIFI_PASSWORD";

SoftwareSerial ser(5, 6); // RX, TX

double mapf(double val, double in_min, double in_max,
            double out_min, double out_max) {
  return (val - in_min) * (out_max - out_min) /
         (in_max - in_min) + out_min;
}

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Sensors ");
  lcd.setCursor(0, 1);
  lcd.print("using IOT");
  delay(2000);
  lcd.clear();

  analogReference(DEFAULT);
  Serial.begin(9600);
  ser.begin(115200);

  // ESP8266 initialization using AT commands
  ser.println("AT+RST");
  delay(500);
  ser.println("AT+CWMODE=3");
  delay(500);

  String joinCmd = "AT+CWJAP=\"" + wifiSSID + "\",\"" + wifiPassword + "\"";
  ser.println(joinCmd);
  delay(500);
}

void loop() {
  DHT.read11(dht_dpin);
  i = DHT.humidity;
  j = DHT.temperature;

  voltdiv = mapf(analogRead(A1), 0, 1023, 0, 5);
  int per = (int)mapf(voltdiv, 3.5, 4.3, 0, 100);

  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltdiv);
  lcd.print(" ");

  lcd.setCursor(8, 0);
  lcd.print("B:");
  lcd.print(per);
  lcd.print(" % ");

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(i);
  lcd.print(" ");

  lcd.setCursor(8, 1);
  lcd.print("T:");
  lcd.print(j);
  lcd.print(" ");

  strvoltdiv = dtostrf(voltdiv * 1000, 4, 1, buf1);
  strper = dtostrf(per, 4, 1, buf2);
  stri = dtostrf(i, 4, 1, buf3);
  strj = dtostrf(j, 4, 1, buf4);

  Serial.print(strvoltdiv); Serial.print(" ");
  Serial.print(strper); Serial.print(" ");
  Serial.print(stri); Serial.print(" ");
  Serial.print(strj); Serial.print(" ");
  Serial.println();

  // ThingSpeak field 1: voltage
  if (millis() - t1 > 6000) {
    t1 = millis();
    String cmd = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80";
    ser.println(cmd);
    if (ser.find("Error")) return;

    String getStr = "GET /update?api_key=" + apiKey +
                    "&field1=" + String(strvoltdiv) + "\r\n\r\n";

    cmd = "AT+CIPSEND=" + String(getStr.length());
    ser.println(cmd);
    if (ser.find(">")) {
      ser.print(getStr);
    } else {
      ser.println("AT+CIPCLOSE");
    }
  }

  // ThingSpeak field 4: battery percentage
  if (millis() - t2 > 8000) {
    t2 = millis();
    String cmd = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80";
    ser.println(cmd);
    if (ser.find("Error")) return;

    String getStr = "GET /update?api_key=" + apiKey +
                    "&field4=" + String(strper) + "\r\n\r\n";

    cmd = "AT+CIPSEND=" + String(getStr.length());
    ser.println(cmd);
    if (ser.find(">")) {
      ser.print(getStr);
    } else {
      ser.println("AT+CIPCLOSE");
    }
  }

  // ThingSpeak field 2: humidity
  if (millis() - t3 > 3000) {
    t3 = millis();
    String cmd = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80";
    ser.println(cmd);
    if (ser.find("Error")) return;

    String getStr = "GET /update?api_key=" + apiKey +
                    "&field2=" + String(stri) + "\r\n\r\n";

    cmd = "AT+CIPSEND=" + String(getStr.length());
    ser.println(cmd);
    if (ser.find(">")) {
      ser.print(getStr);
    } else {
      ser.println("AT+CIPCLOSE");
    }
  }

  // ThingSpeak field 3: temperature
  if (millis() - t4 > 5000) {
    t4 = millis();
    String cmd = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80";
    ser.println(cmd);
    if (ser.find("Error")) return;

    String getStr = "GET /update?api_key=" + apiKey +
                    "&field3=" + String(strj) + "\r\n\r\n";

    cmd = "AT+CIPSEND=" + String(getStr.length());
    ser.println(cmd);
    if (ser.find(">")) {
      ser.print(getStr);
    } else {
      ser.println("AT+CIPCLOSE");
    }
  }
}
