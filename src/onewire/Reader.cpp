#include <OneWire.h>
#include "config.h"
#include "Key.h"

OneWire oneWire(iButtonPort);

bool readKey(OneWireKey& key) {
    oneWire.reset();
    if (!oneWire.search(key.data)) {
        oneWire.reset_search();
        return false;
    }
    oneWire.reset_search();

    return true;
}