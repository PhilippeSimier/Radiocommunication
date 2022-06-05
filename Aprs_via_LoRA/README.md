# Tracker Aprs via LoRa

## Objectifs
Le projet est de réaliser un **traqueur APRS** (TX UHF) ainsi qu'une **passerelle** (RX) qui va renvoyer les informations reçues sur le serveur aprs.fi. 
Les objectifs  sont, entre autres, une faible consommation d’énergie et une longue portée  entre le tracker et la passerelle (15 km environ).

![aprs.fi](/Aprs_via_LoRA/images/Lora_Aprs.png)

## Principe

Le mécanisme d’échange de données est le suivant  : toutes les passerelles du réseau sont  
positionnées en mode « écoute », et dès  qu’un tracker situé dans leur champ de détection   transmet une donnée, elles reçoivent toutes la donnée qu’elles retransmettent au serveur  de stockage et de traitement de données  aprs.fi, en y  ajoutant une information de niveau de réception de la donnée appelé  RSSI  (Received  Signal  Strenght  Indication)

 - La technologie  **LoRa**, aussi parfois appelée  LoRa RF,  désigne l’interface radio  du réseau de  communications sans fil (couche  physique), alors que l’appellation  LoRaWAN, parfois désignée  également par le terme  LoRa MAC  désigne le protocole de communication.
 

 - **APRS** est un protocole de couche application qui définit des trames avec des adresses source et destination (appellé aussi indicatifs source et destination). Ce protocole définit entre autres les trames position weather.

![Schéma](/Aprs_via_LoRA/images/Aprs_via_LoRA.png)

## Matériels Utilisés

La solution repose sur l’utilisation de modules ttgo du constructeur  LILYGO®.

## Tracker 
**LILYGO® TTGO T-Beam V1.1**
WIFI & Bluetooth & LoRa & GPS
|  |  |
|--|--|
| ![Schéma](/Aprs_via_LoRA/images/LILYGO_TTGO_T_Beam.png)  |  ESP32 - Écran OLED de 0.96 pouces (résolution : 128x64)  - GPS avec une led indicant la réception - Puissance maximum en émission : + 20dBm Mode de modulation :  LoRa  |

## Gateway 
**LILYGO® TTGO ESP32-LoRa32 T3_V1.6** 
|  |  |
|--|--|
| ![Schéma](/Aprs_via_LoRA/images/LILYGO_TTGO_ESP32-LoRa32.png)  | frequence: **433/470MHz**(433HMz version) puissance d'émission: +20dBm sensibilité :-139dBm@LoRa &62.5Khz&SF=12&146bps |

## Installation du driver CH9102 pour ttgo

Sur les derniers modèles la puce USB to UART CP2104 est remplacée par CH9102. Pour windows, l'installation du driver CH9102  est disponible sur le site de [lilyGO](https://github.com/Xinyuan-LilyGO/CH9102_Driver). 
après avoir lancé l'exécution du fichier  CH9102_WIN.EXE  vous pouvez connecter votre TTGO ESP32-LoRa32. 
Ouvrir le gestionnaire de périphériques pour constater le bon fonctionnement du driver.

![Driver](/Aprs_via_LoRA/images/Driver_for_windows.png)


## Installation des logiciels

Les logiciels compilés sont disponibles sur F4G0H. L'installation peut être directement être faite à partir du navigateur ***Google Chrome*** ou ***Microsoft Edge***.
Automatic installation [firmwares](https://f4goh.github.io/lora-aprs-esp32/index.html)

 - Connectez le tracker à votre ordinateur à l'aide d'un câble USB.
 - Vous devez commencer par installer  le programme **Reset** GPS. Après le téléchargement, le  TTGO T-Beam va redémarrer automatiquement.
 - Ouvrir  Putty, configurer la vitesse sur **115200** et serial line   sur **/dev/ttyACM0**
 - Laisser le programme fonctionner jusqu'à obtenir des trames de position NMEA sur  l'écran.
 - Vous pouvez maintenant installer le logiciel **tracker**
 - Ouvrir putty juste après le démarrage du programme appuyer sur la touche m pour entrer dans le menu de configuration. Saisir **help** pour obtenir les commande disponibles.
 - Configurer l'identifiant du tracker.
 - Pour quitter la configuration taper **exit** 
 - Même procédure pour l'installation du programme Igate Wifi

![Logo_Touchard](/Aprs_via_LoRA/images/logo_touchard.png)

