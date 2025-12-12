#define __DISPLAY_CPP_

#include "mg.h"

/***
 * Turn all leds OFF
 */
void all_leds_off() {

  //  Output is LOW, no current flow from GPIO
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);
}

/***
 * Turn all leds ON
 */
void all_leds_on() {

  //  Output is HIGH, current flow from GPIO to LED and ground
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
  digitalWrite(D5, HIGH);
  digitalWrite(D6, HIGH);
  digitalWrite(D7, HIGH);
  digitalWrite(D8, HIGH);    
}

void init_leds() {

    // initialize digital pin D3-D8 and the built-in LED as an output.
    pinMode(D3,OUTPUT);
    pinMode(D4,OUTPUT);
    pinMode(D5,OUTPUT);
    pinMode(D6,OUTPUT);
    pinMode(D7,OUTPUT);
    pinMode(D8,OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT); 

    for (int i = 0; i < 3; i++) {
        all_leds_off();
        delay(250);
        all_leds_on();
        delay(250);
    }
}


void circle_one_led(int time) {

    all_leds_off();

    digitalWrite(D3, HIGH);
    delay(time);
    
    digitalWrite(D3, LOW);
    digitalWrite(D4, HIGH);
    delay(time);

    digitalWrite(D4, LOW);
    digitalWrite(D5, HIGH);
    delay(time);

    digitalWrite(D5, LOW);
    digitalWrite(D6, HIGH);
    delay(time);

    digitalWrite(D6, LOW);
    digitalWrite(D7, HIGH);
    delay(time);

    digitalWrite(D7, LOW);
    digitalWrite(D8, HIGH);
    delay(time);

    digitalWrite(D8, LOW);
}

void update_status_led() {

  static int status_cnt = 0;

  status_cnt++;
  digitalWrite(LED_BUILTIN,(status_cnt & 0x0001) ? HIGH : LOW);
  delay(100);
}

void update_health_led() {

  static int health_cnt = 0;

  all_leds_off();

  switch(health_cnt) {
    case 0 : digitalWrite(D3,HIGH);break;
    case 1 : digitalWrite(D4,HIGH);break;
    case 2 : digitalWrite(D5,HIGH);break;
    case 3 : digitalWrite(D6,HIGH);break;
    case 4 : digitalWrite(D7,HIGH);break;
    case 5 : digitalWrite(D8,HIGH);break;
    default: break;
  }

  health_cnt = ( health_cnt + 1 ) % 6;
  delay(500);
}