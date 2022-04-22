/* 
 * File:   Menu.cpp
 * Author: philippe SIMIER
 * 
 * Created on 22 avril 2022, 17:11
 */

#include "Menu.h"

Menu::Menu() {
    con = new Console();
    configuration = new Preferences();
    anchor = this;

}

Menu::Menu(const Menu& orig) {
}

Menu::~Menu() {
    //delete con;
}

void Menu::run() {

    char car;
    int cpt = 0;
    exitFlag = false;

    while ((cpt < 10) && !exitFlag) {
        if (Serial.available() > 0) {
            car = Serial.read();
            if (car == 'm') {
                Serial.println(F("help command for info"));
                while (exitFlag == false) {
                    con->run();
                }
            }
        }
        Serial.print(".");
        delay(1000);
        cpt++;
    }
    Serial.println("\nConsole exit");
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
    con->onUnknown(_unknown);
    con->start();
}

void Menu::_exit_(ArgList& L, Stream& S) {
    anchor->exitFlag = true;

}

void Menu::_help_(ArgList& L, Stream& S) {
    Serial.println(F("Commandes disponibles :"));
    Serial.println(F("Set  Source callsign : source f4kmn-13"));
    Serial.println(F("Set  Dest callsign   : dest f4kmn-1"));
    Serial.println(F("Set  Path1           : path1 WIDE1-1"));
    Serial.println(F("Set  Path2           : path2 WIDE2-2"));
    Serial.println(F("Set  Frequence       : freq 144.800"));
    Serial.println(F("Show configuration   : show"));
    Serial.println(F("Exit menu            : exit"));
}

bool Menu::acceptCmd(String cmd, int longMin, int longMax) {

    if (cmd.length() >= longMin && cmd.length() <= longMax) {
        return true;
    } else {
        Serial.println(F("erreur"));
        return false;
    }
}

void Menu::_unknown(String& L, Stream& S) {
    S.print("? ");
    S.println(L);
}

void Menu::_source_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 3, 10)) {
        S.printf("Source Callsign is %s\n\r", p.c_str());
        anchor->configuration->begin("radio", false);
        anchor->configuration->putString("srcCallsign", p);
        anchor->configuration->end();
    }
}

void Menu::_dest_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 3, 10)) {
        S.printf("Dest Callsign is %s\n\r", p.c_str());
        anchor->configuration->begin("radio", false);
        anchor->configuration->putString("dstCallsign", p);
        anchor->configuration->end();
    }
}

void Menu::_path1_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 3, 10)) {
        S.printf("Path1 is %s\n\r", p.c_str());
        anchor->configuration->begin("radio", false);
        anchor->configuration->putString("path1", p);
        anchor->configuration->end();
    }
}

void Menu::_path2_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 3, 10)) {
        S.printf("Path2 is %s\n\r", p.c_str());
        anchor->configuration->begin("radio", false);
        anchor->configuration->putString("path2", p);
        anchor->configuration->end();
    }
}

void Menu::_config_(ArgList& L, Stream& S) {
    anchor->configuration->begin("radio", false);
    Serial.printf("Source Callsign : %s\n\r", anchor->configuration->getString("srcCallsign").c_str());
    Serial.printf("Dest   Callsign : %s\n\r", anchor->configuration->getString("dstCallsign").c_str());
    Serial.printf("Path1           : %s\n\r", anchor->configuration->getString("path1").c_str());
    Serial.printf("Path2           : %s\n\r", anchor->configuration->getString("path2").c_str());
    Serial.printf("Frequence       : %.3f\n\r", anchor->configuration->getFloat("frequence"));
    anchor->configuration->end();
}

void Menu::_frequence_(ArgList& L, Stream& S) {

    String p;
    p = L.getNextArg();

    if (anchor->acceptCmd(p, 7, 7)) {
        S.printf("Frequence is %.3f Mhz\n\r", p.toFloat());
        anchor->configuration->begin("radio", false);
        anchor->configuration->putFloat("frequence", p.toFloat());
        anchor->configuration->end();
    }else{
        S.println("Erreur frequence");
    }
}

Menu* Menu::anchor = NULL;
