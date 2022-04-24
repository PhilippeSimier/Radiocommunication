# Ballon sonde stratosphérique

## Objectifs pédagogiques
La  construction d'une nacelle expérimentale de ballon doit permettre aux jeunes, une découverte des sciences expérimentales. Les expériences  mettent en oeuvre la mesure de grandeurs physiques avec la transmission des données par radiofréquence.  Une initiation à la démarche de projet est aussi proposée.

## Introduction
 L'objectif  est de faire de la science afin de pouvoir mieux comprendre notre atmosphère jusqu’à 30 km d’altitude. Nous tenterons de répondre à la question suivante :

 - Dans quelle mesure l'atmosphère nous protège t-elle du [rayonnement cosmiques](https://www.sievert-system.org/#Rayonnement)   ?

Le rayonnement cosmique est formé de particules qui se déplacent à une vitesse proche de celle de la lumière. Il est constitué de deux composantes, l’une stable, d’origine galactique, l’autre plus fluctuante, liée à l’activité du soleil.
Pour le comprendre, commençons par l'espace lointain. Les rayons cosmiques sont essentiellement des débris subatomiques d'étoiles mourantes, accélérés à une vitesse proche de la lumière par des explosions de supernova. Ils voyagent à travers l'espace et s'approchent de la Terre
dans toutes les directions, bombardant notre planète 24h/24 et 7j/7.
Lorsque les rayons cosmiques s'écrasent dans l'atmosphère terrestre, ils produisent un jet de particules secondaires et de photons qui est le plus intense à l'entrée de la stratosphère. Les physiciens Eric Regener et Georg Pfotzer ont découvert ce maximum en utilisant des ballons dans les années 1930 et c'est ce que nous allons vérifier aujourd'hui.

|Altitude (m)|  Débit de dose (μSv/h)|
|--|--|
|80  | 0.08 |
|5000  | 1.8 |
|8200  | 4.1 |
|10000  | 5.2 |
|11200  | 7.7 |

 [Unité de mesures](https://www.sievert-system.org/#Mesure)

Lors du vol, le ballon traverse une couche appelée le Regener-Pfotzer Maximum, une couche de rayonnement maximal à environ 20 km au-dessus de la surface de la Terre. Ce graphique de rayonnement en fonction du temps pris lors d'un vol en ballon en juillet 2015 illustre le pic :

![Ionizing_Radiation](/projet_ballon/images/Ionizing_Radiation.png)

## Principe de la mesure rayonnement ionisant

Le rayonnement est composé de photons gamma et de particules capables de provoquer, directement ou indirectement, la formation d'ions (par arrachement d'électrons) dans la matière qu’ils traversent.

La détection du rayonnement ionisant repose  sur un type particulier de diode, la **diode PIN**. PIN pour Positive Intrisic Negative fait référence directement à la manière dont est constitué le composant.  Dans  une diode PIN on trouve entre les zones P et N une zone non dopée dite intrinsèque.  
La structure PIN permet la détection de photons, un peu à la manière d'un tube Geiger-Muller avec un effet cascade et une brève conductivité. Cela signifie que si un rayon de haute énergie est absorbé, un signal détectable d'électrons est généré. l'absorption photoélectrique est dominante dans la région d'énergie gamma d'intérêt entre 5 keV et 100 keV.
Les détecteurs basés sur **l'absorption directe dans le silicium** constituent une alternative  peu coûteuse par rapport au détecteur à gaz (tube Geiger-Muller).
le capteur utilisé possède une photodiode PIN **[X100-7](https://www.first-sensor.com/cms/upload/datasheets/X100-7_THD_5000040.pdf)** pour la détection des rayons ionisants, ayant une surface de section efficace à grande échelle (100 mm ^ 2), fabriquée par la société **FirstSensor** qui a également fourni un capteur similaire pour le télescope à rayons X de la NASA. En règle générale, l'efficacité du capteur à photodiode PIN est extrêmement stable et ne présente aucune dégradation liée à l'âge.
La détection et l’électronique qui l'entoure est là pour amplifier le signal et le transmettre avec des niveaux adaptés vers un entrée digital de l'ESP32. On obtient le débit de dose par comptage des impulsions sur une periode donnée.

### La Nacelle
Pendant le vol, les capteurs seront intégrés dans la Nacelle  qui assurera les fonctions de plate-forme : intégration mécanique, isolation et contrôle thermique, localisation/datation des observations, fourniture de l’énergie grâce à des panneaux solaires et batteries.
La nacelle devra être capable de fonctionner de manière autonome pendant 4 heures, en environnement stratosphérique (pression 50-70 hPa, température environ -80°C).
Les contraintes de masse embarquée sous ballons pressurisés sont telles qu’il est impossible de faire voler une nacelle de plus de 1,8 kg.

### Mécanique 
(design, calculs, atelier, réalisation, intégration) : 
Cette tâche consiste en la conception et la réalisation des coques externes de la  nacelle, ainsi que la structure interne permettant l’intégration des capteurs et des différents sous-systèmes. La structure de la nacelle est réalisée en polystyrène expansé de 30mm d'épaisseur.
La nacelle est recouverte par collage d'une couverture de survie. La colle utilisée est une colle en spray **3M PhotoMount**.

![Nacelle](/projet_ballon/images/Nacelle_construction.jpg)

### Les cartes de télémesures et d'alimentation

L’ensemble des cartes électroniques ont le même format et sont fixées au couvercle. Elles sont ainsi facilement retirables de la nacelle. Les cartes sont empilées  et sont maintenues séparées les unes des autres au moyen de 4 entretoises hexagonales disposées dans les coins. 

![Carte de télémesures](/projet_ballon/images/carte_télémesures_2.jpg)

Tout  au  long  du  vol,  les  mesures  sont  enregistrées à bord sur une carte SD. elles  sont aussi envoyées au sol par l'émetteur embarqué.


![Carte de télémesures](/projet_ballon/images/carte_télémesures.jpg)

### Le suivi de la trajectoire de vol 

Afin de faciliter la récupération de la nacelle, le système de télémesures est équipé d’un récepteur GPS pouvant fonctionner en haute altitude.
le suivi du vol est effectué par transmission de la position dans des trames **APRS** encapsulé dans des trames **FX25**. la position du ballon peut être suivie sur le site https://aprs.fi/

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
 
 

 

