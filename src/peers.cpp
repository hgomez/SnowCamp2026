#define __PEERS_CPP__

#include "mg.h"

struct CardInfo {
    std::string addr;       // Adresse MAC de la carte
    unsigned long lastSeen; // Timestamp du dernier scan
    std::unordered_set<std::string> topics; // Un ensemble de topics DISTINCTS vus pour cette carte
};

// La Map des cartes détectées, indexée par l'adresse MAC
std::unordered_map<std::string, CardInfo> detectedCards;

// La Map des statistiques de fréquences (inchangée)
// Key: Topic (ex: "+JAVA"), Value: Nombre de cartes diffusant ce topic
std::unordered_map<std::string, int> topicCounts;

/*
 * On a reçu un topic depuis une adresse 
*/
void update_peers(std::string addr, std::string topic) 
{
    // Si le topic ne correspond pas à un des notres, fan ou detracteur, on ne le retient pas
    if ((! is_buddy(topic.c_str())) && (! is_detractor(topic.c_str()))) {

        #if LOG_LEVEL >= LOG_LEVEL_INFO
            Serial.printf(">> Topic '%s' is neither buddy nor detractor, discarding\n", topic.c_str());
        #endif

        return;
    }

// 1. S'assurer que l'entrée existe
      if (detectedCards.count(addr) == 0) {
          CardInfo info;
          info.addr = addr;
          detectedCards[addr] = info;
      }

      // 2. Mettre à jour le timestamp
      detectedCards[addr].lastSeen = millis();
      
      // 3. Tenter d'insérer le topic. 'inserted' est vrai si le topic est nouveau pour cette carte.
      auto [it, inserted] = detectedCards[addr].topics.insert(topic);

      if (inserted) {
          // Le topic est nouveau pour cette carte -> Mettre à jour les statistiques globales
#if LOG_LEVEL >= LOG_LEVEL_INFO
          Serial.printf(">> NRF Detected! Addr: %s, NEW Topic added: '%s'\n", addr.c_str(), topic.c_str());
#endif
          topicCounts[topic]++;
      } else {
          // 
#if LOG_LEVEL >= LOG_LEVEL_INFO
          Serial.printf(">> NRF Detected! Topic '%s' already known for %s\n", topic.c_str(), addr.c_str());
#endif          
      }
}

/*
 * Menage dans les peers, pour supprimer les définitions trop anciennes
*/
void cleanup_peers() 
{
    unsigned long current_time = millis();
    std::vector<std::string> addresses_to_remove; 

    // Étape 1 : Identifier les cartes expirées et décrémenter les topics
    for (auto const& [addr_key, info] : detectedCards) 
    {
        if (current_time - info.lastSeen > CARD_TIMEOUT_MS) 
        {
            addresses_to_remove.push_back(addr_key);
            
            // Décrémenter TOUS les topics de cette carte
            for (const std::string& expired_topic : info.topics) {
                 if (topicCounts.count(expired_topic)) {
                     topicCounts[expired_topic]--;
                 }
            }
        }
    }

    // Étape 2 : Supprimer les entrées de detectedCards et nettoyer topicCounts
    for (const std::string& addr : addresses_to_remove) 
    {
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
        Serial.printf("[CLEANUP] Removing expired card: %s\n", addr.c_str());
#endif
        detectedCards.erase(addr);
    }
    
    // Nettoyer topicCounts (supprimer les topics dont le compteur est tombé à 0)
    std::vector<std::string> topics_to_remove;
    for (auto const& [topic_key, count] : topicCounts) {
        if (count <= 0) {
            topics_to_remove.push_back(topic_key);
        }
    }
    for (const std::string& topic : topics_to_remove) {
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
        Serial.printf("[CLEANUP] Removing empty topic: '%s'\n", topic.c_str());
#endif
        topicCounts.erase(topic);
    }
}

/***
 * On affiche le stats sur les cartes
 */
void print_peers_stats() {

#if LOG_LEVEL >= LOG_LEVEL_INFO

      // Afficher l'état actuel des topics
      Serial.println("\n--- TOPIC STATS ---");
      for (auto const& [topic, count] : topicCounts) {
          if (count > 0) { // N'afficher que les topics actifs
            Serial.printf("Topic: '%s' | Count: %d\n", topic.c_str(), count);
          }
      }
      
      // Afficher l'état actuel des cartes vues
      Serial.println("--- CARDS SEEN ---");
      for (auto const& [addr, info] : detectedCards) {
          Serial.printf("Card: %s | LastSeen: %lu | Topics: %u\n", info.addr.c_str(), info.lastSeen, info.topics.size());
      }

#endif
      
}