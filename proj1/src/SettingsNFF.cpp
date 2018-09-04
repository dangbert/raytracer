#include "SettingsNFF.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <iterator>
#include <assert.h>
using namespace std;

SettingsNFF::SettingsNFF() {
    reset();
}

// reset to default settings
void SettingsNFF::reset() {
    // default background color is 0,0,0
    for (int i=0; i<3; i++) {
        bColor[i] = i;
    }
}

/**
 * You should be able to read any NFF format file, but ignore anything you do not
 * implement. For the basic assignment, you should at least handle the "v" viewing
 * specification, "b" background color, "f" object material specification (just
 * the r g b color part, ignore the rest), and "p" polygon specification. "c" and
 * "pp" are all multi-line, so you will at least need to recognize enough of those
 * to know how much to skip
 */
int SettingsNFF::readFile(std::string fname) {
    reset(); // reset to default settings
    std::string line;
    ifstream f(fname);

    while (std::getline(f, line)) {
        // split line on space delimeter to vector of strings
        std::istringstream iss(line);
        std::istream_iterator<std::string> begin(iss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> tokens(begin, end);

        cout << line << "\tlength=" << tokens.size() << endl;

        if (tokens.at(0) == "b") {
            std::istringstream iss(line);       // start string stream over
            std::string tmp;
            float r, g, b;
            if (!(iss >> tmp >> r >> g >> b)) { return 1; } // error
            cout << "read to (" << r << ", " << g << ", " << b << ")" << endl;
            bColor[0] = r;
            bColor[1] = g;
            bColor[2] = b;

        }
        return 2; // for now

    }
    return 0; // success
}

// print out the NFF settings stored in this object for debugging
std::ostream& operator<<(std::ostream &sout, const SettingsNFF &nff) {
    sout << "\n---NFF Settings---\n";
    sout << "background color: ";
    for (int i=0; i<3; i++) {
        sout << nff.bColor[i] << " ";
    }
    sout << endl;

    sout << "------------------\n";
    return sout;
}
