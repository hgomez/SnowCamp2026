#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Arduino.h>
#include <unordered_map>
#include <string>

// Définition des Pins (Xiao nRF52840)
#define LED_G1 D3
#define LED_G2 D4
#define LED_G3 D5
#define LED_R1 D6
#define LED_R2 D7
#define LED_R3 D8

/**
 * Initialise les pins des LEDs en sortie
 */
void init_social_leds();

/**
 * Calcule les scores et met à jour l'état des LEDs
 * @param topicCounts La map contenant les statistiques des topics
 */
void update_social_leds(const std::unordered_map<std::string, int>& topicCounts);

/**
 * Éteint toutes les LEDs
 */
void clear_social_leds();

#endif