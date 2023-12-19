
#include "event.h"

namespace PeonySword {
    class KeyValuePair {
    public:
        unsigned char key = 0;
        KeyValue value = KeyValueInvalid;

        KeyValuePair() = default;
    };

#ifdef _WIN32
    static const KeyValuePair KeyValueTable[] = {
            {8,   KeyValueBack},
            {9,   KeyValueTab},
            {13,  KeyValueEnter},
            {19,  KeyValuePause},
            {20,  KeyValueCaps},
            {27,  KeyValueEscape},
            {32,  KeyValueSpace},
            {33,  KeyValuePgUp},
            {34,  KeyValuePgDown},
            {35,  KeyValueEnd},
            {36,  KeyValueHome},
            {37,  KeyValueLeft},
            {38,  KeyValueUp},
            {39,  KeyValueRight},
            {40,  KeyValueDown},
            {44,  KeyValuePrtScreen},
            {45,  KeyValueInsert},
            {46,  KeyValueDelete},
            {48,  KeyValue0},
            {49,  KeyValue1},
            {50,  KeyValue2},
            {51,  KeyValue3},
            {52,  KeyValue4},
            {53,  KeyValue5},
            {54,  KeyValue6},
            {55,  KeyValue7},
            {56,  KeyValue8},
            {57,  KeyValue9},
            {65,  KeyValueA},
            {66,  KeyValueB},
            {67,  KeyValueC},
            {68,  KeyValueD},
            {69,  KeyValueE},
            {70,  KeyValueF},
            {71,  KeyValueG},
            {72,  KeyValueH},
            {73,  KeyValueI},
            {74,  KeyValueJ},
            {75,  KeyValueK},
            {76,  KeyValueL},
            {77,  KeyValueM},
            {78,  KeyValueN},
            {79,  KeyValueO},
            {80,  KeyValueP},
            {81,  KeyValueQ},
            {82,  KeyValueR},
            {83,  KeyValueS},
            {84,  KeyValueT},
            {85,  KeyValueU},
            {86,  KeyValueV},
            {87,  KeyValueW},
            {88,  KeyValueX},
            {89,  KeyValueY},
            {90,  KeyValueZ},
            {91,  KeyValueStart},
            {93,  KeyValueMenu},
            {112, KeyValueF1},
            {113, KeyValueF2},
            {114, KeyValueF3},
            {115, KeyValueF4},
            {116, KeyValueF5},
            {117, KeyValueF6},
            {118, KeyValueF7},
            {119, KeyValueF8},
            {120, KeyValueF9},
            {121, KeyValueF10},
            {122, KeyValueF11},
            {123, KeyValueF12},
            {145, KeyValueScrollLock},
            {160, KeyValueLeftShift},
            {161, KeyValueRightShift},
            {162, KeyValueLeftCtrl},
            {163, KeyValueRightCtrl},
            {164, KeyValueLeftAlt},
            {165, KeyValueRightAlt},
            {186, KeyValueSemiconlon},
            {187, KeyValueEqual},
            {188, KeyValueComma},
            {189, KeyValueSub},
            {190, KeyValuePeriod},
            {191, KeyValueSlash},
            {192, KeyValueTilde},
            {219, KeyValueLeftBracket},
            {220, KeyValueBackslash},
            {221, KeyValueRightBracket},
            {222, KeyValueQuote},
    };
    static const int KeyValueTableSize =
            sizeof(KeyValueTable) / sizeof(KeyValuePair);
#endif // _WIN32

    KeyValue toKeyValue(int _key) {
        int start = 0;
        int end = KeyValueTableSize;
        while (start < end) {
            int idx = start + (end - start) / 2;
            const KeyValuePair *pair =
                    KeyValueTable + idx;
            if (pair->key > _key) {
                end = idx;
            } else if (pair->key < _key) {
                start = idx + 1;
            } else {
                return pair->value;
            }
        }
        return KeyValueInvalid;
    }

    bool KeyEvent::operator==(
            const KeyEvent &_event) const {
        return mKeyValue == _event.mKeyValue &&
               mKeyState == _event.mKeyState;
    }

    bool KeyEvent::operator!=(
            const KeyEvent &_event) const {
        return mKeyValue != _event.mKeyValue ||
               mKeyState != _event.mKeyState;
    }
}

