## ❄️ SnowCamp 2026 - Proposition de Lab NRF52840

### Thème : Le Thermomètre Social BLE (Bluetooth Low Energy) - Meet & Chips

---

### I. 📋 Prérequis et Préparation (À faire avant le Lab)

Pour garantir que les 3 heures soient dédiées au code et non à l'installation, les participants doivent avoir :

#### A. Outils de Développement (Déjà mentionnés)

* **IDE :** [VSCode](https://code.visualstudio.com/)
* **Extension :** [PlatformIO Extension](https://docs.platformio.org/en/latest/integration/ide/vscode.html)
* **Pilotes :** Les pilotes USB/UART nécessaires pour le type de carte utilisé (ex: Siag Labs, Adafruit).

#### B. Matériel Requis

* **Carte :** **Xiao nRF52840** 
    * *Raison :* Le Xiao est compact et économique, parfait pour un Lab.
* **Câble :** Câble USB-C (pour le Xiao) ou Micro-USB (pour le Feather).

#### C. Configuration de PlatformIO (À préparer)

1.  Créer un nouveau projet PlatformIO.
2.  Utiliser la plateforme `nordicnrf52`.
3.  Installer le Core Arduino Adafruit pour le nRF52.
4.  Vérifier que le programme de base ("Blink") compile et se télécharge.

---

### II. 🎯 Objectif du Lab (3 Heures Chrono)

L'objectif est de construire un "Thermomètre Social" simple où les cartes nRF52 diffusent un "état" et scannent l'environnement pour comptabiliser les autres états.

#### A. Étapes Techniques

1.  **Advertising (1h00) :** Mettre en place la diffusion d'un paquet BLE *Non-Connectable* (comme dans notre discussion précédente) avec un nom structuré :
    * **Format :** `M&G<ID>+<TOPIC>` (Ex: `M&G1234+JAVA ` ou `M&G4567-NODE `)
    * **Rotation :** Implémenter la rotation du sujet toutes les 35 secondes (`advertising_topic_duration_ms`).
    * *Niveau 1 :* Rotation manuelle (`next_topic()`).
    * *Niveau 2 :* Rotation basée sur un capteur (si la carte en a un, ex: bouton, température).

2.  **Scanning (1h30) :** Implémenter le balayage continu pour détecter les autres cartes `M&G`.
    * **Implémentation :** Utiliser la `scan_callback` pour filtrer les noms et extraire le `TOPIC`.
    * **Problème de Cache :** Utiliser le contournement du cache (redémarrage du scan toutes les 5 secondes) pour garantir la détection continue.

3.  **Comptage & Affichage (30 min) :** Maintenir un tableau ou une structure de données pour comptabiliser les *topics* vus récemment.
    * Afficher sur le **Moniteur Série** le Top 3 des sujets les plus populaires vus par la carte.

#### B. Thèmes possibles pour la Rotation

* **Humeur :** `+FEELING` (Bien), `-FEELING` (Moyen), `~FEELING` (Fatigué).
* **Sujets de Conférence :** `+IA`, `-OPS`, `+WEB`.
* **Boissons :** `+COFFEE`, `-TEA`, `+BEER`.

---

### III. 📝 Conclusion et Next Steps (Pour la suite)

* **Défis :** Comment gérer le temps écoulé (timeout) pour que les sujets cessent d'être comptés après 60 secondes ? (Utilisation de structures `struct` et `millis()` pour l'expiration).
* **Amélioration :** Afficher l'état sur un LED (ex: LED Verte si `+TOPIC` est le plus vu, LED Rouge si `-TOPIC` est le plus vu).

Ce Lab touche à la gestion de l'état (rotation), au BLE (Advertising/Scanning) et aux défis réels de l'embarqué (timing, cache de librairie), ce qui en fait un excellent exercice de 3 heures.