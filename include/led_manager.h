#ifndef __LED_MANAGER_CPP__

#include <Arduino.h>
#include <unordered_map>
#include <string>

#include <unordered_map>
#include <string>

/**
 * Assignation des Pins sur le Seeed Xiao nRF52840
 * Les 3 premières sont généralement Vertes (+) et les 3 suivantes Rouges (-)
 */
#define LED_PIN_1 D3
#define LED_PIN_2 D4
#define LED_PIN_3 D5
#define LED_PIN_4 D6
#define LED_PIN_5 D7
#define LED_PIN_6 D8

/**
 * Initialise le gestionnaire de LEDs.
 * Configure les pins en SORTIE et construit la correspondance dynamique 
 * entre le tableau global 'topics' et les pins physiques.
 */
void init_social_leds();

/**
 * Met à jour les compteurs internes de chaque LED.
 * Doit être appelé régulièrement (ex: toutes les 10s après le nettoyage du cache).
 * * @param topicCounts La map <string, int> contenant les résultats du scan BLE.
 */
void update_led_counts(const std::unordered_map<std::string, int>& topicCounts);

/**
 * Gère l'animation de pulsation PWM.
 * Cette fonction est non-bloquante et doit être appelée à chaque itération 
 * de la fonction loop() principale pour garantir la fluidité de l'effet.
 */
void refresh_led_effects();

#endif