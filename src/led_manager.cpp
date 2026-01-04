#include "led_manager.h"

void init_social_leds() {
    pinMode(LED_G1, OUTPUT); pinMode(LED_G2, OUTPUT); pinMode(LED_G3, OUTPUT);
    pinMode(LED_R1, OUTPUT); pinMode(LED_R2, OUTPUT); pinMode(LED_R3, OUTPUT);
    clear_social_leds();
}

void clear_social_leds() {
    digitalWrite(LED_G1, LOW); digitalWrite(LED_G2, LOW); digitalWrite(LED_G3, LOW);
    digitalWrite(LED_R1, LOW); digitalWrite(LED_R2, LOW); digitalWrite(LED_R3, LOW);
}

void update_social_leds(const std::unordered_map<std::string, int>& topicCounts) {
    int score_plus = 0;
    int score_moins = 0;

    // Analyse des topics détectés
    for (auto const& [topic, count] : topicCounts) {
        if (count > 0) {
            if (topic[0] == '+') {
                score_plus += count;
            } else if (topic[0] == '-') {
                score_moins += count;
            }
        }
    }

    // --- Logique d'affichage ---
    // On allume les LEDs progressivement selon le score (1, 2, ou 3+)
    
    // VERTES (J'aime)
    digitalWrite(LED_G1, (score_plus >= 1) ? HIGH : LOW);
    digitalWrite(LED_G2, (score_plus >= 2) ? HIGH : LOW);
    digitalWrite(LED_G3, (score_plus >= 3) ? HIGH : LOW);

    // ROUGES (J'aime pas)
    digitalWrite(LED_R1, (score_moins >= 1) ? HIGH : LOW);
    digitalWrite(LED_R2, (score_moins >= 2) ? HIGH : LOW);
    digitalWrite(LED_R3, (score_moins >= 3) ? HIGH : LOW);
}
