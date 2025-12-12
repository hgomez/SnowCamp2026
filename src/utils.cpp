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


#ifdef __arm__
// Variables externes pour le suivi du tas (Heap) par le compilateur GCC/newlib
extern "C" char *sbrk(int i);
#else
// Définition de sbrk factice pour les environnements de compilation non ARM
char *sbrk(int incr) {
  static char *heap_ptr = NULL;
  char *prev_heap_ptr;
  if (heap_ptr == NULL) {
    heap_ptr = (char *)malloc(1024); // Simuler une petite zone
  }
  prev_heap_ptr = heap_ptr;
  heap_ptr += incr;
  return prev_heap_ptr;
}
#endif

/***
 */

// Adresse de fin physique de la RAM pour le NRF52840 (256KB)
// 0x20000000 (Base) + 0x40000 (256KB) = 0x20040000
const uint32_t RAM_END = 0x20040000; 

size_t get_free_ram() {
  char *heap_end = sbrk(0);
  // On calcule l'espace entre la fin actuelle du tas et la fin physique de la RAM
  return (size_t)(RAM_END - (uint32_t)heap_end);
}

/***
 * Calcul de la plus grande mémoire allouable
 */
size_t get_max_allocatable() {

  size_t size = 150000; // On commence gros (150ko)
  size_t step = 10000;  // Pas de 10ko
  void* ptr = NULL;

  // On essaie de réduire la taille tant qu'on n'arrive pas à allouer
  while (size > 0) {
    ptr = malloc(size);
    if (ptr != NULL) {
      free(ptr); // On libère tout de suite !
      return size;
    }
    size -= step;
    if (size < step) step = 100; // Affinage pour la fin
  }
  return 0;
}
