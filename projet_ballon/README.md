﻿# Ballon sonde stratosphérique

## Objectifs pédagogiques
La  construction d'une nacelle expérimentale de ballon doit permettre aux jeunes, une découverte des sciences et des  
techniques qui comprend la mesure de grandeurs physiques et la transmission des données par radiofréquence.  Une initiation à la démarche de projet est aussi proposée.

## Introduction
 L'objectif  est de faire de la science afin de pouvoir mieux comprendre notre atmosphère jusqu’à 30 km d’altitude. Nous tenterons de répondre à la question suivante :

 - Dans quelle mesure l'atmosphère nous protège t'elle des rayons cosmiques  gamma ?

Pour ce faire Le ballon embarque une nacelle de moins de 1,8 kg qui contient le système de télémesures avec des capteurs pour mesurer rayonnement gamma, la température extérieure et intérieure, la pression et l'humidité.

### La carte de télémesures

![Carte de télémesures](/projet_ballon/images/carte_télémesures.jpg)

Tout  au  long  du  vol,  les  mesures  sont  enregistrées à bord et aussi envoyées au sol par l'émetteur embarqué.
Le système de stockage des données à bord utilise une carte SD.

### Le suivi de la trajectoire de vol 

Afin de faciliter la récupération de la nacelle, le système de télémesures est équipé d’un récepteur GPS pouvant fonctionner en haute altitude.
le suivi du vol est effectué par transmission de trame position **APRS** encapsulé dans des trames **FX25**. la position du ballon peut être suivie sur le site https://aprs.fi/

![aprs.fi](/projet_ballon/images/Capture_aprs.fi.png)

ou sur le site https://aprs-map.info/

![aprs-map.info](/projet_ballon/images/Capture_aprs-map.info.png)

## Principe
Le ballon s’élève dans l’atmosphère grâce à la **poussée d’Archimède**. Plus il monte, plus la pression de l’air autour de lui, diminue. L’hélium occupe alors de plus en plus de volume : le ballon se dilate et finit par éclater.

A ce moment, la chaîne de vol redescend vers la terre. Le parachute inséré initialement «  en sapin  » entre l’enveloppe du ballon et le réflecteur radar, joue alors son rôle de frein.

La durée totale du vol se situe entre 2 et 4 heures. **Planète Sciences** se charge de la demande d’autorisation de vol auprès des autorités. 


## Les contraintes

 1. La chaîne de mesure doit être étalonnée dans son intégralité     (système au sol compris) avant le vol.

 2. L'alimentation de la chaîne de mesure doit posséder une autonomie supérieure à 4 heures.

 3. Le système utilise un transmetteur radio, la fréquence d'émission est accordée sur 144800 Mhz. l’Equipe du projet dispose d’une station de réception au sol adaptée. Le système est composé d'une clé SDR connecté à un Raspberry pi. Le logiciel DireWolf décode les trames reçues et les retransmet sur un des serveurs aprs.  Sur la nacelle, une antenne du type "Ground plane" rayonne. Le brin central est vertical et orienté vers le bas.
 
 

 
