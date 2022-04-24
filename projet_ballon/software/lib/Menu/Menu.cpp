/* 
 * File:   Menu.cpp
 * Author: philippe SIMIER Touchard Washington le Mans
 * 
 * Created on 22 avril 2022, 17:11
 */

#include "Menu.h"

Menu::Menu() :
exitFlag(false),
con(new Console()),
configuration(new Preferences()) {
    anchor = this;
}

Menu::Menu(const Menu& orig) {
}

Menu::~Menu() {

    delete configuration;
    anchor = NULL;
}

void Menu::run() {

    char car;
    int cpt = 0;
    exitFlag = false;

    while ((cpt < 10) && !exitFlag) {

        Serial.print(".");
        cpt++;

        if (Serial.available() > 0) {
            car = Serial.read();
            if (car == 'm') {
                Serial.println(F("help command for info"));
                while (exitFlag == false) {
                    con->run();
                }
            }
        }
        delay(1000);
    }
    Serial.println(F("\nConsole exit"));
}

void Menu::setup() {
    con->onCmd("help", _help_);
    con->onCmd("exit", _exit_);
    con->onCmd("source", _source_);
    con->onCmd("dest", _dest_);
    con->onCmd("path1", _path1_);
    con->onCmd("path2", _path2_);
    con->onCmd("freq", _frequence_);
    con->onCmd("show", _config_);
    con->onCmd("charge", _charge_);
    con->onCmd("fec", _fec_);
    con->onCmd("raz", _raz_);
    con->onUnknown(_unknown);
    con->start();

    anchor->configuration->begin("radio", false);
    if (anchor->configuration->getBool("config", false) == false) {
        anchor->configuration->putString("srcCallsign", "F4KMN-13");
        anchor->configuration->putString("dstCallsign", "F4KMN-1");
        anchor->configuration->putString("path1", "WIDE1-1");
        anchor->configuration->putString("path2", "WIDE2-2");
        anchor->configuration->putFloat("frequence", 144.800);
        anchor->configuration->putBool("fec", false);
        anchor->configuration->putBool("config", true);
    }
    anchor->configuration->end();
    this->run();
}

void Menu::_exit_(ArgList& L, Stream& S) {
    anchor->exitFlag = true;

}

void Menu::_help_(ArgList& L, Stream& S) {
    S.println(F("Commandes disponibles :"));
    S.println(F("Set  Source callsign : source f4kmn-13"));
    S.println(F("Set  Dest callsign   : dest f4kmn-1"));
    S.println(F("Set  Path1           : path1 WIDE1-1"));
    S.println(F("Set  Path2           : path2 WIDE2-2"));
    S.println(F("Set  Frequence       : freq 144.800"));
    S.println(F("Set  Charge (mAh)    : charge 2000"));
    S.println(F("Enable/disable FEC   : fec 1"));
    S.println(F("Show configuration   : show"));
    S.println(F("RAZ                  : raz"));
    S.println(F("Exit menu            : exit"));
}

bool Menu::acceptCmd(String cmd, int longMin, int longMax) {

    if (cmd.length() >= longMin && cmd.length() <= longMax) {
        return true;
    } else {
        return false;
    }
}

void Menu::_unknown(String& L, Stream& S) {

    S.print(L);
    S.println(" : commande introuvable");
}

void Menu::_source_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 3, 10)) {

        anchor->configuration->begin("radio", false);
        anchor->configuration->putString("srcCallsign", p);
        S.printf("Source Callsign is %s\n\r", anchor->configuration->getString("srcCallsign").c_str());
        anchor->configuration->end();
    } else {
        S.println("Erreur");
    }
}

void Menu::_dest_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 3, 10)) {
        anchor->configuration->begin("radio", false);
        anchor->configuration->putString("dstCallsign", p);
        S.printf("Dest Callsign is %s\n\r", anchor->configuration->getString("dstCallsign").c_str());
        anchor->configuration->end();
    } else {
        S.println("Erreur");
    }
}

void Menu::_path1_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 3, 10)) {
        anchor->configuration->begin("radio", false);
        anchor->configuration->putString("path1", p);
        S.printf("Path1 is %s\n\r", anchor->configuration->getString("path1").c_str());
        anchor->configuration->end();
    } else {
        S.println("Erreur");
    }
}

void Menu::_path2_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 3, 10)) {

        anchor->configuration->begin("radio", false);
        anchor->configuration->putString("path2", p);
        S.printf("Path2 is %s\n\r", anchor->configuration->getString("path2").c_str());
        anchor->configuration->end();
    } else {
        S.println("Erreur");
    }
}

void Menu::_config_(ArgList& L, Stream& S) {
    anchor->configuration->begin("radio", false);
    S.printf("Source Callsign : %s\n\r", anchor->configuration->getString("srcCallsign").c_str());
    S.printf("Dest   Callsign : %s\n\r", anchor->configuration->getString("dstCallsign").c_str());
    S.printf("Path1           : %s\n\r", anchor->configuration->getString("path1").c_str());
    S.printf("Path2           : %s\n\r", anchor->configuration->getString("path2").c_str());
    S.printf("Frequence (MHz) : %.3f\n\r", anchor->configuration->getFloat("frequence"));
    S.printf("FEC             : %s\n\r", anchor->configuration->getBool("fec") ? "Enable" : "Disable");
    anchor->configuration->end();
    anchor->configuration->begin("battery", false);
    S.printf("Charge (mAh)    : %.1f\n\r", anchor->configuration->getFloat("charge"));
    anchor->configuration->end();
}

void Menu::_frequence_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 7, 7)) {

        anchor->configuration->begin("radio", false);
        anchor->configuration->putFloat("frequence", p.toFloat());
        S.printf("Frequence is %.3f Mhz\n\r", anchor->configuration->getFloat("frequence"));
        anchor->configuration->end();
    } else {
        S.println("Erreur frequence");
    }
}

void Menu::_charge_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 2, 10)) {

        anchor->configuration->begin("battery", false);
        anchor->configuration->putFloat("charge", p.toFloat());
        S.printf("Charge is %.1f mAh\n\r", anchor->configuration->getFloat("charge"));
        anchor->configuration->end();
    } else {
        S.println("Erreur charge batterie");
    }
}

void Menu::_fec_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 1, 1)) {

        anchor->configuration->begin("radio", false);
        anchor->configuration->putBool("fec", (int8_t) p.toInt());
        S.printf("FEC is : %s\n\r", anchor->configuration->getBool("fec") ? "Enable" : "Disable");
        anchor->configuration->end();
    } else {
        S.println("Erreur fec");
    }
}

void Menu::_raz_(ArgList& L, Stream& S) {
    anchor->configuration->begin("radio", false);
    anchor->configuration->putBool("config", false);
    anchor->configuration->clear();
    ESP.restart();
}

Menu* Menu::anchor = NULL;
