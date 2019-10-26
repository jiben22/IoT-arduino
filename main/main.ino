#include <SoftwareSerial.h>

// Pour passer en mode commande sur le module bluetooth
// mettre la broche EN sur le pin 3.3V

#define loop_delay 100 // delai enter deux lancement de la fonction loop()

// Bluetooth
#define RxD 10 //Pin 10 pour arduino RX --> vers TxD de la carte Bluetooth
#define TxD 11 //Pin 11 pour arduino TX --> vers RxD de la carte Bluetooth

SoftwareSerial BTSerie(RxD, TxD); // RxD | TxD


// Motor
#define MOTOR_UP 2        //Pin 2 pour le moteur (fermer)
#define MOTOR_DOWN 4      //Pin 4 pour le moteur (ouvrir)
#define LED 7             //Pin 7 pour la led (qui indique que le moteur est allumé)
#define motor_tps_on 5000 // temps d'allumage du moteur (5 sec)

int cpt_motor = 0;       // compteur pour le moteur
int motor_up = 0;        // 0=False, 1=True
int motor_down = 0;      // 0=False, 1=True
int shutter_is_open = 1; // par default c'est ouvert


// Capteur de luminosité
int photocellPin = 0; // the cell and 10K pulldown are connected to a0
int photocellReading; // the analog reading from the analog resistor divider


void setup() {
  InitCommunicationSerie();
  Serial.println("Initialisation carte : Ok");

  // Configuration du moteur
  pinMode(MOTOR_UP, OUTPUT);
  pinMode(MOTOR_DOWN, OUTPUT);
  pinMode(LED, OUTPUT);

  // Configuration du bluetooth
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  InitCommunicationBluetoothSerie();
}

// Initialiastion de la communication serie avec l'ordinateur
void InitCommunicationSerie() {
  Serial.begin(9600);
  while(!Serial) {}
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
  bluetooth_cmd_routine();
  //temp_routine();
  motor_routine();
  lum_routine(); // FIXME, lum not work ?

  delay(loop_delay);
}

void bluetooth_cmd_routine() {
  // Bluetooth
  String received;
  String sended;

  if(BTSerie.available()) {
    received = BTSerie.readString();
    Serial.print("Reception de : ");
    Serial.print(received);
  }

  if(Serial.available()) {
    sended = Serial.readString();
    Serial.print("Envoie vers Bluetooth : ");
    Serial.print(sended);
    BTSerie.print(sended);
  }

  // Commandes
  if (received == "open\r\n" or sended == "open\n") {
    open_shutter();
  } else if (received == "close\r\n" or sended == "close\n") {
    close_shutter();
  }
}

void temp_routine(){
  // Capteur de temperature
  int valeurBrute = analogRead(A0);

  float tempCelcius = valeurBrute * (5.0 / 1023.0 * 100.0);

  Serial.print(tempCelcius, 2);
  Serial.println("°C");
  
}

void motor_routine() {
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

    if (motor_up) shutter_is_open = 1;
    else if (motor_down) shutter_is_open = 0;

    // reset var
    motor_up = 0;
    motor_down = 0;
    cpt_motor = 0;
  }
}

void lum_routine() {
  // Capteur de luminosité
  int lum = analogRead(A0);

  if (lum < 200) {
    if (shutter_is_open and !motor_down) close_shutter();
  } else {
    if (!shutter_is_open and !motor_up) open_shutter();
  }

  /*Serial.print(lum);

  if (lum < 10) {
    Serial.println(" - Noir");
  } else if (lum < 200) {
    Serial.println(" - Sombre");
  } else if (lum < 500) {
    Serial.println(" - Lumiere");
  } else if (lum < 800) {
    Serial.println(" - Lumineux");
  } else {
    Serial.println(" - Tres lumineux");
  }*/
}


void open_shutter() {
  digitalWrite(MOTOR_DOWN, LOW);
  motor_down = 0;
  motor_up = 1;
  cpt_motor = 0;
  
  Serial.println("Ouverture des volets !");
  BTSerie.println("Ouverture des volets !");
}

void close_shutter() {
  digitalWrite(MOTOR_UP, LOW);
  motor_up = 0;
  motor_down = 1;
  cpt_motor = 0;
  
  Serial.println("Fermeture des volets !");
  BTSerie.println("Fermeture des volets !");
}
