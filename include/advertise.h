// Adversiting Duration (in seconds)
#define ADVERTISING_DURATION 1

// Advertising topic duration in milliseconds
// Receivers have cache so emit long enough, the Cache GATT (Generic Attribute Profile Cache)
// 15s on iOS 15s, 30s on Android/Windows/Linux
//
#define ADVERTISING_TOPIC_DURATION_MS (35 * 1000)

extern void start_advertising(void);
extern void advertising_stop_callback(void);
