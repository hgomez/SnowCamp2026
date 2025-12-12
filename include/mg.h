#include <Arduino.h>
#include <bluefruit.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#define LOG_LEVEL_FATAL 1
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_WARN  3
#define LOG_LEVEL_INFO  4
#define LOG_LEVEL_DEBUG 5
#define LOG_LEVEL_TRACE 6


#ifndef LOG_LEVEL 
#define LOG_LEVEL LOG_LEVEL_TRACE 
#endif

// Emit mode in fast mode
#define ADVERTISING_DURATION_FASTMODE 3

// Advertising topic duration in milliseconds
// Receivers have cache so emit long enough, the Cache GATT (Generic Attribute Profile Cache)
// 15s on iOS 15s, 30s on Android/Windows/Linux
//
#define ADVERTISING_TOPIC_DURATION_MS (35 * 1000)

// Restart Scanner regularly (5s)
#define SCAN_RESTART_INTERVAL_MS (5 * 1000)

// On garde l'information sur les cartes pendant 10mn (en millisecondes)
#define CARD_TIMEOUT_MS (10 * 60 * 1000)

// Périodicité du nettoyage des cartes (30s)
#define CLEANUP_INTERVAL_MS (30 * 1000)

#ifndef __ADVERTISE_CPP__
extern void start_advertising(void);
#endif

#ifndef __MAIN_CPP__
extern unsigned int topics_index;
#endif

#ifndef __SCAN_CPP__
extern void start_scanner(void);
#endif

#ifndef __PEERS_CPP__
extern void update_peers(std::string addr, std::string topic);
extern void cleanup_peers(void);
extern void print_peers_stats(void);
#endif

#ifndef __UTILS_CPP__
extern void addr_to_str(const ble_gap_addr_t& addr, char* str);
extern void print_adv_data(const uint8_t* data, uint8_t len);
extern size_t get_free_ram(void);
extern size_t get_max_allocatable(void);
extern void print_addresses(void);

#endif

#include <malloc.h> // Nécessaire pour mallinfo
