/*
* Arduino sketch: USB Keyboard combiner.
* It combines the inputs from multiple keyboards, primarily to work around
* the way Mac computers do not allow the modifier keys of one keyboard to act
* on the keys of another keyboard.
* In particular, it allows you to use foot pedals as Shift/Alt/Command/Control modifiers
* for a keyboard.
*
* Runs on an Arduino that can act as a USB client (Leonardo or Due),
* with the USB Host Shield 2 from Circuits at Home.
* https://www.circuitsathome.com/products-page/arduino-shields
*
* Relies on the USB Host Shield 2 library:
* https://github.com/felis/USB_Host_Shield_2.0
*
* This sketch was adapted from the USBHIDBootKbd example in the USB Host Shield 2 library.
*
* For more details, see: https://github.com/davidbkemp/KeyboardCombinerArduinoSketch/wiki
*
* The code is released under the GNU General Public License.
*/

const boolean debug = false;

/*
* If using the Arduino Due, you will need the following include:
* #include <SPI.h>
*/

#include <hidboot.h>
#include <usbhub.h>
// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#define UHS_HID_BOOT_KEY_MAC_RETURN 0x28
#define UHS_HID_BOOT_KEY_MAC_ENTER  0x58
#define UHS_HID_BOOT_KEY_ESC        0x29
#define UHS_HID_BOOT_KEY_DEL        0x4C
#define UHS_HID_BOOT_KEY_BACKSPACE  0x2A
#define UHS_HID_BOOT_KEY_HOME       0x4A
#define UHS_HID_BOOT_KEY_PAGE_UP    0x4B
#define UHS_HID_BOOT_KEY_END        0x4D
#define UHS_HID_BOOT_KEY_PAGE_DN    0x4E
#define UHS_HID_BOOT_KEY_CLEAR      0x53
#define UHS_HID_BOOT_KEY_PAD_EQL    0x67
#define UHS_HID_BOOT_KEY_TAB        0x2B
#define UHS_HID_BOOT_KEY_CAPS       0x39

// variable definitions
typedef enum 
{
    qwerty=0, 
    tarmak1,
    tarmak2,
    tarmak3,
    tarmak4,
    colemak,
    dvorak,
    workman
} KeyboardLayout;

// Keymap based on the scancodes from 4 to 57, refer to the HID usage table on the meaning of each element
PROGMEM prog_uint8_t    qwertyKeymap[] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57};

//Tarmak 2012 E-T-O-I-R
//PROGMEM prog_uint8_t tarmak1Keymap[] = {4, 5, 6, 7, 13,  9, 10, 11, 12, 17, 8, 15, 16, 14, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 42};
//PROGMEM prog_uint8_t tarmak2Keymap[] = {4, 5, 6,  7,  9, 23, 13, 11, 12, 17,  8, 15, 16, 14, 18, 19, 20, 21, 22, 10, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 42};
//PROGMEM prog_uint8_t tarmak3Keymap[] = {4, 5, 6, 7,  9, 23, 51, 11, 12, 17, 8, 15, 16, 14, 28, 19, 20, 21, 22, 10, 24, 25, 26, 27, 13, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 18, 52, 53, 54, 55, 56, 42};
//PROGMEM prog_uint8_t tarmak4Keymap[] = {4, 5, 6, 7,  9, 23, 51, 11, 24, 17, 8, 12, 16, 14, 28, 19, 20, 21, 22, 10, 15, 25, 26, 27, 13, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 18, 52, 53, 54, 55, 56, 42};

//Tarmak 2014 E-T-R-O-I
PROGMEM prog_uint8_t tarmak1Keymap[] = {4, 5, 6,  7, 13,  9, 10, 11, 12, 17,  8, 15, 16, 14, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 42};
PROGMEM prog_uint8_t tarmak2Keymap[] = {4, 5, 6,  7,  9, 23, 13, 11, 12, 17,  8, 15, 16, 14, 18, 19, 20, 21, 22, 10, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 42};
PROGMEM prog_uint8_t tarmak3Keymap[] = {4, 5, 6, 22,  9, 23,  7, 11, 12, 17,  8, 15, 16, 14, 18, 19, 20, 13, 21, 10, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 42};
PROGMEM prog_uint8_t tarmak4Keymap[] = {4, 5, 6, 22,  9, 23,  7, 11, 12, 17,  8, 15, 16, 14, 28, 51, 20, 19, 21, 10, 24, 25, 26, 27, 13, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 18, 52, 53, 54, 55, 56, 42};
PROGMEM prog_uint8_t colemakKeymap[] = {4, 5, 6, 22,  9, 23,  7, 11, 24, 17,  8, 12, 16, 14, 28, 51, 20, 19, 21, 10, 15, 25, 26, 27, 13, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 18, 52, 53, 54, 55, 56, 42};
PROGMEM prog_uint8_t dvorakKeymap[] = {4, 27, 13, 8, 55, 24, 12, 7, 6, 11, 23, 17, 16, 5, 21, 15, 52, 19, 18, 28, 10, 14, 54, 20, 9, 51, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 47, 48, 56, 46, 49, 50, 22, 45, 53, 26, 25, 29, 57};
PROGMEM prog_uint8_t workmanKeymap[] = {4, 25, 16, 11, 21, 23, 10, 28, 24, 17, 8, 18, 15, 14, 19, 51, 20, 26, 22, 5, 9, 6, 7, 27, 13, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 12, 52, 53, 54, 55, 56, 42};

