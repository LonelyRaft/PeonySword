#ifndef PEONYSWORD_EVENT_H
#define PEONYSWORD_EVENT_H

#include <chrono>

namespace PeonySword {

    enum KeyValue : unsigned char {
        KeyValueInvalid,
        KeyValueEscape,
        KeyValueInsert,
        KeyValueDelete,
        KeyValueHome,
        KeyValueEnd,
        KeyValuePgUp,
        KeyValuePgDown,
        KeyValueUp,
        KeyValueDown,
        KeyValueLeft,
        KeyValueRight,
        KeyValueBack,
        KeyValueTab,
        KeyValueCaps,
        KeyValueEnter,
        KeyValueLeftShift,
        KeyValueRightShift,
        KeyValueLeftCtrl,
        KeyValueRightCtrl,
        KeyValueSpace,
        KeyValueQuote,
        KeyValueComma,
        KeyValueSub,
        KeyValuePeriod,
        KeyValueSlash,
        KeyValue0,
        KeyValue1,
        KeyValue2,
        KeyValue3,
        KeyValue4,
        KeyValue5,
        KeyValue6,
        KeyValue7,
        KeyValue8,
        KeyValue9,
        KeyValueSemiconlon,
        KeyValueEqual,
        KeyValueA,
        KeyValueB,
        KeyValueC,
        KeyValueD,
        KeyValueE,
        KeyValueF,
        KeyValueG,
        KeyValueH,
        KeyValueI,
        KeyValueJ,
        KeyValueK,
        KeyValueL,
        KeyValueM,
        KeyValueN,
        KeyValueO,
        KeyValueP,
        KeyValueQ,
        KeyValueR,
        KeyValueS,
        KeyValueT,
        KeyValueU,
        KeyValueV,
        KeyValueW,
        KeyValueX,
        KeyValueY,
        KeyValueZ,
        KeyValueLeftBracket,
        KeyValueBackslash,
        KeyValueRightBracket,
        KeyValueTilde,
        KeyValueF1,
        KeyValueF2,
        KeyValueF3,
        KeyValueF4,
        KeyValueF5,
        KeyValueF6,
        KeyValueF7,
        KeyValueF8,
        KeyValueF9,
        KeyValueF10,
        KeyValueF11,
        KeyValueF12,
        KeyValuePrtScreen,
        KeyValueScrollLock,
        KeyValuePause,
        KeyValueMenu,
        KeyValueLeftAlt,
        KeyValueRightAlt,
        KeyValueStart,
        KeyValueUpperLimit = 0x80
    };
    enum KeyState : unsigned char {
        KeyStateInvalid,
        KeyStateDown,
        KeyStateUp,
    };

    // convert system key code to KeyValue code
    KeyValue toKeyValue(int _key);

    class KeyEvent {
    public:
        KeyEvent() = default;

        std::chrono::milliseconds mStamp{};
        KeyValue mKeyValue = KeyValueInvalid;
        KeyState mKeyState = KeyStateInvalid;

        bool operator==(const KeyEvent &) const;

        bool operator!=(const KeyEvent &) const;
    };
}


#endif //PEONYSWORD_EVENT_H
