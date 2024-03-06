#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN   27  // Pino de reset
#define SDA_PIN   5   // Pino SS

MFRC522 mfrc522(SDA_PIN, RST_PIN);  // Crie o objeto MFRC522

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Aguardando cartão RFID...");
}

void loop() {
  // Procure por cartões RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Cartão detectado!");

    // Obtenha o UID lido
    String uidString = "";
    for (byte i = 0; i < mfrc522.uid.size; ++i) {
      uidString += String(mfrc522.uid.uidByte[i], HEX);
      uidString.toUpperCase();
      uidString += " ";
    }

    // Compare o UID lido com o UID associado a "Marcus"
    if (verificarUID(uidString)) {
      Serial.println("Cartão do Marcus detectado!");
    } else {
      Serial.println("Cartão não associado a uma pessoa conhecida.");
    }

    mfrc522.PICC_HaltA();  // Pare a leitura do cartão
    delay(5000);  // Aguarda 5 segundos antes de procurar o próximo cartão
    Serial.println("Aguardando cartão RFID...");
  }
}

bool verificarUID(String uid) {
  // Compare o UID lido com o UID associado a "Marcus"
  // Neste exemplo, apenas um UID específico está associado a "Marcus"
  return uid == "43 71 3E 25 ";
}
