#include <SoftwareSerial.h>

#define RxD 10 //Pin 10 pour arduino RX --> vers TxD de la carte Bluetooth
#define TxD 11 //Pin 11 pour arduino TX --> vers RxD de la carte Bluetooth 

SoftwareSerial BTSerie(RxD, TxD); // RxD | TxD

void setup() {
  InitCommunicationSerie();
  Serial.println("Initialisation carte : Ok");

  // Configuration du bluetooth
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);

  InitCommunicationBluetoothSerie();
}

// Initialiastion de la communication serie avec l'ordinateur

void InitCommunicationSerie() {
  Serial.begin(9600);
  while(!Serial) {
    
  }
  Serial.println("Demarrage connexion serie : Ok");
}

void InitCommunicationBluetoothSerie() {
  BTSerie.begin(9600); //38400 / 57600 / 38400
  while(!BTSerie) {
    Serial.println("Attente reponse Bluetooth");
  }
  Serial.println("Demarrage connexion Bluetooth serie : Ok");
}

void loop() {
  char c;

  if(BTSerie.available()) {
    Serial.println("Reception de : ");
    while(BTSerie.available()) {
      c = BTSerie.read();
      Serial.print(c);
    }
  }

  if(Serial.available()) {
    Serial.print("Envoie vers BLuetooth : ");
    while(Serial.available()) {
      c = Serial.read();
      Serial.print(c);
      BTSerie.print(c);
    }
  }

  delay(1000);

}
