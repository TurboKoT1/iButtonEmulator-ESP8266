#include <OneWireHub.h>
#include <DS2401.h>
#include "config.h"
#include "Key.h"

static OneWireHub hub(iButtonPort);
static DS2401* ds1990A = nullptr;

void emulateKey(const OneWireKey& key) {
    if (ds1990A != nullptr) {
        hub.detach(*ds1990A);
        delete ds1990A;
        ds1990A = nullptr;
    }
    ds1990A = new DS2401(
        key.data[0], key.data[1], key.data[2], key.data[3],
        key.data[4], key.data[5], key.data[6]
    );
    hub.attach(*ds1990A);
}

void stopEmulation() {
    if (ds1990A != nullptr) {
        hub.detach(*ds1990A);
        delete ds1990A;
        ds1990A = nullptr;
    }
}

void pollEmulator() {
    hub.poll();
}