#define __ADVERTISE_CPP__

#include "mg.h"

typedef volatile uint32_t REG32;

#define pREG32 (REG32 *)
#define MAC_ADDRESS_LOW   (*(pREG32 (0x100000a4)))

// ble_name is a char array to hold Bluetooth name 
// 14 chars and a trailing 0x00
static char ble_name[15];

/**
 * Callback invoked when advertising is stopped by timeout
 */
void advertising_stop_callback(void)
{
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
  logger("[advertising_stop_callback] end of advsertision on %s\n", ble_name);
#endif

  // Next index
  topics_index = (topics_index + 1) % NUM_TOPICS;

  start_advertising();
}

void start_advertising(void)
{   
  // Construit le nom de diffusion sur 14 caractères, on ne peut pas plus large
  // 14 c'est 3 + 4 + 7 soit :
  // M&GAAAASSSSSSS où AAAA sont les 16bits bas de l'adresse Bluetooth et SSSSSSS le sujet (7 caractères max)
  sprintf(ble_name,"M&G%04lX%.7s", (MAC_ADDRESS_LOW) & 0xFFFF, topics[topics_index]);

  // Clean
  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();
  Bluefruit.ScanResponse.clearData();
  Bluefruit.setName(ble_name);

  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.setStopCallback(advertising_stop_callback);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(96, 244);                           // in units of 0.625 ms (every 60ms, not spam)
  Bluefruit.Advertising.setFastTimeout(ADVERTISING_DURATION);           // number of seconds in fast mode
  Bluefruit.Advertising.start(ADVERTISING_DURATION);                    // Advertise forever 

  // Get topic and advertising name
  logger("[start_advertising] Diffusion du sujet '%s' et message d'annonce Bluetooth '%s'\n", topics[topics_index], ble_name);
}

