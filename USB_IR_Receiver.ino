#include <IRremote.h> // using old version currently (2.2.3)
#include <Keyboard.h>
#include <Mouse.h>
 
//The below IR codes are for Sony DVP-SR210P, according to Logitech MyHarmony software
#define AUDIO       0x26B92
#define CHAPNEXT    0x8CB92
#define CHAPPREV    0x0CB92
#define DIRDOWN     0x5EB92
#define DIRUP       0x9EB92
#define DIRLEFT     0xDEB92
#define DIRRIGHT    0x3EB92
#define DISPLAY     0x2AB92
#define FFORWARD    0xC4B92
#define MENU        0xD8B92
#define OPENCLOSE   0x68B92
#define PAUSE       0x9CB92
#define PLAY        0x4CB92
#define POWEROFF    0xF4B92
#define POWERON     0x74B92
#define POWERTOGGLE 0xA8B92
#define RETURN      0x70B92
#define REWIND      0x44B92
#define SELECT      0xD0B92
#define STOP        0x1CB92
#define SUBTITLE    0xC6B92
#define NUM0        0x90B92
#define NUM1        0x00B92
#define NUM2        0x80B92
#define NUM3        0x40B92
#define NUM4        0xC0B92
#define NUM5        0x20B92
#define NUM6        0xA0B92
#define NUM7        0x60B92
#define NUM8        0xE0B92
#define NUM9        0x10B92

#define KEY_SPACEBAR 32

#define CODETYPE 4  // IR code type to listen for
#define RECV_PIN 10 // Output pin on IR receiver
#define GND_PIN 15  // Ground pin on IR receiver
#define AT_DELAY 1.5 // Max is ~4.19s
 
IRrecv irrecv(RECV_PIN);
decode_results results;
bool altPressed = false;
 
void setup(){
  // Setup timer1 to handle delay for alt-tab functionality
  cli(); // disable interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  //OCR1A = [(desired delay period (s)) * (clock speed (Hz)) / (prescaler (int))]  - 1
  OCR1A = min(round(AT_DELAY * 16e6 / 1024 - 1), 65535); // range is 1-65535
  TCCR1B |= (1 << WGM12); // turn on CTC (Clear Timer on Compare match) mode
  TCCR1B |= (1 << CS10) | (1 << CS12); // Set prescaler to 1024
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei(); //reenable interrupts
  
  pinMode(GND_PIN, OUTPUT);
  digitalWrite(GND_PIN, LOW); //Enables IR receiver
  Serial.begin(9600);
  irrecv.enableIRIn();
  Keyboard.begin();
  Mouse.begin();
  Serial.println("OCR1A = " + OCR1A);
}

ISR(TIMER1_COMPA_vect) {
  // This releases the alt key AT_DELAY seconds after the AUDIO command is last received
  if (altPressed) {
    Keyboard.release(KEY_LEFT_ALT);
    altPressed = false;
  }
}
 
