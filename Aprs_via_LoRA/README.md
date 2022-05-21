# Tracker Aprs via LoRa

## Objectifs
Le projet est de réaliser un **traqueur APRS** (TX UHF) ainsi qu'une **passerelle** (RX) qui va renvoyer les informations reçues sur le serveur aprs.fi. 
Les objectifs  sont, entre autres, une faible consommation d’énergie et une longue portée  entre le tracker et la passerelle (15 km environ).

## Principe
Le principe  est de transmettre les données de position par liaison radio  à faible puissance d’émission,  fonctionnant en autonomie sur une petite batterie au format 18650. 
Les trackers  envoient leurs trames à une (ou plusieurs) **gateway**(s)  qui fait office de relais entre les trackers qui sont dans le rayon de couverture et le serveur de stockage et de traitement de données  aprs.fi.

 - **LoRa** est une technologie de modulation du signal radio qui permet la transmission de données. (Couche physique 1 du modèle OSI).
 - **APRS** est un protocole de couche liaison de données qui définit des trames avec des adresses source et destination (appellé aussi indicatifs source et destination). 
 - **APRS** est aussi un protocole de couche application. Ce protocole définit entre autres les trames position weather.

![Schéma](/Aprs_via_LoRA/images/Aprs_via_LoRA.png)

## Matériels Utilisés

La solution repose sur l’utilisation de modules ttgo du constructeur  LILYGO®.

## LILYGO® TTGO T-Beam V1.1

![Schéma](/Aprs_via_LoRA/images/LILYGO_TTGO_T_Beam.png)
 - ESP32
 - Écran OLED de 0.96 pouces (résolution : 128x64)
 - GPS avec une led indicant la réception
 - Puissance maximum en émission : + 20dBm Mode de modulation :  **LoRa**  

