#define __SCAN_CPP__

#include "mg.h"

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

/*
 * Scan Callback
 *
 * A priori pas réentrant, il faut donc reset régulièrement (c'est balo)
 * 
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
