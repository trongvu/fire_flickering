//MÔ TẢ:

//Đây là mạch điều khiển đèn LED lắp trong xe ô tô đồ chơi, điều khiển bằng hồng ngoại, sử dụng chip Atmega328, 

//Đầu bài bổ sung:

// 1. const unsigned long AAAA     = 0xXXXX; thì 3 đèn dưới đây:
//CASE 11        = ON/OFF PIN 11  - READING
//CASE 12        = ON/OFF PIN 12  - RESERVE 1
//CASE 13        = ON/OFF PIN 13  - RESERVE 2

//sẽ nháy theo hiệu ứng sequence đèn cảnh sát: PIN 11 nháy nhanh 2 lần - STOP - PIN 12 nháy nhanh 2 lần rồi STOP (LẶP LẠI 2 LẦN)
                                            //PIN 11 nháy nhanh 1 lần - STOP - PIN 12 nháy nhanh 1 lần rồi STOP - PIN 11 nháy nhanh 1 lần - STOP - PIN 12 nháy nhanh 1 lần rồi STOP (LẶP LẠI 2 LẦN)
                                            //CHẠY ĐUỔI (TỐC ĐỘ NHANH) PIN 11 - TẮT - PIN 12 - TẮT - PIN 13 - TẮT 
                                            //CHẠY ĐUỔI (TỐC ĐỘ NHANH) PIN 13 - TẮT - PIN 12 - TẮT - PIN 11 - TẮT - SEQUENCE CHẠY LẶP 3 LẦN 
                                            //PIN 11, PIN 12, PIN 13 NHÁY NHANH RANDOM KHÔNG THEO PATTERN NÀO TRONG KHOẢNG 5 GIÂY
                                            //TOÀN BỘ SEQUENCE TRÊN CHẠY VÒNG LẶP CHO TỚI KHI BẤM AAAA LẦN NỮA
                                            //TOÀN BỘ CÁC THÔNG SỐ NHÁY, SỐ LẦN SEQUENCE CỦA MỖI PATTERN TRÊN CÓ THỂ ĐIỀU CHỈNH TRONG VALUE

//tham khảo hiệu ứng ở đây: https://www.youtube.com/watch?v=lvONX2qOfss (không phải giống hệt như something like this) - anh chỉ có 3 đèn trong khi họ có 10 đèn.  

// 2. const unsigned long ABAB      = 0xYzyx; thì:

//CASE 11        = ON/OFF PIN 11  - READING
//CASE 12        = ON/OFF PIN 12  - RESERVE 1
//CASE 13        = ON/OFF PIN 13  - RESERVE 2

//sẽ bật theo hiệu ứng đèn hàn như thế này https://www.youtube.com/watch?v=8qcqw1eFXyI hoặc mấy video gửi kèm email


// 3. const unsigned long BBBB      = 0xYYYY; thì:

//CASE 7         = ON/OFF PIN 7   - LEFT SIGNAL
//CASE 8         = ON/OFF PIN 8   - RIGHT SIGNAL

//PIN 7 nháy nhanh 2 lần - TẮT - PIN 8 nháy nhanh 2 lần - TẮT (lặp lại liên tục cho tới khi bấm BBBB lần nữa) - Tốc độ nháy có thể điều chỉnh trong giá trị VALUE

// 4. const unsigned long CCCC      = 0xZZZZ; thì: 

//CASE 7         = ON/OFF PIN 7   - LEFT SIGNAL
//CASE 8         = ON/OFF PIN 8   - RIGHT SIGNAL

//nháy cùng lúc 2 lần rồi tắt (khởi động/khóa xe)

// 5. Hiện bấm phím POWER thì bật tất cả các đèn, bấm lần nữa thì tắt tất cả các đèn. Tuy nhiên, nếu đang có đèn nào đang sáng mà bấm POWER thì đèn đó lại tắt mà tất cả các đèn 
//lại bật, theo dạng TOGGLE. Muốn cứ bấm POWER thì cho dù đèn nào (1 hoặc nhiều đèn) đang bật cũng tắt, bấm lần nữa thì tất cả đều bật.

//----------------------

