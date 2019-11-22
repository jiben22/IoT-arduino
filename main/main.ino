#include <SoftwareSerial.h>

// Pour passer en mode commande sur le module bluetooth
// mettre la broche EN sur le pin 3.3V

#define loop_delay 100 // delai enter deux lancement de la fonction loop()

// Bluetooth
#define RxD01 10 //Pin 10 pour arduino RX --> vers TxD de la carte Bluetooth
#define TxD01 11 //Pin 11 pour arduino TX --> vers RxD de la carte Bluetooth
#define RxD02 12 //Pin 12 pour arduino RX --> vers TxD de la carte Bluetooth
#define TxD02 13 //Pin 13 pour arduino TX --> vers RxD de la carte Bluetooth

SoftwareSerial BTSerieSmartphone(RxD01, TxD01); // For smartphone
SoftwareSerial BTSerieHC05(RxD02, TxD02); // For another HC-05 module


// Motor
#define MOTOR_UP 2        //Pin 2 pour le moteur (fermer)
#define MOTOR_DOWN 4      //Pin 4 pour le moteur (ouvrir)
#define LED 7             //Pin 7 pour la led (qui indique que le moteur est allumé)
#define motor_tps_on 5000 // temps d'allumage du moteur (5 sec)
#define timeout_other_card 2000 // timeout avant de quitter (2sec)
#define temp_asked_delay 30000 // on demande la temperature toutes les 30 sec (temps relatif)

int cpt_temp = 0;        // compteur pour demande temperature
int cpt_motor = 0;       // compteur pour le moteur
int motor_up = 0;        // 0=False, 1=True
int motor_down = 0;      // 0=False, 1=True
int shutter_is_open = 1; // par default c'est ouvert
int mode_auto = 1;       // mode automatique, ou manuel (pour l'ouverture/fermeture des volets)

int timeout_card_cpt = 0;

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
  digitalWrite(LED, HIGH); // turn off

  // Bluetooth Configuration for smartphone
  pinMode(RxD01, INPUT);
  pinMode(TxD01, OUTPUT);
  BTSerieSmartphone.begin(38400); //38400 / 57600 / 9600
  while(!BTSerieSmartphone) Serial.println("Attente reponse Bluetooth");
  Serial.println("Demarrage connexion Bluetooth smartphone : Ok");

  // Bluetooth Configuration for another HC-05 module
  pinMode(RxD02, INPUT);
  pinMode(TxD02, OUTPUT);
  BTSerieHC05.begin(38400); //38400 / 57600 / 9600
  while(!BTSerieHC05) Serial.println("Attente reponse Bluetooth");
  Serial.println("Demarrage connexion Bluetooth serie : Ok");
  
  // default, listen Smartphone
  BTSerieSmartphone.listen();
}

// Init communication with the computer
void InitCommunicationSerie() {
  Serial.begin(9600);
  while(!Serial) {}
  Serial.println("Demarrage connexion serie : Ok");
}

void loop() {
  bluetooth_routine();
  temp_routine();
  motor_routine();
  if (mode_auto) lum_routine();

  delay(loop_delay);
}

void bluetooth_routine() {
  // Bluetooth Smartphone
  String received_s = "";
  String received_o = "";
  String sended = "";

  // when we wait some answer from other arduino card
  if(BTSerieHC05.isListening()) {
    if (BTSerieHC05.available()) {
      received_o = BTSerieHC05.readString();
  
      // reconnect to  smartphone
      timeout_card_cpt=0;
      BTSerieSmartphone.listen();
      
      Serial.print("[DEBUG] Reception par HC05 de : ");
      Serial.print(received_o);
      BTSerieSmartphone.print(received_o); // envoie de la reponse
    }
    
    timeout_card_cpt++;
    if (timeout_card_cpt * loop_delay >= timeout_other_card) {
      // reconnect to  smartphone
      timeout_card_cpt=0;
      BTSerieSmartphone.listen();
      Serial.println("[WARNING] Module 2 inaccessible !");
    }
  }

  if(BTSerieSmartphone.isListening() && BTSerieSmartphone.available()) {
    received_s = BTSerieSmartphone.readString();
    Serial.print("[DEBUG] Reception de Smartphone de : ");
    Serial.print(received_s);
  }

  // DEBUG on computer
  if(Serial.available()) {
    sended = Serial.readString();
    Serial.print("[DEBUG] Envoie vers Bluetooth : ");
    Serial.print(sended);
    BTSerieSmartphone.print(sended);
  }

  // Commandes
  if (received_s == "open\r\n" or sended == "open\n") {
    mode_auto=0;        // passage auto en mode manuel
    open_shutter();     // ouverture des volets
  } else if (received_s == "close\r\n" or sended == "close\n") {
    mode_auto=0;        // passage auto en mode manuel
    close_shutter();    // fermeture des volets
  } else if (received_s == "auto\r\n" or sended == "auto\n") {
    if (!mode_auto) {
      mode_auto=1;
      Serial.println("[DEBUG] Passage en mode automatique du volet !");
      BTSerieSmartphone.println("Passage en mode automatique du volet !");
    } else {
      Serial.println("[DEBUG] Mode automatique deja active !");
      BTSerieSmartphone.println("Mode automatique deja active !");
    }
  } else if (received_s != "" or sended != "") {
    // send to other arduino card
    BTSerieHC05.listen();
    if (received_s != "") BTSerieHC05.print(received_s);
    else BTSerieHC05.print(sended);
  }
}

void temp_routine(){
  // On demande la température de temps en temps via l'autre module
  cpt_temp++;
  if (cpt_temp * loop_delay >= temp_asked_delay) {
    cpt_temp=0;
    Serial.println("[DEBUG] Demande de la temperature ambiante ! (autre carte)");
    BTSerieHC05.listen();
    BTSerieHC05.print("temperature");
  }
  // Capteur de temperature
  /*int valeurBrute = analogRead(A1);

  float tempCelcius = valeurBrute * (5.0 / 1023.0 * 100.0);

  Serial.print(tempCelcius, 2);
  Serial.println("°C");*/
  
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

  if (lum < 300) {
    if (shutter_is_open and !motor_down) {
      Serial.println("[DEBUG] Il commence a faire sombre !");
      BTSerieSmartphone.println("Il commence a faire sombre !");
      close_shutter();
    }
  } else {
    if (!shutter_is_open and !motor_up) {
      Serial.println("[DEBUG] Le temps s'eclaircit !");
      BTSerieSmartphone.println("Le temps s'eclaircit !");
      open_shutter();
    }
  }

  // DEBUG
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
  if (!shutter_is_open) {
    digitalWrite(MOTOR_DOWN, LOW);
    motor_down = 0;
    motor_up = 1;
    cpt_motor = 0;
    
    Serial.println("[DEBUG] Ouverture des volets !");
    BTSerieSmartphone.println("Ouverture des volets !");

    // envoi vers l'autre carte
    BTSerieHC05.listen();
    BTSerieHC05.print("is_opening_shutter");
    BTSerieSmartphone.listen();
  }
}

void close_shutter() {
  if (shutter_is_open) {
    digitalWrite(MOTOR_UP, LOW);
    motor_up = 0;
    motor_down = 1;
    cpt_motor = 0;
    
    Serial.println("[DEBUG] Fermeture des volets !");
    BTSerieSmartphone.println("Fermeture des volets !");
    
    // envoi vers l'autre carte
    BTSerieHC05.listen();
    BTSerieHC05.print("is_closing_shutter");
    BTSerieSmartphone.listen();
  }
}
