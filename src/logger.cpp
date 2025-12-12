#define __LOGGER_CPP__

#include "mg.h"

#include <stdarg.h> // Nécessaire pour la gestion des arguments variables
#include <string.h> // Nécessaire pour strlen()
#include <stdio.h>  // Nécessaire pour vsnprintf

// Taille du buffer pour le formatage. 512 octets est un choix sûr.
#define LOG_BUFFER_SIZE 512

char log_buffer[LOG_BUFFER_SIZE];

void logger_impl(const char *fmt, ...) {
    
    // --- 1. Gestion de l'Horodatage ---
    unsigned long current_time_ms = millis();
    char time_buffer[9]; // HH:MM:SS\0
    ms_to_hms(current_time_ms, time_buffer); 
    
    // Afficher l'estampille temporelle immédiatement
    Serial.printf("[%s] ", time_buffer); 
    
    // --- 2. Formatage dans un Buffer Mémoire ---
    
    va_list args; 
    va_start(args, fmt);
    
    // Utilisation de vsnprintf : version sécurisée de vsprintf.
    // Elle ne dépassera JAMAIS la taille de LOG_BUFFER_SIZE.
    vsnprintf(log_buffer, LOG_BUFFER_SIZE, fmt, args);
    va_end(args);
    
    // --- 3. Impression du Buffer ---
    
    // Utiliser la fonction Serial.printf EXISTANTE pour imprimer le contenu formaté.
    Serial.printf("%s", log_buffer);
}

