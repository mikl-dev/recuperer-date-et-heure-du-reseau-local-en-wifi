/***********************************************************************************
   Horloge connectée à base d'ESP8266 ou d'ESP32
   Affichage de l'heure et de la date dans le moniteur série.
   L'heure est obtenue grâce à un serveur NTP
   
   https://electroniqueamateur.blogspot.com/2018/10/horloge-wi-fi-esp8266.html
   
*************************************************************************************/

#if defined ARDUINO_ARCH_ESP8266  // s'il s'agit d'un ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32  // s'il s'agit d'un ESP32
#include "WiFi.h"
#endif

#include <time.h>

const char* ssid = "Bbox-E9ED9E75-pro-2.4G";
const char* password = "Vivimimi123456789";

const int decalage = 2;  // la valeur dépend de votre fuseau horaire. Essayez 2 pour la France. 
const int delaiDemande = 5 * 60; // nombre de secondes entre deux demandes consécutives au serveur NTP

unsigned long derniereDemande = millis(); // moment de la plus récente demande au serveur NTP
unsigned long derniereMaJ = millis(); // moment de la plus récente mise à jour de l'affichage de l'heure
time_t maintenant;
struct tm * timeinfo;


void afficheHeureDate() {

  timeinfo = localtime(&maintenant);

  Serial.print("Heure:   ");
  if ((timeinfo->tm_hour ) < 10) {
    Serial.print("0");
  }
  Serial.print(timeinfo->tm_hour );  // heure entre 0 et 23
  Serial.print(":");

  if (timeinfo->tm_min < 10) {
    Serial.print("0");
  }
  Serial.print(timeinfo->tm_min);   // timeinfo->tm_min: minutes (0 - 59)
  Serial.print(":");

  if (timeinfo->tm_sec < 10) {
    Serial.print("0");
  }
  Serial.print(timeinfo->tm_sec);   // timeinfo->tm_sec: secondes (0 - 60)

  Serial.print("        Date:    ");

  if (timeinfo->tm_mday < 10) {
    Serial.print("0");
  }

  Serial.print(timeinfo->tm_mday);  // timeinfo->tm_mday: jour du mois (1 - 31)
  Serial.print("-");

  if ((timeinfo->tm_mon + 1) < 10) {
    Serial.print("0");
  }

  Serial.print(timeinfo->tm_mon + 1);    // timeinfo->tm_mon: mois (0 - 11, 0 correspond à janvier)
  Serial.print("-");
  Serial.println(timeinfo->tm_year + 1900);  // timeinfo->tm_year: tm_year nombre d'années écoulées depuis 1900

}

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);

  Serial.println();

  WiFi.begin(ssid, password);
  Serial.print("Connexion au reseau WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println();

  
  configTime(decalage * 3600, 0, "fr.pool.ntp.org");  //ca.pool.ntp.org serveurs canadiens
       // en Europe, essayez europe.pool.ntp.org ou fr.pool.ntp.org
  
  Serial.print("Attente date et heure");

  while (time(nullptr) <= 100000) {
    Serial.print(".");
    delay(1000);
  }

  time(&maintenant);

  Serial.println("");
}



void loop() {

  // est-ce le moment de demander l'heure NTP?
  if ((millis() - derniereDemande) >=  delaiDemande * 1000 ) {
    time(&maintenant);
    derniereDemande = millis();

    Serial.println("Interrogation du serveur NTP");
  }

  // est-ce que millis() a débordé?
  if (millis() < derniereDemande ) {
    time(&maintenant);
    derniereDemande = millis();
  }

  // est-ce le moment de raffraichir la date indiquée?
  if ((millis() - derniereMaJ) >=   1000 ) {
    maintenant = maintenant + 1;
    afficheHeureDate();
    derniereMaJ = millis();
  }
}