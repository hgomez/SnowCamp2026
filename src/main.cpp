#define __MAIN_CPP__

#include "mg.h"

typedef volatile uint32_t REG32;

#define pREG32 (REG32 *)
#define MAC_ADDRESS_LOW   (*(pREG32 (0x100000a4)))

// Last Scan Restart time
unsigned long last_scan_restart_time = 0;

// Last cleanup time
unsigned long last_cleanup_time = 0;

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
const char *topics[] = { "+JAVA", "+6502" , "+DEVOPS", "-Node", "-REACT", "-K8S" };

// Compute array size
const unsigned int NB_TOPICS = sizeof(topics) / sizeof(topics[0]);

// Our topic index
unsigned int topics_index = 0;

// Last time advising started
unsigned long last_update_time = 0;

// Bluetooth address
ble_gap_addr_t blue_addr;

// Bluetooth address as string
char blue_addr_str[18]; 

// mémoire allouable maximum
size_t max_allocatable_memory;

/***
 * On cherche si le topic envoyé correspond à un buddy
 * Un buddy met un + sur de nos +, exemple il envoie +JAVA et on a mis +JAVA
 * Un buddy met un - sur de nos -, exemple il envoie -NODE et on a mis -NODE
 * On doit avoir une correspondance sur les chaines
 */
boolean is_buddy(const char * topic) {

  for (unsigned int i = 0; i < NB_TOPICS; i++) {
    // topic correspondant, signe et nom ?
    if (strcasecmp(topics[i], topic) == 0)
      return (true);
  }
  
  return (false);
}

/***
 * On cherche si le topic envoyé correspond à un detractor
 * Un detractor met un + sur de nos -, exemple il envoie +NODE et on a mis -NODE
 * Un detractor met un - sur de nos +, exemple il envoie -JAVA et on a mis °JAVA
* On doit avoir une correspondance sur les noms et une différence sur les signes
*/
boolean is_detractor(const char * topic) {

  for (unsigned int i = 0; i < NB_TOPICS; i++) {

    // nom de topic correspondant ?
    if (strcasecmp(&topics[i][1], &topic[1]) == 0) {

      // j'ai mis +, il a mis -
      if (topics[i][0] == '+' && topic[0] == '-')
        return (true);

      // j'ai mis -, il a mis +
      if (topics[i][0] == '-' && topic[0] == '+')
        return (true);
    }
  }

  return (false);
}

/***
 * Move to next topic, increment topics_index variables and wrap around
 */
void next_topic() {
    
  Serial.printf("current topic #%d\n", topics_index);

  // Increment topic index
  topics_index++; 
  
  // If topic index is too large, back to 0
  if (topics_index >= NB_TOPICS) { 
    topics_index = 0;
  }

  Serial.printf("next topic #%d\n", topics_index);
}


char * build_advising_name(const char * lesujet) {
  sprintf(&ble_name[0],"M&G%04lX%.7s", (MAC_ADDRESS_LOW) & 0xFFFF, lesujet);
  return &ble_name[0];
}


void setup() {
  // On commence par calculer la taille maximum allouable via malloc
  max_allocatable_memory = get_max_allocatable();

  init_leds();

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

  update_status_led();

  if (Bluefruit.Scanner.isRunning())
    update_health_led();

  unsigned long current_time = millis();
  
  // Vérifie si l'intervalle de temps est écoulé
  if (current_time > (last_update_time + ADVERTISING_TOPIC_DURATION_MS)) {
    
    Serial.printf("\nMy Bluetooth address %s at %ld\n", blue_addr_str, current_time);

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

  current_time = millis();

  if (current_time >= (last_scan_restart_time + SCAN_RESTART_INTERVAL_MS)) {
        
        Serial.printf("\n[Scan Restart] Nettoyage du cache at %ld\n", current_time);
        
        // Mettre à jour le temps de redémarrage
        last_scan_restart_time = current_time;

        // Arrêter et redémarrer le scanner pour vider son cache interne
        if (Bluefruit.Scanner.isRunning()) {
            Bluefruit.Scanner.stop();
            Bluefruit.Scanner.start(0);
        }
    }  

  if (current_time >= (last_cleanup_time + CLEANUP_INTERVAL_MS)) {
      
      cleanup_peers();
      print_peers_stats();

      last_cleanup_time += CLEANUP_INTERVAL_MS;

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
  Serial.printf("Estimation mémoire libre %d - max disponible initialement %d\n", get_free_ram(), max_allocatable_memory);  
#endif

  }    
}
