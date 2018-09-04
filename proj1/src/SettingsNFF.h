#ifndef _SETTINGSNFF_H_
#define _SETTINGSNFF_H_
/* File: SettingsNFF.h
    Dan Engbert - UMBC CMSC435 F18 Project 1
*/
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

class SettingsNFF {
    public:
        SettingsNFF();
        int readFile(std::string fname);
        friend std::ostream &operator<<(std::ostream &sout, const SettingsNFF &nff);
        void reset();

    private:
        float bColor[3];
};
#endif
