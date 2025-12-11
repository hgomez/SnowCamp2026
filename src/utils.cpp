#define __UTIL_CPP__

#include "mg.h"

/*
   Retourne une chaine de caractère de l'adress Mac fournie, au format AA:BB:CC:DD:EE:FF 
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
