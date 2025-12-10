#include <Arduino.h>
#include <bluefruit.h>

// Emit mode in fast mode
#define ADVERTISING_DURATION_FASTMODE 3

// Advertising topic duration in milliseconds
// Receivers have cache so emit long enough, the Cache GATT (Generic Attribute Profile Cache)
// 15s on iOS 15s, 30s on Android/Windows/Linux
//
#define ADVERTISING_TOPIC_DURATION_MS 35000L

// Restart Scanner regularly (5s)
#define SCAN_RESTART_INTERVAL_MS 5000L

#ifndef __ADVERTISE_CPP__
extern void start_advertising(void);
#endif

#ifndef __MAIN_CPP__
extern unsigned int topics_index;
#endif

#ifndef __SCAN_CPP__
extern void start_scanner(void);
#endif

