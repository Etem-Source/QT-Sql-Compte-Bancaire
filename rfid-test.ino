#include <SPI.h>
#include <MFRC522.h>

// Définir les broches du module RC522
#define SS_PIN 12  // Pin SDA (Slave Select) est D12
#define RST_PIN 9  // Pin RESET, garde cette broche par défaut à D9

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Créer un objet MFRC522 avec la configuration des broches

void setup() {
  Serial.begin(9600);  // Initialiser le moniteur série
  SPI.begin();         // Initialiser la communication SPI
  
  // Le module RC522 utilise par défaut SCK sur D13, MOSI sur D11, et MISO sur D10
  // On n'a donc pas besoin de spécifier ces broches dans le code

  mfrc522.PCD_Init();  // Initialiser le module RC522
  
  Serial.println("Approchez votre carte RFID...");
}

void loop() {
  // Vérifier si une carte est présente
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Vérifier si la carte peut être lue
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("UID de la carte : ");
      
      // Afficher l'UID sous forme hexadécimale
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        Serial.print(" ");
      }
      
      Serial.println();  // Nouvelle ligne
      mfrc522.PICC_HaltA();  // Arrêter la lecture de la carte
      mfrc522.PCD_StopCrypto1();  // Arrêter la communication
    }
  }
}