//IR CONTROL    = PIN 2
//HEADLIGHT 1   = PIN 3   * (1:144 = HEADLIGHT1)
//HEADLIGHT 2   = PIN 4   * (1:144 = HEADLIGHT2)
//FOG LIGHT     = PIN 5   * (1:144 = FOGLIGHT)
//CABIN LIGHT   = PIN 6   * (1:144 = CABIN LIGHT )
//LEFT SIGNAL   = PIN 7   * (1:144 = LEFT SIGNAL)
//RIGHT SIGNAL  = PIN 8   * (1:144 = RIGHT SIGNAL)
//TAIL LIGHT    = PIN 9   * (1:144 = TAIL LIGHT)
//REVERSE       = PIN 10  * (1:144 = REVERSE)
//READING       = PIN 11  * (1:144 = READING LIGHT)
//RESERVE 1     = PIN 12  * (1:144 = LIGHT AROUND TRUCK)
//RESERVE 2     = PIN 13  * (1:144 = HIGH STOPLIGHT OR SPECIAL EFFECT LIGHT IN FIRETRUCK)
//SERVO 1       = PIN A0 - ON-DEMAND
//SERVO 2       = PIN A1 - CONTINUOUS

//CASE 3         = ON/OFF PIN 3   - HEADLIGHT 1
//CASE 4         = ON/OFF PIN 4   - HEADLIGHT 2
//CASE 5         = ON/OFF PIN 5   - FOG LIGHT
//CASE 6         = ON/OFF PIN 6   - CABIN LIGHT
//CASE 7         = ON/OFF PIN 7   - LEFT SIGNAL
//CASE 8         = ON/OFF PIN 8   - RIGHT SIGNAL
//CASE 9         = ON/OFF PIN 9   - TAIL LIGHT
//CASE 10        = ON/OFF PIN 10  - REVERSE LIGHT
//CASE 11        = ON/OFF PIN 11  - READING
//CASE 12        = ON/OFF PIN 12  - RESERVE 1
//CASE 13        = ON/OFF PIN 13  - RESERVE 2 - BLINKING FAST
//CASE POWER     = ON/OFF ALL LIGHTS 
//CASE EMMERGENCY= ON/OFF SIGNAL LIGHT
//TOTAL          = 13 FUNCTIONAL BUTTONS


#include <IRremote.h>
#include <Servo.h>

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);
Servo servo_1;
Servo servo_2;

decode_results results;

const unsigned long THREE       = 0x562A;//HEADLIGHT1   //WILLIAMS    - SONY PROJECTOR RM-PJ8         - UP
const unsigned long FOUR        = 0x752A;//HEADLIGHT2   //WILLIAMS    - SONY PROJECTOR RM-PJ8         - INPUT
const unsigned long FIVE        = 0x6B54;//FOG LIGHT    //WILLIAMS    - SONY PROJECTOR RM-PJ8         - APA
const unsigned long SIX         = 0x88BE4;//CABIN LIGHT //WILLIAMS    - SONY PROJECTOR RM-PJ8         - ECO MODE
const unsigned long SEVEN       = 0x162A;//LEFT SIGNAL  //WILLIAMS    - SONY PROJECTOR RM-PJ8         - LEFT
const unsigned long EIGHT       = 0x662A;//RIGHT SIGNAL //WILLIAMS    - SONY PROJECTOR RM-PJ8         - RIGH
const unsigned long NINE        = 0x362A;//TAIL LIGHT   //WILLIAMS    - SONY PROJECTOR RM-PJ8         - DOWN
const unsigned long TEN         = 0x4A2A;//REVERSE      //WILLIAMS    - SONY PROJECTOR RM-PJ8         - MENU
const unsigned long ELEVEN      = 0x6F2A;//READING      //WILLIAMS    - SONY PROJECTOR RM-PJ8         - RESET
const unsigned long TWELVE      = 0x122A; //RESERVE 1   //WILLIAMS    - SONY PROJECTOR RM-PJ8         - BLANK
const unsigned long THIRTEEN    = 0x142A; //RESERVE2 BL //WILLIAMS    - SONY PROJECTOR RM-PJ8         - MUTING
const unsigned long POWER       = 0x542A; //ON/OFF      //WILLIAMS    - SONY PROJECTOR RM-PJ8         - POWER
const unsigned long EMMERGENCY  = 0x2D2A; //ON/OFF      //WILLIAMS    - SONY PROJECTOR RM-PJ8         - ENTER
const unsigned long SERVO1_1    = 0xE6B54; //SERVO CONTINUOUS   //WILLIAMS    - SONY PROJECTOR RM-PJ8       - FREEZE
const unsigned long SERVO22     = 0x56B54; //SERVO +            //WILLIAMS    - SONY PROJECTOR RM-PJ8       - D-ZOOM +
const unsigned long SERVO2_     = 0xD6B54; //SERVO -            //WILLIAMS    - SONY PROJECTOR RM-PJ8       - D-ZOOM -