void interpretResults(decode_results *results) {
  int codeType = results->decode_type;
  if (codeType == CODETYPE) {
    switch (results->value){
     
      case AUDIO:
        // Alt-Tab
        Serial.println("AUDIO (alt-tab)");
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.write(KEY_TAB);
        altPressed = true;
        TCNT1 = 0; // reset timer1 counter to 0 (reset delay period)
        break;
       
      case CHAPNEXT:
        // Next chapter
        Serial.println("CHAPNEXT (next chapter)");
        Keyboard.write(KEY_PAGE_DOWN);
        break;
       
      case CHAPPREV:
        // Previous chapter
        Serial.println("CHAPPREV (previous chapter)");
        Keyboard.write(KEY_PAGE_UP);
        break;

      case DIRUP:
        // Next subtitle track
        Serial.println("DIRUP (next subtitle track)");
        Keyboard.write('s');
        break;
       
      case DIRDOWN:
        // Previous subtitle track
        Serial.println("DIRDOWN (previous subtitle track)");
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.write('s');
        Keyboard.release(KEY_LEFT_SHIFT);
        break;
       
      case DIRLEFT:
        // Increase audio delay
        Serial.println("DIRLEFT (increase audio delay)");
        Keyboard.write(KEY_F3);
        break;
       
      case DIRRIGHT:
        // Decrease audio delay
        Serial.println("DIRRIGHT (decrease audio delay)");
        Keyboard.write(KEY_F4);
        break;
       
      case DISPLAY:
        // Display current file name (OSD)
        Serial.println("DISPLAY (display currently loaded filename)");
        Keyboard.write('n');
        break;
       
      case FFORWARD:
        // Large jump forward (12 sec)
        Serial.println("FFORWARD (large jump forward)");
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.write(KEY_RIGHT_ARROW);
        Keyboard.release(KEY_LEFT_ALT);
        break;
       
      case MENU:
        // Display current time (OSD)
        Serial.println("MENU (display current/total playtime)");
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.write('i');
        Keyboard.release(KEY_LEFT_CTRL);
        break;
       
      case OPENCLOSE:
        // Open subtitle download window
        Serial.println("OPENCLOSE (open sutitle download window)");
        Keyboard.write('d');
        break;
       
      case PAUSE:
        // Pause
        Serial.println("PAUSE (pause)");
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.write(KEY_SPACEBAR);
        Keyboard.release(KEY_LEFT_SHIFT);
        break;
       
      case PLAY:
        // Play
        Serial.println("PLAY (play)");
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.write(KEY_SPACEBAR);
        Keyboard.release(KEY_LEFT_CTRL);
        break;
       
      case POWEROFF:
        // Decrease subtitle delay
        Serial.println("CHANNEL DOWN (decrease subtitle delay)");
        Keyboard.write(KEY_F1);
        break;
       
      case POWERON:
      // Increase subtitle delay
        Serial.println("VOLUME UP (increase subtitle delay)");
        Keyboard.write(KEY_F2);
        break;
       
      case POWERTOGGLE:
        // Full-screen
        Serial.println("POWERTOGGLE (fullscreen)");
        Keyboard.write(KEY_F11);
        break;
      
      case RETURN:
        // ESC
        Serial.println("RETURN (esc key)");
        Keyboard.write(KEY_ESC);
        break;
       
      case REWIND:
        // Large jump backward (12 sec)
        Serial.println("REWIND (large jump backward)");
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.write(KEY_LEFT_ARROW);
        Keyboard.release(KEY_LEFT_ALT);
        break;
       
//      case SELECT:
//        Serial.println("SELECT");
//        Keyboard.press(KEY_RETURN);
//        break;
//       
//      case STOP:
//        Serial.println("STOP");
//        break;
       
      case SUBTITLE:
        // Toggle subtitles
        Serial.println("SUBTITLE (toggle subtitles)");
        Keyboard.print('w');
        break;
       
//      case NUM0:
//        Serial.println("NUM0");
//        break;
//       
//      case NUM1:
//        Serial.println("NUM1");
//        break;
//       
//      case NUM2:
//        Serial.println("NUM2");
//        break;
//       
//      case NUM3:
//        Serial.println("NUM3");
//        break;
//       
//      case NUM4:
//        Serial.println("NUM4");
//        break;
//       
//      case NUM5:
//        Serial.println("NUM5");
//        break;
//       
//      case NUM6:
//        Serial.println("NUM6");
//        break;
//       
//      case NUM7:
//        Serial.println("NUM7");
//        break;
//       
//      case NUM8:
//        Serial.println("NUM8");
//        break;
//       
//      case NUM9:
//        Serial.println("NUM9");
//        break;
       
      default:
        Serial.println(results->value, HEX);
        break;
    }
  }else {
    //Serial.print("Code type: ");
    //Serial.println(codeType);
  }
}
 
void loop() {
  if (irrecv.decode(&results)){
    interpretResults(&results);
    irrecv.resume();
  }
}
