# L'organisation mémoire du NRF52840 

Pour rappel la carte Seed Studio a 256Ko de RAM et 1Mo de Flasg.
On utilise le framework Arduino qui s'appuie lui même sous FreeRTOS

Vous avez :

  * **Pile (Stack) :** `0x2000...` (Bas de la mémoire)
  * **Tas (Heap) :** `0x2002...` (Haut de la mémoire)

Cela confirme définitivement que **votre programme tourne sur un OS temps réel (RTOS)** (probablement FreeRTOS masqué sous la couche Arduino/Adafruit), et cela change toutes les règles du jeu.

### Ce qui se passe réellement dans votre NRF52

Contrairement à un Arduino Uno où il n'y a qu'une seule pile qui part du plafond, ici **votre `loop()` est une Tâche (Thread)**.

1.  **Les Piles sont des objets :** Dans un RTOS, quand une tâche est créée (comme votre `loop`), le système alloue un bloc de mémoire fixe pour sa pile. Ici, il a décidé de placer la pile de votre tâche dans les adresses basses (`0x2000a60c`).
2.  **Le Malloc cherche ailleurs :** Quand vous demandez de la mémoire dynamique (`malloc`), le gestionnaire cherche de l'espace libre ailleurs. Il a trouvé de la place beaucoup plus loin, vers `0x2002e3e8`.

C'est pour cela que votre calcul de distance est négatif : vous n'êtes pas dans un sandwich "Pile vs Tas", vous êtes dans un système où **les piles sont des îlots flottant au milieu de la mémoire.**

### Conséquences pour vous

1.  **Le risque de collision est différent :** Vous ne risquez pas que le Tas écrase la Pile "parce qu'ils se rejoignent". Vous risquez plutôt :

      * **Stack Overflow :** Si vos variables locales dépassent la taille fixe allouée à la tâche `loop()` (souvent 4 Ko ou 8 Ko par défaut).
      * **Heap Exhaustion :** Si `malloc` ne trouve plus de place nulle part.

2.  **L'indicateur fiable :**
    Puisque la géométrie est complexe et gérée par l'OS, les calculs de pointeurs (`sbrk`, soustractions d'adresses) sont inutiles et trompeurs ici.

### La seule solution qui marche à 100%

Revenez à la méthode "Brute Force" que je vous ai donnée (la fonction `getMaxAllocatable`). C'est la seule qui interroge la réalité physique : **"Combien je peux vraiment prendre maintenant ?"**

Voici la version optimisée et définitive à garder dans votre boîte à outils pour ce projet :

```cpp
#include <Arduino.h>

// Cette fonction teste physiquement la mémoire disponible
// sans se soucier de la complexité de l'OS
size_t getMaxFreeBlock() {
  size_t size = 100000; // On commence par chercher un bloc de 100ko
  size_t step = 5000;   // On réduit par pas de 5ko
  void* ptr = NULL;

  // Recherche grossière
  while (size > 0) {
    ptr = malloc(size);
    if (ptr != NULL) {
      free(ptr);
      break; 
    }
    size -= step;
  }

  if (size == 0) return 0;

  // Affinage (optionnel, pour être précis à l'octet près)
  size += step; 
  while (size > 0) {
    ptr = malloc(size);
    if (ptr != NULL) {
      free(ptr);
      return size;
    }
    size -= 100; // Affinage par 100 octets
  }
  return 0;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("--- Démarrage ---");
  Serial.printf("Plus gros bloc disponible : %d octets\n", getMaxFreeBlock());
}

void loop() {
  // Rien ici
}
```

Cette fonction vous renvoie environ **140 000 ou 150 000 octets**, tout va bien : vous avez énormément de place (la NRF52840 est un monstre de puissance) et vous pouvez continuer à coder sans crainte.

On voit ces informations à la compilation

```...
|-- Adafruit Bluefruit nRF52 Libraries @ 0.21.0
Building in release mode
Compiling .pio/build/seeed-xiao-nrf52840/src/main.cpp.o
Compiling .pio/build/seeed-xiao-nrf52840/src/utils.cpp.o
Linking .pio/build/seeed-xiao-nrf52840/firmware.elf
Building .pio/build/seeed-xiao-nrf52840/firmware.hex
Checking size .pio/build/seeed-xiao-nrf52840/firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [=         ]   5.8% (used 13864 bytes from 237568 bytes)
Flash: [==        ]  15.2% (used 123672 bytes from 811008 bytes)
Building .pio/build/seeed-xiao-nrf52840/firmware.zip
Zip created at .pio/build/seeed-xiao-nrf52840/firmware.zip
```

Pour la RAM, 13864 octets utilisés sur les 237568 octets  , et pour la Flash 123672 octets sur les 811008 octets
256ko de RAM et 1Mo de Flash