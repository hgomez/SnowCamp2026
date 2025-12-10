#include <Arduino.h>
#include <bluefruit.h>

typedef volatile uint32_t REG32;

#define pREG32 (REG32 *)
#define MAC_ADDRESS_LOW   (*(pREG32 (0x100000a4)))

// Advertising topic duration in milliseconds
// Receivers have cache so emit long enough, the Cache GATT (Generic Attribute Profile Cache)
// 15s on iOS 15s, 30s on Android/Windows/Linux
//
const unsigned long ADVERTISING_TOPIC_DURATION_MS = 35000L;

// Emit mode in fast mode
const uint16_t ADVERTISING_DURATION_FASTMODE = 3;

// Restart Scanner regularly
const unsigned long SCAN_RESTART_INTERVAL_MS = 5000L; // 5 secondes

// Last Scan Restart time
unsigned long last_scan_restart_time = 0;

//
// In BLE, only 30 bytes are allowed in headers and BlueFruit allow only 14 characters in name
// 
// We define this format on name so :
// First 3 chars are M&G, our device header
// Next 4 chars and lower 16 bits of card mac address
// Next char, is + (like) or - (don't like)
// Remaining 6 chars are topic name, say Java/K8S/Node/Intel....
//

// 01234567890123
//
// M&G1234+Java 
// M&G1234-Node 
// M&G1234+6502 
// M&G1234-Z80 
// M&G1234-K8S 
// M&G1234+DEVOPS 

// ble_name is a char array to hold Bluetooth name 
// 14 chars and a trailing 0x00
char ble_name[15];

// My very own topics
const char *topics[] = { "+JAVA ", "+6502" , "-Node ", "-Z80", "-K8S", "+DEVOPS" };

// Our topic index
unsigned int topics_index = 0;

// Last time advising started
unsigned long last_update_time = 0;

// Bluetooth address
ble_gap_addr_t blue_addr;

// Bluetooth address as string
char blue_addr_str[18]; 


char * build_advising_name(const char * lesujet) {
  sprintf(&ble_name[0],"M&G%04lX%-7s", (MAC_ADDRESS_LOW) & 0xFFFF, lesujet);
  return &ble_name[0];
}

/*
*/
void addr_to_str(const ble_gap_addr_t& addr, char* str) {
  sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
          addr.addr[5], addr.addr[4], addr.addr[3], 
          addr.addr[2], addr.addr[1], addr.addr[0]);
}


void print_adv_data(const uint8_t* data, uint8_t len) {
    Serial.print("Data: ");
    for (int i = 0; i < len; i++) {
        // Afficher chaque octet en hexadécimal (sur deux chiffres)
        Serial.printf("%02X ", data[i]);
    }
    Serial.println();
}


/***
 * Move to next topic, increment topics_index variables and wrap around
 */
void next_topic() {
  
  // Compute array size
  const unsigned int NB_TOPICS = sizeof(topics) / sizeof(topics[0]);
  
  Serial.printf("current topic #%d\n", topics_index);

  // Increment topic index
  topics_index++; 
  
  // If topic index is too large, back to 0
  if (topics_index >= NB_TOPICS) { 
    topics_index = 0;
  }

  Serial.printf("next topic #%d\n", topics_index);
}


/**
 * Callback invoked when advertising is stopped by timeout
 */
void adv_stop_callback(void)
{
  Serial.printf("Unexpected end of advertising. next topic #%d\n", topics_index);
}

void start_advertising(void)
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
  // Bluefruit.Advertising.setInterval(32, 244);                        // in units of 0.625 ms
  Bluefruit.Advertising.setInterval(32, 32);                            // in units of 0.625 ms => 20ms fast mode
  Bluefruit.Advertising.setFastTimeout(ADVERTISING_DURATION_FASTMODE);   // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                                       // Advertise forever 
}

