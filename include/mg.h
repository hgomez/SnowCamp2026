#include <Arduino.h>
#include <bluefruit.h>
#include <string>
#include <vector>

#include "advertise.h"
#include "display.h"
#include "main.h"
#include "led_manager.h"
#include "logger.h"
#include "peers.h"
#include "utils.h"

#ifndef LOG_LEVEL 
#define LOG_LEVEL LOG_LEVEL_TRACE 
#endif

// Restart Scanner regularly (5s)
#define SCAN_RESTART_INTERVAL_MS (5 * 1000)

// On garde l'information sur les cartes pendant 10mn (en millisecondes)
#define CARD_TIMEOUT_MS (10 * 60 * 1000)

// Périodicité du nettoyage des cartes (30s)
#define CLEANUP_INTERVAL_MS (30 * 1000)






#ifndef __SCAN_CPP__
extern void start_scanner(void);
#endif


