#define S_IDLE 1
#define S_STRIKE_ARC 2
#define S_WHITE_ARC 3
#define S_BLUE_ARC 4
#define S_RED_GLOW 5
#define S_TURNOFF 6

static int state_main_control = S_IDLE; // initial state is idle.
static int state_blue_arc_control = S_IDLE; // initial state is idle.
static unsigned long ts;  // To store the "current" time.
static unsigned long wait;  // To store the wait time  for delays.

#include <IRremote.h>
#include <Servo.h>

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);
decode_results results;
const unsigned long FIRE_FLICKERING     = 0xF6BE4; //SERVO -            //WILLIAMS    - SONY PROJECTOR RM-PJ8       - D-ZOOM -
bool fire_flickering_state = 0;
static int ledPin_white_arc = 10; // White LED on pin 10
static int ledPin_blue_arc = 11;  // Blue LED on pin 11
static int ledPin_red_glow = 12;  // Red LED on pin 12

void setup()
{
  Serial.begin(9600);
  Serial.println("ACTIVATING IR");
  irrecv.enableIRIn();
  Serial.println("IR ACTIVATED");

  pinMode(ledPin_white_arc, OUTPUT);
  pinMode(ledPin_blue_arc, OUTPUT);
  pinMode(ledPin_red_glow, OUTPUT);
  
}
void loop() {
    //reset power state

    if (irrecv.decode(&results)) {
      Serial.println(results.value, HEX);
      irrecv.resume();
  
      switch (results.value) {
        case FIRE_FLICKERING:
          fire_flickering_state = !fire_flickering_state;
          Serial.write("button pressed, change status = %d\n", fire_flickering_state);
          state_main_control = S_IDLE;
          break;
        default:
          break;
      }
    }
    //lighting logic
 if(fire_flickering_state) {
  switch (state_main_control)// Main control state manages the arc welding cycle
  {
    case S_IDLE:
      ts = millis();  // Remember the current tim
      wait = random(5000, 10000); //Set a wait time before welding cycle starts.
      state_main_control = S_STRIKE_ARC; // Move to the Arc "striking" state.
      break;

    case S_STRIKE_ARC: // Simulates the striking needed between welding rod and work to start an arc
      if (millis() > ts + wait)
      {
        digitalWrite(ledPin_white_arc, HIGH ); // Turn the white arc LED on for first attempt
        delay(100); // First strike of white arc
        digitalWrite(ledPin_white_arc, LOW ); // Turn the white arc LED off
        delay(500); // Half second delay before second attempt
        digitalWrite(ledPin_white_arc, HIGH); // set the Arc LED on for the second attempt
        delay(200); // Second strike of white arc
        digitalWrite(ledPin_white_arc, LOW ); // Turn the white arc LED off
        delay (500); // Half second delay before second attempt
        digitalWrite(ledPin_white_arc, HIGH); // set the Arc LED on for the third attempt
        delay(300); // Third strike of white arc
        digitalWrite(ledPin_white_arc, LOW ); // Turn the white arc LED off in preparation for full arc

        ts = millis(); // Remember the current time
        wait = random(5000, 10000); //Set a random time for now loing the arc welding cycle to run
        state_main_control = S_WHITE_ARC; // Move on to next state
        state_blue_arc_control = S_BLUE_ARC; // start up a simultaneous blue arc with the white arc
      }
      break;

    case S_WHITE_ARC: // main flashing of white arc.
      if (ts + wait > millis())// provided random time not exceede then kep welding.
      {
        digitalWrite(ledPin_white_arc, HIGH); // set the Arc LED off
        delay(random(60));
        digitalWrite(ledPin_white_arc, LOW); // set the Arc LED on
        delay(random(200));
        break;
      }
      ts = millis(); // Remember current time
      wait = random(5000, 10000); //Set a random time for the weld glow to run
      state_main_control = S_RED_GLOW;
      state_blue_arc_control = S_IDLE;
      break;

    case S_RED_GLOW:// Simulates the cooling of the work from red hot after the arc is stopped using an analogue write
      if (ts + wait > millis())
      {
        int i;
        for (int i = 50; i > 0; i--) { //descrease i with 1
          analogWrite(ledPin_red_glow, i);
          delay(70);
        }
        state_main_control = S_TURNOFF;
        break;
      }
    case S_TURNOFF:
      digitalWrite(ledPin_red_glow, LOW); // Kill the last bit of analogue glow
      state_main_control = S_IDLE;
      break;
  }
  switch (state_blue_arc_control)// Separate state machine running blue arc alongside white arc at different flash frequency
  {
      case S_BLUE_ARC:
      digitalWrite(ledPin_blue_arc, HIGH); // set the Arc LED on
      delay(random(10));
      digitalWrite(ledPin_blue_arc, LOW); // set the Arc LED off
      delay(random(100));
      break;
  }
 }else {
    digitalWrite(ledPin_white_arc, LOW );
    digitalWrite(ledPin_blue_arc, LOW );
    digitalWrite(ledPin_red_glow, LOW );
  } 
}
