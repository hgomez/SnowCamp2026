#define __LED_MANAGER_CPP__

#include "mg.h"
#include <math.h>

// Tableau des pins dans l'ordre (D3-D5 pour +, D6-D8 pour -)
static const int LED_PINS[] = {D3, D4, D5, D6, D7, D8};

// Structure interne pour stocker l'état
struct TopicState {
    int pin;
    int count;
};

// Map dynamique : Clé = Nom du topic, Valeur = Pin + Compteur
static std::unordered_map<std::string, TopicState> ledMap;

void init_social_leds() {
    ledMap.clear();
    
    // Construction dynamique de la map basée sur le tableau 'topics'
    for (unsigned int i = 0; i < NUM_TOPICS; i++) {
        pinMode(LED_PINS[i], OUTPUT);
        digitalWrite(LED_PINS[i], LOW);
        
        // On associe le nom du topic au pin correspondant par l'index
        ledMap[topics[i]] = { LED_PINS[i], 0 };

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
        logger("[init_social_leds] topics %d topic %s pin %d count %d\n", 
            i, 
            topics[i], 
            ledMap[topics[i]].pin, 
            ledMap[topics[i]].count);
#endif

    }
}

void update_led_counts(const std::unordered_map<std::string, int>& ptopicCounts) {
    // On met à jour les compteurs internes de notre ledMap
    for (auto& [name, state] : ledMap) {
        if (ptopicCounts.count(name)) {
            state.count = ptopicCounts.at(name);
        } else {
            state.count = 0;
        }
    }
}

void refresh_led_effects() {
    unsigned long now = millis();

    for (auto const& [name, state] : ledMap) {
        if (state.count <= 0) {
            analogWrite(state.pin, 0);
            continue;
        }

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
        logger("[refresh_led_effects] name %s pin %d count %d\n", 
            name, 
            state.pin, 
            state.count);
#endif

        // --- Calcul de la Pulsation PWM ---
        // Fréquence f = 1.0 + (n * 0.5) Hz (ajustable selon tes préférences)
        float frequency = 0.5 + (state.count * 0.5); 
        float phase = (now / 1000.0) * 2.0 * M_PI * frequency;
        
        int dutyCycle = (int)((sin(phase) * 127.5) + 127.5);
        
        analogWrite(state.pin, dutyCycle);
    }
}
