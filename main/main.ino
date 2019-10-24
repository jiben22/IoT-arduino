#include <SoftwareSerial.h>

#define RxD 10 //Pin 10 pour arduino RX --> vers TxD de la carte Bluetooth
#define TxD 11 //Pin 11 pour arduino TX --> vers RxD de la carte Bluetooth

#define MOTOR_UP 2 //Pin 2 pour le moteur (fermer)
#define MOTOR_DOWN 7 //Pin 7 pour le moteur (ouvrir)
#define LED 4 //Pin 4 pour la led (qui indique que le moteur est allumé)

const int loop_delay = 100; // delai enter deux lancement de la fonction loop()
const int motor_tps_on = 5000; // temps d'allumage du moteur (5 sec)
int cpt_motor = 0; // compteur pour le moteur

int motor_up = 0; // 0=False, 1=True
int motor_down = 0; // 0=False, 1=True


// Pour passer en mode commande, mettre la broche EN sur le pin 3.3V

SoftwareSerial BTSerie(RxD, TxD); // RxD | TxD

void setup() {
  InitCommunicationSerie();
  Serial.println("Initialisation carte : Ok");

  // Config moteur
  pinMode(MOTOR_UP, OUTPUT);
  pinMode(MOTOR_DOWN, OUTPUT);
  pinMode(LED, OUTPUT);

  // Configuration du bluetooth
  /*pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);

  InitCommunicationBluetoothSerie();*/
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
  // Bluetooth
  /*char c;
  
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
  }*/

  // Capteur de temperature
  /*int valeurBrute = analogRead(A0);

  float tempCelcius = valeurBrute * (5.0 / 1023.0 * 100.0);

  Serial.print(tempCelcius, 2);
  Serial.println("°C");*/

  // Detection de commandes
  if(Serial.available()) {
    char cmd[] = "";
    int count = 0;

    while(Serial.available()) {
      char c = Serial.read();
      cmd[count++] = c;
    }

    Serial.print(cmd);
    String a = String(cmd); // FIXME !! la comparaison de str ne marche pas ..? (strcmp, String.equals(), ...)
    if (a.equals(String("close"))) {
      motor_up = 1;
    } else if (a.equals(String("open"))) {
      motor_down = 1;
    }
  }

  // Moteur
  if (( motor_up or motor_down) and cpt_motor*loop_delay <= motor_tps_on) {
    cpt_motor++;

    // turn on or off the led
    if (cpt_motor%2 == 0) {
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
    }

    // set motor to on
    if (motor_up) {
      digitalWrite(MOTOR_UP, HIGH);
    } else {
      digitalWrite(MOTOR_DOWN, HIGH);
    }
  } else {
    // turn off motor and led
    digitalWrite(MOTOR_UP, LOW);
    digitalWrite(MOTOR_DOWN, LOW);
    digitalWrite(LED, HIGH);

    // reset var
    motor_up = 0;
    motor_down = 0;
    cpt_motor = 0;
  }
  
  
  delay(loop_delay);
}
