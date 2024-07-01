/*
   Arduino = nano 328
   receiver bluetooth di serial RX, 9600 baud
   output motor 2,3 (kanan) 4,5 (kiri)
   2 = port D2  =M1B(kanan)
   3 = D3       =M1A(kanan) = maju
   4 = D4       =M2B(kiri)
   5 = D5       =M2A(kiri) = maju
   Sensor:
   A0 - lantai, kiri
   A1 - lantai, kanan

*/
//#include<Servo.h>

#define LED1BIT 0x04
#define LED2BIT 0x08
#define M2ABIT 0x20
#define M2BBIT 0x10
#define M1ABIT 0x08
#define M1BBIT 0x04
#define led1on() \
  PORTB &= ~LED1BIT; \
  DDRB |= LED1BIT
#define led2on() \
  PORTB &= ~LED2BIT; \
  DDRB |= LED2BIT
#define led1off() \
  DDRB &= ~LED1BIT; \
  PORTB |= LED1BIT
#define led2off() \
  DDRB &= ~LED2BIT; \
  PORTB |= LED2BIT
#define tombol1() (PINB & LED1BIT)
#define tombol2() (PINB & LED2BIT)
#define motor1enable() DDRD |= (M1ABIT | M1BBIT)
#define motor2enable() DDRD |= (M2ABIT | M2BBIT)
#define motor1disable() DDRD &= ~(M1ABIT | M1BBIT)
#define motor2disable() DDRD &= ~(M2ABIT | M2BBIT)
//#define motorenable() motor2enable();motor1enable()
//#define motordisable() motor2disable();motor1disable()
#define motorenable() DDRD |= (M1ABIT | M1BBIT | M2ABIT | M2BBIT)
#define motordisable() DDRD &= ~(M1ABIT | M1BBIT | M2ABIT | M2BBIT)
#define m1fwd() \
  PORTD |= M1ABIT; \
  PORTD &= ~M1BBIT
#define m1rev() \
  PORTD |= M1BBIT; \
  PORTD &= ~M1ABIT
#define m1off() PORTD &= ~(M1ABIT | M1BBIT)
#define m2fwd() \
  PORTD |= M2ABIT; \
  PORTD &= ~M2BBIT
#define m2rev() \
  PORTD |= M2BBIT; \
  PORTD &= ~M2ABIT
