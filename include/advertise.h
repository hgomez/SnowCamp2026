#ifndef __ADVERTISE_CPP__

extern void start_advertising(void);

#endif

// Emit mode in fast mode
#define ADVERTISING_DURATION_FASTMODE 3

// Advertising topic duration in milliseconds
// Receivers have cache so emit long enough, the Cache GATT (Generic Attribute Profile Cache)
// 15s on iOS 15s, 30s on Android/Windows/Linux
//
#define ADVERTISING_TOPIC_DURATION_MS (35 * 1000)