const uint8_t *Keymap[] =
{   
    qwertyKeymap,
    tarmak1Keymap,
    tarmak2Keymap,
    tarmak3Keymap,
    tarmak4Keymap,
    colemakKeymap,
    dvorakKeymap,
    workmanKeymap
};

KeyboardLayout CurrentLayout = tarmak2;

boolean keyboardInitialized = false;
void pressKey(uint8_t key);
void releaseKey(uint8_t key);

// Keep track of press/release events for every key.
uint8_t keyPressCount[256];

const uint8_t modifier_keys[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, KEY_LEFT_GUI,
        KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, KEY_RIGHT_ALT, KEY_RIGHT_GUI};

class KbdRptParser : public KeyboardReportParser
{
        uint8_t mapToAscii(uint8_t);
        void PrintKey(uint8_t mod, uint8_t key);

protected:
        virtual void OnControlKeysChanged(uint8_t before, uint8_t after);
        virtual void OnKeyDown(uint8_t mod, uint8_t key);
        virtual void OnKeyUp(uint8_t mod, uint8_t key);
};

// Adapted from KeyboardReportParser::OemToAscii
// Maps the key reported from the USB-Host to a byte that can be passed to Keyboard.press()
// As well as mapping the printable characters, it also maps most of the non-printable keys.
uint8_t KbdRptParser::mapToAscii(uint8_t key) {
        // [a-z]
        if (VALUE_WITHIN(key, 0x04, 0x1d)) {
                return pgm_read_byte(Keymap[CurrentLayout] + key - 4) + 'a' - 4;
//                return (key - 4 + 'a');
        }// Numbers
        else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
                 return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
        }// Keypad Numbers
        else if(VALUE_WITHIN(key, 0x59, 0x61)) {
                return (key - 0x59 + '1');
        } else if(VALUE_WITHIN(key, 0x2d, 0x38)) {
                return (uint8_t)pgm_read_byte(&getSymKeysLo()[key - 0x2d]);
        } else if(VALUE_WITHIN(key, 0x54, 0x57)) {
                return (uint8_t)pgm_read_byte(&getPadKeys()[key - 0x54]);
        } else if(VALUE_WITHIN(key, 0x3A, 0x45)) {
                // Function keys F1- F12
                return key - 0x3A + 0xC2;
        } else if(VALUE_WITHIN(key, 0x4F, 0x52)) {
                // Arrow keys
                return key - 0x4F + 0xD7;
        } else {
                switch(key) {
                        case UHS_HID_BOOT_KEY_SPACE: return 0x20;
                        case UHS_HID_BOOT_KEY_MAC_ENTER: return 0x0A;
                        case UHS_HID_BOOT_KEY_MAC_RETURN: return KEY_RETURN;
                        case UHS_HID_BOOT_KEY_ZERO2: return '0';
                        case UHS_HID_BOOT_KEY_PERIOD: return '.';
                        case UHS_HID_BOOT_KEY_ESC: return KEY_ESC;
                        case UHS_HID_BOOT_KEY_DEL: return KEY_DELETE;
                        case UHS_HID_BOOT_KEY_BACKSPACE: return KEY_BACKSPACE;
                        case UHS_HID_BOOT_KEY_HOME: return KEY_HOME;
                        case UHS_HID_BOOT_KEY_PAGE_UP: return KEY_PAGE_UP;
                        case UHS_HID_BOOT_KEY_END: return KEY_END;
                        case UHS_HID_BOOT_KEY_PAGE_DN: return KEY_PAGE_DOWN;
                        case UHS_HID_BOOT_KEY_PAD_EQL: return '=';
                        case UHS_HID_BOOT_KEY_TAB: return KEY_TAB;
                        case UHS_HID_BOOT_KEY_CAPS: return KEY_CAPS_LOCK;
                }
        }
        return ( 0);
}

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
  if (debug) {
    MODIFIERKEYS mod;
    *((uint8_t*)&mod) = m;
    Serial.print((mod.bmLeftCtrl   == 1) ? "C" : " ");
    Serial.print((mod.bmLeftShift  == 1) ? "S" : " ");
    Serial.print((mod.bmLeftAlt    == 1) ? "A" : " ");
    Serial.print((mod.bmLeftGUI    == 1) ? "G" : " ");

    Serial.print(" >");
    PrintHex<uint8_t>(key, 0x80);
    Serial.print("< ");

    Serial.print((mod.bmRightCtrl   == 1) ? "C" : " ");
    Serial.print((mod.bmRightShift  == 1) ? "S" : " ");
    Serial.print((mod.bmRightAlt    == 1) ? "A" : " ");
    Serial.println((mod.bmRightGUI    == 1) ? "G" : " ");
  }
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
    if (debug) {
      Serial.print("DN ");
      PrintKey(mod, key);
    }
    uint8_t c = mapToAscii(key);

    if (c) {
        pressKey(c);
    }
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {
    MODIFIERKEYS beforeMod;
    *((uint8_t*)&beforeMod) = before;

    MODIFIERKEYS afterMod;
    *((uint8_t*)&afterMod) = after;

    uint8_t allBefore[] = {beforeMod.bmLeftCtrl, beforeMod.bmLeftShift, beforeMod.bmLeftAlt, beforeMod.bmLeftGUI,
          beforeMod.bmRightCtrl, beforeMod.bmRightShift, beforeMod.bmRightAlt, beforeMod.bmRightGUI};
    uint8_t allAfter[] = {afterMod.bmLeftCtrl, afterMod.bmLeftShift, afterMod.bmLeftAlt, afterMod.bmLeftGUI,
          afterMod.bmRightCtrl, afterMod.bmRightShift, afterMod.bmRightAlt, afterMod.bmRightGUI};
    
    for(int i = 0; i < 8; i++) {
      if (allBefore[i] != allAfter[i]) {
        if (allAfter[i]) {
          pressKey(modifier_keys[i]);
        } else {
          releaseKey(modifier_keys[i]);
        }
      }
    }
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
    if (debug) {
      Serial.print("UP ");
      PrintKey(mod, key);
    }
    uint8_t c = mapToAscii(key);

    if (c) {
        releaseKey(c);
    }
}

