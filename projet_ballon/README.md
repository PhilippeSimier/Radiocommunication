# Ballon sonde 

## Objectif pédagogique
La  construction d'une nacelle expérimentale de ballon doit permettre aux jeunes, une découverte des sciences et des  
techniques de mesures et transmission ainsi qu'une initiation à la démarche de projet.

## Introduction
Le ballon embarque une nacelle de moins de 1,8 kg qui contient un système de télémesures. L'objectif  est de faire de la science afin de pouvoir mieux comprendre notre atmosphère jusqu’à 30 km d’altitude.

### La carte de télémesures
![Carte de télémesures](/projet_ballon/images/carte_télémesures.jpg)
Un système de stockage des données à bord (sur une carte SD) est présent.

### Le suivi de la trajectoire de vol 
Afin de faciliter la récupération de la nacelle, le système de télémesures est équipé d’un récepteur GPS pouvant fonctionner en haute haltitude.
le suivi du vol est effectué par transmission de trame position APRS encapsulé dans des trames FX25. la position du ballon est visible sur le site aprs.fi
![aprs.fi](/projet_ballon/images/Capture_aprs.fi.png)


## Principe
Le ballon s’élève dans l’atmosphère grace à la **poussée d’Archimède**. Plus il monte, plus la pression de l’air autour de lui, diminue. L’hélium occupe alors de plus en plus de volume : le ballon se dilate et finit par éclater.

A ce moment, la chaîne de vol redescend vers la terre. Le parachute inséré initialement «  en sapin  » entre l’enveloppe du ballon et le réflecteur radar, joue alors son rôle de frein.

La durée totale du vol se situe entre 2 et 4 heures. **Planète Sciences** se charge de la demande d’autorisation de vol auprès des autorités. 


## Les contraintes

 1. L'alimentation de la chaîne de mesure doit posséder une autonomie supérieure à 3 heures.
 2. La chaîne de mesure doit être étalonnée dans son intégralité     (système au sol compris) avant le vol.
 3. Le système de transmission utilise un transmetteur radio, la fréquence d'émission est accordée sur 144800 Mhz. l’Equipe dispose d’une station de réception adaptée. Le système est composé d'une clé SDR connecté à un Raspberry pi. Le logiciel DireWolf décode les trames recues et les retransmet sur un des serveurs aprs.  Sur la nacelle, une antenne du type "Ground plane" rayonne. Le brin central est vertical et orienté vers le bas.

 

