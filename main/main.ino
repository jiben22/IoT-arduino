#include <SoftwareSerial.h>

#define RxD 10 //Pin 10 pour arduino RX --> vers TxD de la carte Bluetooth
#define TxD 11 //Pin 11 pour arduino TX --> vers RxD de la carte Bluetooth

// Pour passer en mode commande, mettre la broche EN sur le pin 3.3V

SoftwareSerial BTSerie(RxD, TxD); // RxD | TxD

// Capteur de luminosité
int photocellPin = 0; // the cell and 10K pulldown are connected to a0
int photocellReading; // the analog reading from the analog resistor divider

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
  BTSerie.begin(38400); //38400 / 57600 / 9600
  while(!BTSerie) {
    Serial.println("Attente reponse Bluetooth");
  }
  Serial.println("Demarrage connexion Bluetooth serie : Ok");
}

void loop() {
  char c;

  // Bluetooth
  if(BTSerie.available()) {
    Serial.println("Reception de : ");
    while(BTSerie.available()) {
      c = BTSerie.read();
      Serial.print(c);
    }
    Serial.println(); // retour a la ligne
  }

  if(Serial.available()) {
    Serial.print("Envoie vers Bluetooth : ");
    while(Serial.available()) {
      c = Serial.read();
      Serial.print(c);
      BTSerie.print(c);
    }
    Serial.println(); // retour a la ligne
    BTSerie.println();
  }

  // Capteur de temperature
  /*int valeurBrute = analogRead(A0);

  float tempCelcius = valeurBrute * (5.0 / 1023.0 * 100.0);

   Serial.print(tempCelcius, 2);
   Serial.println("°C");
  
  delay(100);*/

  // Capteur de luminosité

  int valeurLuminosite = analogRead(A0);

  Serial.print(valeurLuminosite);

  if (valeurLuminosite < 10) {
    Serial.println(" - Noir");
  } else if (valeurLuminosite < 200) {
    Serial.println(" - Sombre");
  } else if (valeurLuminosite < 500) {
    Serial.println(" - Lumiere");
  } else if (valeurLuminosite < 800) {
    Serial.println(" - Lumineux");
  } else {
    Serial.println(" - Tres lumineux");
  }
  delay(100);
}
