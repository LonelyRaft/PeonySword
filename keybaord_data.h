#ifndef PEONYSWORD_KEYBAORD_DATA_H
#define PEONYSWORD_KEYBAORD_DATA_H

namespace PeonySword {
    class KeyData {
    public:
        bool mKeepEnable = true;
        bool mKeeping = false;
    };

    class KeyValuePair {
    public:
        unsigned char key = 0;
        KeyValue value = KeyValueInvalid;

        KeyValuePair() = default;
    };
}

#endif //PEONYSWORD_KEYBAORD_DATA_H
