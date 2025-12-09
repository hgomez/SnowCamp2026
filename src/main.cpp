#include <Arduino.h>
#include <bluefruit.h>

// Diffusion d'un sujet, pendant 10s
#define TEMPS_DIFFUSION_SUJET   10

// Diffusion d'un sujet mode rapide, pendant 3s
#define TEMPS_DIFFUSION_SUJET_FASTMODE 3

typedef volatile uint32_t REG32;
#define pREG32 (REG32 *)
#define MAC_ADDRESS_LOW   (*(pREG32 (0x100000a4)))

boolean advertising_stopped = false;

//
// Le format de nom de diffusion
// les 3 premiers caractères sont M&G, l'entête
// Les 4 suivants les 4 suivants, les 32 bits bas de l'adresse mac de la carte 
// Le suivant, + (si on aime), - (si on n'aime pas)
// Les 6 suivants, le sujet, Java/K8S/Node/Intel....
//
// Exemples
// 01234567890123
// M&G1234+Java 
// M&G1234-Node 
// M&G1234+6502 
// M&G1234-Z80 

// On définit une zone de 15 caractères 
// 14 caractères, limite de diffusion de nom et un caractère de fin de ligne, 0x00
char ble_name[15];

char *sujets[] = { "+JAVA ", "+6502" , "-Node ", "-Z80", "-K8S" };

unsigned int index_sujets = 0;

char * construit_advising(char * lesujet) {
  sprintf(&ble_name[0],"M&G%04lX%.7s", (MAC_ADDRESS_LOW) & 0xFFFF, lesujet);
  return &ble_name[0];
}

char * sujet_suivant() {

  Serial.printf("sujet #%d\n", index_sujets);

  if (index_sujets >= (sizeof(sujets) / sizeof(sujets[0]))) 
    index_sujets = 0;

  // On est dans les sujets aimés ?
  if (index_sujets < (sizeof(sujets) / sizeof(sujets[0])))
    return (sujets[index_sujets++]);

  // Impossible d'être là 
  return ((char *) "");
}


/**
 * Callback invoked when advertising is stopped by timeout
 */
void adv_stop_callback(void)
{
  if (advertising_stopped == false) {
    Serial.printf("Fin de diffusion du sujet, prochain sujet #%d\n", index_sujets);
    advertising_stopped = true;
    Bluefruit.Advertising.stop();
  }
}

void diffusion_bluetooth(void)
{   
  // Advertising packet
  Bluefruit.Advertising.clearData();
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.setStopCallback(adv_stop_callback);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);                             // in units of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(TEMPS_DIFFUSION_SUJET_FASTMODE);   // number of seconds in fast mode
  Bluefruit.Advertising.start(TEMPS_DIFFUSION_SUJET);                     // Stop advertising entirely after TEMPS_DIFFUSION_SUJET seconds 
  advertising_stopped = false;
}


void setup() {
  // initialize digital pin D3-D8 and the built-in LED as an output.
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D8,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); 

  // Init serial communication for debugging
  Serial.begin(115200);
  long start = millis();
  while ( !Serial && (millis() - start < 2000)) delay(10); 

  // start bluetooth
  Bluefruit.begin();
  Bluefruit.setTxPower(4);

  char * sujet_a_diffuser = sujet_suivant();
  Serial.printf("Diffusion du premier sujet %s\n", sujet_a_diffuser);

  // Set name
  Bluefruit.setName(construit_advising(sujet_a_diffuser));
  diffusion_bluetooth();
}


void loop() {
  char * sujet_a_diffuser;
  static int cnt = 0;

  digitalWrite(D3,LOW);
  digitalWrite(D4,LOW);
  digitalWrite(D5,LOW);
  digitalWrite(D6,LOW);
  digitalWrite(D7,LOW);
  digitalWrite(D8,LOW);

  switch(cnt) {
    case 0 : digitalWrite(D3,HIGH);break;
    case 1 : digitalWrite(D4,HIGH);break;
    case 2 : digitalWrite(D5,HIGH);break;
    case 3 : digitalWrite(D6,HIGH);break;
    case 4 : digitalWrite(D7,HIGH);break;
    case 5 : digitalWrite(D8,HIGH);break;
    default: break;
  }

  digitalWrite(LED_BUILTIN,(cnt&1)?HIGH:LOW);
  cnt = ( cnt + 1 ) % 6;
  delay(500);
  Serial.printf("cnt %d\n", cnt);

  if (advertising_stopped == true) {
    advertising_stopped = false;
    // On diffuse le nom suivant
    sujet_a_diffuser = sujet_suivant();
    Bluefruit.setName(construit_advising(sujet_a_diffuser));
    Serial.printf("Diffusion du sujet %s\n", sujet_a_diffuser);
    diffusion_bluetooth();
  }

}
