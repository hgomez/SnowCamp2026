#define __ADVERTISE_CPP__

#include "mg.h"

/**
 * Callback invoked when advertising is stopped by timeout
 */
void adv_stop_callback(void)
{
  Serial.printf("Unexpected end of advertising. next topic #%d\n", topics_index);
}

void start_advertising(void)
{   
  // Advertising packet
  Bluefruit.Advertising.clearData();
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
  Bluefruit.Advertising.setStopCallback(adv_stop_callback);
  Bluefruit.Advertising.restartOnDisconnect(true);
  // Bluefruit.Advertising.setInterval(32, 244);                        // in units of 0.625 ms
  Bluefruit.Advertising.setInterval(32, 32);                            // in units of 0.625 ms => 20ms fast mode
  Bluefruit.Advertising.setFastTimeout(ADVERTISING_DURATION_FASTMODE);   // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                                       // Advertise forever 
}