int LED_3 = 3;//HEAD LIGHT 1
bool LED_3_state = false;

int LED_4 = 4;//HEAD LIGHT 2
bool LED_4_state = false;

int LED_5 = 5;//FOG LIGHT
bool LED_5_state = false;

int LED_6 = 6;//CABIN LIGHT//NAVIGATION LIGHT
bool LED_6_state = false;

int LED_7_blink = 7;//LEFT SIGNAL LIGHT
bool LED_7_state = false;
unsigned long time_LED_7 = 0;

int LED_8_blink = 8;//RIGHT SIGNAL LIGHT
bool LED_8_state = false;
unsigned long time_LED_8 = 0;

int LED_13_blink = 13;//RESERVE 2 - BLINKING
bool LED_13_state = false;
unsigned long time_LED_13 = 0;

int LED_9 = 9;//TAIL LIGHT
bool LED_9_state = false;

int LED_10 = 10;//REVERSE 
bool LED_10_state = false;

int LED_11 = 11;//READING LIGHT
bool LED_11_state = false;

int LED_12 = 12;//RESERVE 1
bool LED_12_state = false;

int SERVO_1 = A1;
bool SERVO_1_state = 0;
int pos_1 = 0;
bool dir_ser_1 = true;

int SERVO_SPEED = 30;

int SERVO_2 = A0;
int pos_2 = 0;
byte SERVO_2_state_1 = 0;
byte SERVO_2_state_2 = 0;

//trongvu
bool power_state = false;
void control_all_lights(bool turn_on){
  //update sequence control value
  LED_3_state = false;
  LED_4_state = false;
  LED_5_state = false;
  LED_6_state = false; 
  LED_7_state = false;
  LED_8_state = false;
  LED_9_state = false;
  LED_10_state = false;
  LED_11_state = false;
  LED_12_state = false;
  LED_13_state = false;
  
  digitalWrite(LED_3, turn_on);
  digitalWrite(LED_4, turn_on);
  digitalWrite(LED_5, turn_on);
  digitalWrite(LED_6, turn_on);
  digitalWrite(LED_7_blink, turn_on);
  digitalWrite(LED_8_blink, turn_on);
  digitalWrite(LED_9, turn_on);
  digitalWrite(LED_10, turn_on);
  digitalWrite(LED_11, turn_on);
  digitalWrite(LED_12, turn_on);
  digitalWrite(LED_13_blink, turn_on);
}