void pressKey(uint8_t key)
{
    if (!keyboardInitialized) {
        keyboardInitialized = true;
        if (debug) {
          Serial.println("Initializing keyboard");
        } else {
          Keyboard.begin();
        }
    }

    keyPressCount[key]++;
    if (keyPressCount[key] == 1) {
      if (debug) {
        Serial.print("Press: ");
        Serial.println(key, HEX);
      } else {
        Keyboard.press(key);
      }
    }
};

void releaseKey(uint8_t key)
{
    keyPressCount[key]--;
    if (keyPressCount[key] == 0) {
      if (debug) {
        Serial.print("Release: ");
        Serial.println(key, HEX);
      } else {
        Keyboard.release(key);
      }
    }
};

USB        usb;
USBHub     hub(&usb);
USBHub     hub2(&usb);
USBHub     hub3(&usb);
USBHub     hub4(&usb);
USBHub     hub5(&usb);
USBHub     hub6(&usb);
USBHub     hub7(&usb);

// Don't use the mouse or composite variation unless you can carefully connect them
// the order they are are activated.
// HIDBoot<HID_PROTOCOL_KEYBOARD | HID_PROTOCOL_MOUSE> hidComposite1(&usb);
HIDBoot<HID_PROTOCOL_KEYBOARD>    hidKeyboard1(&usb);
HIDBoot<HID_PROTOCOL_KEYBOARD>    hidKeyboard2(&usb);
HIDBoot<HID_PROTOCOL_KEYBOARD>    hidKeyboard3(&usb);

// KbdRptParser compositeKeyboardReportParser1;
KbdRptParser keyboardReportParser1;
KbdRptParser keyboardReportParser2;
KbdRptParser keyboardReportParser3;

void setup()
{
    for (int i = 0; i < 256; i++) {
      keyPressCount[i] = 0;
    }
  
    if (debug) {
        Serial.begin( 115200 );
        while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
        Serial.println("Started");
    }
    if (usb.Init() == -1 && debug) {
        Serial.println("OSC did not start.");
    }

    delay( 200 );

//    hidComposite1.SetReportParser(0, (HIDReportParser*)&compositeKeyboardReportParser1);
//    hidComposite1.SetReportParser(1, (HIDReportParser*)&compositeMouseReportParser1);
    hidKeyboard1.SetReportParser(0, (HIDReportParser*)&keyboardReportParser1);
    hidKeyboard2.SetReportParser(0, (HIDReportParser*)&keyboardReportParser2);
    hidKeyboard3.SetReportParser(0, (HIDReportParser*)&keyboardReportParser3);
}

void loop()
{
    usb.Task();
}

