SOURCE CODE
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <map>
// WiFi credentials
const char* ssid = "embedded";
const char* password = "embedded123";
// Telegram Bot Token & Chat IDs
String botToken = "7839559906:AAG0V0SKrbi5nFWTjY6JxmbU-2cbLvKSwJU";
String chatID1 = "7098153506"; // Management
String chatID2 = "1811014913"; // Parent
// RFID setup
#define SS_PIN 21
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN);
// UID status map to track board/depart
std::map<String, bool> studentStatus;
String getStudentName(String uid) {
if (uid == "AD412221") return "RANVEER KAPUR";
else if (uid == "C32D0502") return "PREITY MUKUNDHAN";
else return "Unknown Student";
}
void sendTelegramAlert(String msg) {
HTTPClient http;
Serial.println("[INFO] Sending message to management...");
String url1 = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatID1 + "&text=" + msg;
http.begin(url1);
int code1 = http.GET();
Serial.println("[DEBUG] Management Response Code: " + String(code1));
http.end();
delay(500);
Serial.println("[INFO] Sending message to parent...");
String url2 = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatID2 + "&text=" + msg;
http.begin(url2);
int code2 = http.GET();
Serial.println("[DEBUG] Parent Response Code: " + String(code2));
http.end();
}
void setup() {
Serial.begin(115200);
SPI.begin();
rfid.PCD_Init();
Serial.println("[BOOT] Starting system...");
WiFi.begin(ssid, password);
Serial.print("[WiFi] Connecting");
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("\n[WiFi] Connected successfully!");
sendTelegramAlert("School Bus RFID Attendance System Started.");
}
void loop() {
if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;
String uid = "";
for (byte i = 0; i < rfid.uid.size; i++) {
uid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
uid += String(rfid.uid.uidByte[i], HEX);
}
uid.toUpperCase();
Serial.println("\n[RFID] Card Detected!");
Serial.println("[RFID] UID: " + uid);
String name = getStudentName(uid);
bool boarded = studentStatus[uid]; // false by default
if (!boarded) {
Serial.println("[STATUS] " + name + " has BOARDED the bus.");
sendTelegramAlert(name + " has boarded the school bus. (UID: " + uid + ")");
studentStatus[uid] = true;
} else {
Serial.println("[STATUS] " + name + " has DEPARTED from the bus.");
sendTelegramAlert(name + " has departed from the school bus. (UID: " + uid + ")");
studentStatus[uid] = false;
}
rfid.PICC_HaltA();
rfid.PCD_StopCrypto1();
delay(3000);
}