bool changeState(bool state) {
  return !state;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("ACTIVATING IR");
  irrecv.enableIRIn();
  Serial.println("IR ACTIVATED");

  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_5, OUTPUT);
  pinMode(LED_6, OUTPUT);
  pinMode(LED_7_blink, OUTPUT);
  pinMode(LED_8_blink, OUTPUT);
  pinMode(LED_9, OUTPUT);
  pinMode(LED_10, OUTPUT);
  pinMode(LED_11, OUTPUT);
  pinMode(LED_12, OUTPUT);
  pinMode(LED_13_blink, OUTPUT);

  servo_1.attach(SERVO_1);
  servo_2.attach(SERVO_2);
}
void loop() {
    //reset power state

    if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume();

    switch (results.value) {
      case THREE: 
        LED_3_state = changeState(LED_3_state);
        break;
      case FOUR:
        LED_4_state = changeState(LED_4_state);
        break;
      case FIVE: 
        LED_5_state = changeState(LED_5_state);
        break;
      case SIX: 
        LED_6_state = changeState(LED_6_state);
        break;
      case SEVEN: 
        LED_7_state = changeState(LED_7_state);
        break;
      case EIGHT: 
        LED_8_state = changeState(LED_8_state);
        break;
      case EMMERGENCY:
        LED_7_state = changeState(LED_7_state);
        LED_8_state = changeState(LED_8_state);
        break;
      case NINE:   
        LED_9_state = changeState(LED_9_state);
        break;
      case TEN: 
        LED_10_state = changeState(LED_10_state);
        break;
      case ELEVEN: 
        LED_11_state = changeState(LED_11_state);
        break;
      case TWELVE: 
        LED_12_state = changeState(LED_12_state);
        break;
      case THIRTEEN: 
        LED_13_state = changeState(LED_13_state);
        break;
      case POWER:
      //case 4: power
        power_state = !power_state;
        control_all_lights(power_state);
        break;
      case SERVO1_1: 
        SERVO_1_state = changeState(SERVO_1_state);
        break;
      case SERVO22: 
        SERVO_2_state_1 ++;
        SERVO_2_state_2 = 0;
        if ( SERVO_2_state_1 == 3)
          SERVO_2_state_1 = 1; // continous
        break;
      case SERVO2_: 
        SERVO_2_state_1 = 0;
        SERVO_2_state_2 ++;
        if ( SERVO_2_state_2 == 3)
          SERVO_2_state_2 = 1;
        break;
      default:
        break;
    }
  }

  if (LED_3_state == true) {
    digitalWrite(LED_3, !digitalRead(LED_3));
    LED_3_state = false;
  }
  if (LED_4_state == true) {
    digitalWrite(LED_4, !digitalRead(LED_4));
    LED_4_state = false;
  }
  if (LED_5_state == true) {
    digitalWrite(LED_5, !digitalRead(LED_5));
    LED_5_state = false;
  }
  if (LED_6_state == true) {
    digitalWrite(LED_6, !digitalRead(LED_6));
    LED_6_state = false;
  }
  if (LED_9_state == true) {
    digitalWrite(LED_9, !digitalRead(LED_9));
    LED_9_state = false;
  }
  if (LED_10_state == true) {
    digitalWrite(LED_10, !digitalRead(LED_10));
    LED_10_state = false;
  }
  if (LED_11_state == true) {
    digitalWrite(LED_11, !digitalRead(LED_11));
    LED_11_state = false;
  }
  if (LED_12_state == true) {
    digitalWrite(LED_12, !digitalRead(LED_12));
    LED_12_state = false;
  }
  if (LED_7_state == true) {
    if (millis() - time_LED_7 > 800) {
      time_LED_7 = millis();
      digitalWrite(LED_7_blink, !digitalRead(LED_7_blink));
    }
  }
  else  if(!power_state) {
    digitalWrite(LED_7_blink, LOW);
  }
  if (LED_8_state == true) {
    if (millis() - time_LED_8 > 800) {
      time_LED_8 = millis();
      digitalWrite(LED_8_blink, !digitalRead(LED_8_blink));
    }
  }
  else if(!power_state) {
    digitalWrite(LED_8_blink, LOW);
  }
  if (LED_13_state == true) {
    if (millis() - time_LED_13 > 400) {
      time_LED_13 = millis();
      digitalWrite(LED_13_blink, !digitalRead(LED_13_blink));
    }
  }
  else if(!power_state) {
    digitalWrite(LED_13_blink, LOW);
  }
  
    if (SERVO_1_state == true) {
      servo_1.write(pos_1);
      if (dir_ser_1 == true) {
        if ( pos_1 < 90)
          pos_1++;
        else
          dir_ser_1 = false;
      }
      else {
        if ( pos_1 > 0)
          pos_1--;
        else
          dir_ser_1 = true;
      }
    }

    if (SERVO_2_state_1 == true) {
      servo_2.write(pos_2);
      if ( pos_2 < 90)
        pos_2++;
      else
        SERVO_2_state_1 = false;
    }

    if (SERVO_2_state_2 == true) {
      servo_2.write(pos_2);
      if ( pos_2 > 0)
        pos_2 --;
      else
        SERVO_2_state_2 = false;
    }
    delay(SERVO_SPEED);
}
