<?php

/*
 aide serveur APRS
 http://www.aprs-is.net/Connecting.aspx
 utilisation des filtres (description plus simple)
 http://www.aprs-is.net/javAPRSFilter.aspx  
 * 
 exemple d'affichage dans la console


 */

function receptionPositionMobile($callMobile) {
    $crlf="\r\n";
    date_default_timezone_set('Europe/Paris');

    echo "Connexion TCP/IP sur serveur APRS".$crlf;    

    $CALLSIGN = "F4KMN-2";
    $PASSCODE = "12443";
    $LATITUDE = "4759.69N";
    $LONGITUDE = "00012.27E";
    $PHG = "PHG1.30/ 144MHz antenna";
    $INFO = "PHP AirGate IGATE";
    $DESTINATION = "F4KMN";    
    $service_port = 14580;
        
    
    $loginMdp = "user " . $CALLSIGN . " pass " . $PASSCODE . " vers " . $INFO . " filter b/".$callMobile."\n";
    $stationInfo = $CALLSIGN . ">" . $DESTINATION . ":!" . $LATITUDE . "I" . $LONGITUDE . "&" . $PHG . "/" . $INFO . "\n";
 
    // adresse IP du serveur APRS
    $address = gethostbyname('euro.aprs2.net');
    echo "address IP du serveur euro.aprs2.net => " . $address.$crlf;
    $succes = true;

    $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP); //Ouvre un socket en IPV4, TCP 
    socket_set_option($socket,SOL_SOCKET, SO_RCVTIMEO, array("sec"=>1, "usec"=>0));        // time out 1s pour socket_read

    if ($socket === false) {
        echo "socket_create() a échoué : raison :  " . socket_strerror(socket_last_error()).$crlf;
        $succes = false;
    } else {
        $result = socket_connect($socket, $address, $service_port);
        if ($socket === false) {
            echo "socket_connect() a échoué : raison : ($result) " . socket_strerror(socket_last_error($socket)).$crlf;
            $succes = false;
        } else {
            echo "Envoi de la commande login/mdp : ".$crlf;
            $retour = socket_write($socket, $loginMdp, strlen($loginMdp));
            if ($retour === false) {
                echo "erreur login mdp".$crlf;
                $succes = false;
            }
            echo "Envoi de la commande station Info : " . $stationInfo.$crlf;
            $retour = socket_write($socket, $stationInfo, strlen($stationInfo));
            if ($retour === false) {
                echo "erreur station info".$crlf;
                $succes = false;
            }
            if ($succes){

                echo "Attente de reception de la position du mobile " . $callMobile . $crlf;                       
                echo "Pour quitter tapez q puis entrée " . $crlf;

                $clavier = "";
               do {
                    
                    if ($pduAPRS = socket_read($socket, 256, PHP_BINARY_READ )) {                      
                        if (substr($pduAPRS, 0,strlen($callMobile)) == $callMobile){
                            
                            echo $pduAPRS;                        // affichage de la trame brute                     
                            echo date('d-m-y H:i:s') . "\t";      // affichage date et heure
                            $position = parsePosition($pduAPRS);
                            echo ("Latitude  : ". $position[0] . "\t");
                            echo ("longitude : ". $position[1] . "\r\n\r\n");
                        }                    
                    }
                    $clavier = lireClavier();  // appel de la fonction non bloquante                  
                }
                while ($clavier !== "q");
                
            
               socket_close($socket);
               echo "Socket fermée...Bye".$crlf;
            }
        }
    }
    return $succes;
}


function parsePosition($trame)
{
    $pos = strpos($trame, ":"); 
    $temp = substr($trame, $pos+2,18);
    $position = explode("/", $temp);
    return $position;

}


// Fonction non bloquante) pour lire un caractère au clavier (stream STDIN)

function lireClavier() {

        $read = array(STDIN);
        $write = NULL;
        $except = NULL;

        if (false === ($nb = stream_select($read, $write, $except, 0))) {
            print("\$ 001 Socket Error : UNABLE TO WATCH STDIN.\n");
            return FALSE;
        } elseif ($nb > 0) {
                return fread(STDIN,1);
        }
           
    }