#define m2off() PORTD &= ~(M2ABIT | M2BBIT)
#define PWMPERIOD 8
volatile long t, ta1, tlastdata, tp, tlastpwmcycle;
volatile byte f1signal, f2signal, f3signal, f4signal;
int c, state, m1power = 0, m2power = 0, m1lastpower=0, m2lastpower=0;
int powerputar=3;
int powermaju=8;
int accel=1;
void berhenti() {
  m1off();
  led1off();
  m2off();
  led2off();
  m1power = 0;
  m2power = 0;
}
void maju(int p) {
  if(m1power+accel<=p){
  m1power = p;
  }
  else{
    m1power += accel;
  }
  if(m2power+accel<=p){
  m2power = p;
  }
  else{
    m2power+=accel;
  }
  led1on();
  led2on();
}
void mundur(int p) {
  m1power = -p;
  m2power = -p;
  led1on();
  led2on();
}
void putarkanan(int p) {  //m1 kanan mundur
  m1power = -p;
  m2power = p;
  led1off();
  led2on();
}
void putarkiri(int p) {  //m1 kanan maju
  m1power = p;
  m2power = -p;
  led1on();
  led2off();
}
void setpower(int m1p, int m2p) {
  m1power = m1p;
  m2power = m2p;
}
uint8_t waitforbyte(){
  while(!(Serial.available()));
  return Serial.read();
}
void setup() {
  Serial.begin(9600);
  led1on();
  led2off();
  delay(200);
  led2on();
  led1off();
  delay(200);
  led1on();
  led2off();
  delay(100);
  led2on();
  led1off();
  delay(100);
  led2off();
  pinMode(12, OUTPUT);
  /*c    c#    d    d#   e    f    f#    g   g#   a    a#    b   c
    1000 1059 1122 1189 1260 1335 1414 1498 1587 1682 1782 1888 2000
  */
  tone(12, 1000);
  delay(80);
  noTone(12);
  delay(20);
  tone(12, 1335);
  delay(80);
  noTone(12);
  delay(20);
  tone(12, 2000);
  delay(80);
  noTone(12);
  motorenable();
  //tstop = 0;
  state = 0;
  f1signal = 0;
  f2signal = 0;
  f3signal = 0;
  f4signal = 0;
  t = millis();
  tlastpwmcycle = t;
  tp = 0;
  ta1 = t;
  tlastdata = 0;
}
void loop() {
  t = millis();
  if (Serial.available() > 0) {
    //state = 0;
    c = Serial.read();
    tlastdata = t;
    switch (c) {
      case 0:
      case '0': berhenti(); break;  //berhenti
      case 0xa0:
      case 'w':
      case 'W':
        maju(powermaju);
        state = 0;
        break;  //maju
      case 0xa4:
      case 's':
      case 'S':
        mundur(powermaju);
        state = 0;
        break;  //mundur
      case 0xa7:
      case 'q':
      case 'Q':
        setpower(2, 8);
        state = 0;
        break;  //belok kiri
      case 0xa5:
      case 'z':
      case 'Z':
        setpower(-2, -8);
        state = 0;
        break;  //belok kiri mundur
      case 0xa1:
      case 'e':
      case 'E':
        setpower(8, 2);
        state = 0;
        break;  //belok kanan
      case 0xa3:
      case 'c':
      case 'C':
        setpower(-8, -2);
        state = 0;
        break;  //belok kanan mundur
      case 0xa2:
      case 'd':
      case 'D':
        putarkanan(powerputar);
        state = 0;
        break;  //putar kanan
      case 0xa6:
      case 'a':
      case 'A':
        putarkiri(powerputar);
        state = 0;
        break;  //putar kiri
      case 0xaa:
      case 'i':
        f1signal = 1;
        state = 0;
        break;  //function 1
      case 0xab:
      case 'j':
        f2signal = 1;
        state = 0;
        break;  //function 2
      case 0xac:
      case 'k':
        f3signal = 1;
        state = 0;
        break;  //function 3
      case 0xad:
      case 'l':
        f4signal = 1;
        state = 0;
        break;  //function 4
      case 'p': //setting
        //tunggu byte berikutnya
        c=waitforbyte();
        switch(c){
          case 'a': //set accel
          int i = Serial.parseInt();
          Serial.print("accel=");
          Serial.println(i);
          accel=i;
        }
      default:;
    }
  }
  //proses tombol f1-f4
  switch (state) {
    case 0:
      if (f1signal) {
        f1signal = 0;
        state = 1;
        ta1 = t;
        led1on();
      }
      if (f2signal) {
        f2signal = 0;
        state = 2;
        ta1 = t;
        led1on();
      }
      if (f3signal) {
        f3signal = 0;
        state = 3;
        ta1 = t;
        led2on();
      }
      if (f4signal) {
        f4signal = 0;
        state = 4;
        ta1 = t;
        led2on();
      }
      break;

    case 1:  //manuver 1
      //putar kanan 60 derajat, maju melingkar ke kiri
      //150 ms untuk putar kanan
      //motor 1 = kanan
      putarkanan(powerputar);
      if (t - ta1 >= 150) {
        state = 7;
        ta1 = t;
      }
      break;
    case 7:
      //motor kanan 100%, motor kiri 50%
      setpower(8, 2);
      if (t - ta1 > 500) {
        putarkiri(powerputar);
        state = 8;
        ta1 = t;
      }
      break;
    case 8:
      putarkiri(powerputar);
      if (t - ta1 > 250) {
        maju(8);
        state = 0;
      }
      break;
    case 2:  //manuver 2, menghindar ke kiri belakang
      //motor kiri mundur 50%, motor kanan mundur 100%
      setpower(-8, -2);
      if (t - ta1 >= 500) {
        state = 5;
        ta1 = t;
      }
      break;
    case 5:  //putar kiri
      putarkiri(powerputar);
      if (t - ta1 >= 400) {
        state = 0;
        berhenti();
      }
      break;
    case 3:  //super push (pulsing)
      if (t % 120 > 20) {
        maju(8);
      } else {
        berhenti();
      }
      //keluar dari state ini dengan override tombol lain
      break;
    case 4:  //manuver 4 (= 2 tapi ke kanan)
      //motor kanan mundur 50%, motor kiri mundur 100%
      setpower(-2, -8);
      if (t - ta1 >= 400) {
        state = 6;
        ta1 = t;
      }
      break;
    case 6:
      putarkanan(powerputar);
      if (t - ta1 >= 300) {
        state = 0;
        berhenti();
      }
      break;
    default:;
  }
  //soft PWM
  tp = t - tlastpwmcycle;
  if (tp >= PWMPERIOD) {
    // Serial.print(tp);
    // Serial.print(' ');
    // Serial.print(m1power);
    //Serial.print(' ');
    //Serial.print(m2power);
    tlastpwmcycle = t;
    tp=0;
    if (m1power > 0) {
      m1fwd();
      //Serial.print("1f");
    } else if (m1power < 0) {
      m1rev();
      //Serial.print("1r");
    } else {
      m1off();
      //Serial.print("1o");
    }
    if (m2power > 0) {
      m2fwd();
    } else if (m2power < 0) {
      m2rev();
    } else {
      m2off();
    }
    //Serial.println();
  }
  if (tp >= abs(m1power)) {
    m1off();
    //Serial.println("1p");
  }
  if (tp >= abs(m2power)) {
    m2off();
  }
  if (t - tlastdata > 1000) {  //mode test, kalau tidak ada data serial
    //test tombol
    if (digitalRead(10) == LOW) {
      m1fwd();
      delay(1000);
      m1off();
    }
    if (digitalRead(11) == LOW) {
      m2fwd();
      delay(1000);
      m2off();
    }
  }
}
