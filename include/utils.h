#ifndef __UTILS_CPP__

#include <Arduino.h>
#include <bluefruit.h>

extern void addr_to_str(const ble_gap_addr_t& addr, char* str);
extern void print_adv_data(const uint8_t* data, uint8_t len);
extern size_t get_free_ram(void);
extern size_t get_max_allocatable(void);
extern char * ms_to_hms(unsigned long ms, char* buffer);

#endif