/*
 Scan Callback
*/
void scan_callback(ble_gap_evt_adv_report_t* adv_report)
{

  Serial.println("Scan Callback");

  char peer_addr_str[18] = { 0 };
  addr_to_str(adv_report->peer_addr, peer_addr_str);

  char direct_addr_str[18] = { 0 };
  addr_to_str(adv_report->direct_addr, direct_addr_str);
  
  Serial.printf("\n--- Report Received ---\n");
  Serial.printf("Peer Addr: %s | Direct Addr: %s | RSSI: %d dBm | Type: 0x%02X\n", 
                  peer_addr_str, 
                  direct_addr_str, 
                  adv_report->rssi, 
                  adv_report->type);

  // In peu de debug
  print_adv_data(adv_report->data.p_data, adv_report->data.len);

  char name_buffer[32] = { 0 };
  
  // Tenter de récupérer le nom complet
  // Note: Cast explicite de char* vers uint8_t*
  if (Bluefruit.Scanner.parseReportByType(
        adv_report, 
        BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, 
        (uint8_t*)name_buffer, // Correction de type ici
        sizeof(name_buffer))
     )
  {
    // Filtrer uniquement les périphériques M&G (votre protocole)
    if (strncmp(name_buffer, "M&G", 3) == 0)
    {
      char addr_str[18] = { 0 };
      // Adress to String
      addr_to_str(adv_report->peer_addr, addr_str);      
      Serial.printf(">> NRF Detected! Addr: %s, Topic: %s\n", addr_str, name_buffer);
    }
  }
}

/*
 * Scan Part
 * 
 */
void start_scanner() {

  uint16_t scan_interval = 160; // 160 * 0.625 ms = 100 ms
  uint16_t scan_window   = 80;  // 80 * 0.625 ms = 50 ms

  Bluefruit.Scanner.setRxCallback(scan_callback); // Lier la fonction de gestion des résultats
  Bluefruit.Scanner.setInterval(scan_interval, scan_window);
  Bluefruit.Scanner.useActiveScan(true); // Demander des SCAN_RSP (non requis pour votre cas, mais bonne pratique)
  Bluefruit.Scanner.restartOnDisconnect(true);

  // reportEvents is not available in 0.10.2
  // Bluefruit.Scanner.reportEvents(true);

  // Démarrer le scan une fois pour qu'il s'exécute en continu
  Bluefruit.Scanner.start(0); // 0 = Scan en continu (non-bloquant)  
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

  // Wait up to 2s Serial to be ready
  while ( !Serial && (millis() - start < 2000)) delay(10); 

  // Init Bluefruit
  Bluefruit.begin();
  
  blue_addr = Bluefruit.getAddr();
  // 17 chars (XX:XX:XX:XX:XX:XX) + 1 for \0
  
  sprintf(blue_addr_str, "%02X:%02X:%02X:%02X:%02X:%02X",
          blue_addr.addr[5], blue_addr.addr[4], blue_addr.addr[3], 
          blue_addr.addr[2], blue_addr.addr[1], blue_addr.addr[0]);


  Serial.printf("Bluetooth adress %s\n", blue_addr_str);

  // Set Emission Power to be received
  Bluefruit.setTxPower(4);

  // Build Advising name
  build_advising_name(topics[topics_index]);

  // Get topic and advising name
  Serial.printf("Topic '%s' and Bluetooth Advising '%s'\n", topics[topics_index], ble_name);

  // Set name
  Bluefruit.setName(ble_name);

  // Start advertising in Bluetooth
  start_advertising();

  // Start scanner too
  start_scanner();
}


void loop() {

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

  if (Bluefruit.Scanner.isRunning())
    Serial.print("+");
  else
    Serial.print("_");

  unsigned long current_time = millis();
  
  // Vérifie si l'intervalle de temps est écoulé
  if (current_time > (last_update_time + ADVERTISING_TOPIC_DURATION_MS)) {
    
    Serial.printf("\n\nBluetooth adress %s\n", blue_addr_str);

    // Find next topic
    next_topic();

    // Build advertising name
    build_advising_name(topics[topics_index]);

    // Get topic and advertising name
    Serial.printf("Topic '%s' and Bluetooth Advising '%s'\n", topics[topics_index], ble_name);

    // Stop advertising
    Bluefruit.Advertising.stop();

    // Set name
    Bluefruit.setName(ble_name);

    // Start advertising
    // Bluefruit.Advertising.start();
    start_advertising();

    // New time
    last_update_time = current_time;
  }

  if (current_time >= (last_scan_restart_time + SCAN_RESTART_INTERVAL_MS)) {
        
        Serial.println("\n[Scan Restart] Nettoyage du cache.");
        
        // Mettre à jour le temps de redémarrage
        last_scan_restart_time += SCAN_RESTART_INTERVAL_MS;

        // Arrêter et redémarrer le scanner pour vider son cache interne
        if (Bluefruit.Scanner.isRunning()) {
            Bluefruit.Scanner.stop();
            Bluefruit.Scanner.start(0);
        }
    }  
}
