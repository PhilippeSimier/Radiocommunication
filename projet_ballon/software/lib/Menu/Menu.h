/* 
 * File:   Menu.h
 * Author: philippe SIMIER Touchard Washington Le Mans
 *
 * Created on 22 avril 2022, 17:11
 */

#ifndef MENU_H
#define MENU_H
#include <Arduino.h>
#include <console.h>
#include <Preferences.h>        // Classe pour écrire et lire la configuration

class Menu {
    
public:
    Menu();
    Menu(const Menu& orig);
    virtual ~Menu();
    
    void run();
    void setup();
    
    // Méthodes associées aux commandes 
    static void _exit_(ArgList& L, Stream& S);
    static void _help_(ArgList& L, Stream& S);
    static void _unknown(String& L, Stream& S);
    static void _source_(ArgList& L, Stream& S);
    static void _dest_(ArgList& L, Stream& S);
    static void _path1_(ArgList& L, Stream& S);
    static void _path2_(ArgList& L, Stream& S);
    static void _frequence_(ArgList& L, Stream& S);
    static void _charge_(ArgList& L, Stream& S);
    static void _fec_(ArgList& L, Stream& S);
    static void _raz_(ArgList& L, Stream& S);
    static void _config_(ArgList& L, Stream& S);
    

private:
    
    bool exitFlag;
    Console *con;
    Preferences *configuration;
    static Menu* anchor;
    bool acceptCmd(String cmd, int longMin, int longMax);
    
};

#endif /* MENU_H */

