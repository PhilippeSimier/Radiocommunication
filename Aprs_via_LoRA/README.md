# Aprs via Lora

## **Description du projet tracker Aprs via Lora**
Le projet est de réaliser un **traqueur APRS** (TX UHF) ainsi que une **passerelle** (RX) qui va renvoyer les informations reçues sur aprs.fi. 
les objectifs  sont entre autres une faible consommation d’énergie et une longue portée (15km) entre le tracker et la passerelle.

Le principe  est de transmettre des données de position par liaison hertzienne  à faible puissance d’émission,  fonctionnant sur une petite batterie au format 18650. 
Les trackers  envoient leurs trames à une (ou plusieurs) **gateway**(s) (ou concentrateur(s)) qui fait office de relais entre les trackers dans son rayon de couverture et le serveur de stockage et de traitement de données  aprs.fi.

La solution repose sur l’utilisation de modules TTGO.
![Schema](/Aprs_via_LoRA/images/Aprs_via_LoRA.png)

