#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL4ZsH5cRwJ"
#define BLYNK_TEMPLATE_NAME "wifi arduino"
#define BLYNK_AUTH_TOKEN "Or6iAfiawf4vcQb9ofY8-nucnmCMkeg3"

//bibliotek för att wifi ska fungera
#include <SPI.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>

// Ditt WiFi inlogg
// Set password to "" for open networks.
char ssid[] = "NTIG Guest";
char pass[] = "TeknikPassion";
//char ssid[] = "TN-ET4783";
//char pass[] = "duroirHygUt6";

int spd;  // Variabel för hastighet
float turn;  // En skalär som används för svängning
int A_speed;  // Hastighet för motor A
int B_speed;  // Hastighet för motor B

void setup() {
  pinMode(7, INPUT); //on-off pin

  Serial.begin(9600);

  //Setup Channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin
  pinMode(9, OUTPUT); //Initiates Brake Channel A pin

  //Setup Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel A pin
  pinMode(8, OUTPUT);  //Initiates Brake Channel A pin

  //speed-pins
  pinMode(3, OUTPUT);
  pinMode(10, OUTPUT);

  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);  //(om vi byter till 10)

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

}

void loop() {

  drive();  // Kör huvudfunktionslogiken

  Blynk.run();  // Hantera Blynk-uppkopplingen
}

/* 
 * BLYNK_WRITE(V1) kallas när Slider Widget i Blynk-appen uppdateras.
 * Den tar emot ett värde från virtuell pin V1 och uppdaterar hastigheten (spd)
 * baserat på det värdet, vilket sedan används för att styra motorerna.
 */
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // tilldelar det inkommande värdet från pin V1 till en variabel

  if (pinValue == 0) {
    spd = 0;
  } else if (pinValue > 0) {
    spd = map(pinValue, 1, 100, 70, 255); // Mappar värdet för att passa motorhastigheten
  } else {
    spd = map(pinValue, -1, -100, -70, -255); // Mappar värdet för att passa motorhastigheten vid backning
  }
}

/* 
 * BLYNK_WRITE(V2) kallas när ett värde skickas från Blynk-appen till virtuell pin V2.
 * Den tar emot ett flyttal (float) från pin V2 och tilldelar det till variabeln 'turn',
 * vilket representerar svängvinkeln.
 */
BLYNK_WRITE(V2)
{
  turn = param.asFloat(); // tilldelar det inkommande värdet från pin V2 till en variabel
  Serial.println(turn);
}

/*
 * Styr en motor framåt.
 * Parametrar:
 *  - velocity: en integer mellan 0 och 255 som styr hastigheten på motorn.
 *  - motor: en sträng ("A" eller "B") som identifierar vilken motor som ska styras.
 */
void forward(int velocity, String motor) {
  if (motor == "A") {
    //Motor A framåt
    digitalWrite(12, HIGH);
    digitalWrite(9, LOW);
    analogWrite(3, velocity); // Ställer in hastigheten för motor A
  } 
  else if (motor == "B") {
    //Motor B framåt
    digitalWrite(13, HIGH);
    digitalWrite(8, LOW);
    analogWrite(10, velocity); // Ställer in hastigheten för motor B
  }
}

/*
 * Styr en motor bakåt.
 * Liknande 'forward', men styr motorerna i motsatt riktning för backning.
 */
void backward(int velocity, String motor) {
  if (motor == "A") {
    //Motor A bakåt
    digitalWrite(12, LOW);
    digitalWrite(9, LOW);
    analogWrite(3, velocity); // Ställer in hastigheten för motor A för backning
  } 
  else if (motor == "B") {
    //Motor B bakåt
    digitalWrite(13, LOW);
    digitalWrite(8, LOW);
    analogWrite(10, velocity); // Ställer in hastigheten för motor B för backning
  }
}

/*
 * Drive-funktionen innehåller huvudlogiken för att styra motorerna.
 * Den använder globala variabler för att bestämma motorhastigheter baserat på
 * 'spd'-värdet och 'turn'-värdet. Den läser även av pin 7 (on/off-knapp)
 * för att bestämma om motorerna ska köras.
 */
void drive() {
  if (turn > 0) { // Svängningsgrad från slidern i appen
    A_speed = spd - (turn * spd); // Skalar ner hastighet A med hjälp av turn-värdet
    B_speed = spd;
  } 
  else if (turn < 0) { // Annars Skalar ner hastighet B med hjälp av turn-värdet
    A_speed = spd;
    B_speed = spd + (turn * spd);
  }
  
  if (digitalRead(7)) {
    if (spd > 0) {
      forward(A_speed, "A"); // Kör motor A framåt med den beräknade hastigheten
      forward(B_speed, "B"); // Kör motor B framåt med den beräknade hastigheten
    } 
    else {
      backward(-A_speed, "A"); // Kör motor A bakåt med den beräknade hastigheten
      backward(-B_speed, "B"); // Kör motor B bakåt med den beräknade hastigheten
    }
  }
  else {
    forward(0, "A"); // Stanna motor A
    forward(0, "B"); // Stanna motor B
  }
}
