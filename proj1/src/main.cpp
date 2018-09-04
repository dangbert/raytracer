#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "RayTracer.h"
#include "SettingsNFF.h"

using namespace std;

// my driver
int main() {
    RayTracer ray = RayTracer();

    SettingsNFF nff = SettingsNFF();
    cout << nff << endl;
    int status = nff.readFile("../tetra-3.nff");
    cout << "status = " << status << endl;
    cout << nff << endl;

    return 0;
}
