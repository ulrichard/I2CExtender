#include "TinyWireS.h"
#include "Servo8Bit.h"
#include <avr/eeprom.h>
#include <util/delay.h>

// https://learn.sparkfun.com/tutorials/tiny-avr-programmer-hookup-guide/attiny85-use-hints
// https://cdn.sparkfun.com/assets/2/8/b/a/a/Tiny_QuickRef_v2_2.pdf

// TODO: According to documentation, we should move the Servo PIN to PB1, pin 6
// Usable
// PB1  AIO, DIO
// PB3  AI, DIO
// PB4  AI, DIO

// 1  NC Reset
// 2  PB3 Analog Input, Digital Input, Digital Output
// 3  PB4 Analog Input, Digital Input, Digital Output
// 4  GND
// 5  PB0 I2C
// 6  PB1 Analog output, Analog Input, Digital Input, Digital Output
// 7  PB2 I2C
// 8  Power

// ATMEL ATTINY45 / ATTINY85
//                                    +-\/-+
// PCINT5/!RESET/ADC0/dW        PB5  1|    |8  Vcc
// PCINT3/XTAL1/CLKI/!OC1B/ADC3 PB3  2|    |7  PB2 SCK/USCK/ADC1/T0/INTO/PCINT2
// PCINT4/XTAL2/CLKO/OC1B/ADC2  PB4  3|    |6  PB1 MISO/D0/OC0B/OC1A/PCINT1            pwm1
//                              GND  4|    |5  PB0 MOSI/D1/SDA/AIN0/!OC0A/AREF/PCINT0  pwm0
//                                    +----+


// PWM pins are: PB0, PB1, PB3, PB4
// digital input: PB0, PB1, PB2, PB3, PB4
// digital output: PB0, PB1, PB2, PB3, PB4
// analog input: PB2, PB3, PB4
// analog output: PB0, PB1



#define SERVO_ANGLE  0x20
#define SERVO_ATTACH 0x21
#define SERVO_DETACH 0x23

#define RELAY_ATTACH 0x24
#define RELAY_DETACH 0x25
#define RELAY_HIGH   0x26
#define RELAY_LOW    0x27

volatile uint8_t myArray[10];
volatile bool flag = false;
Servo8Bit myservo;

int servoPin = 0;
int relayPin = 0;

static const uint8_t  i2cSlaveAddr = 21; //0x40; // ToDo: configure correctly

void handleEvent();
bool receive();

int main()
{
	_delay_ms(100); // give the master some time to grab the i2c bus

	DDRB = (1 << PB1); // Set Port B pin 1 as output for the relais

    TinyWireS.begin(i2cSlaveAddr);

    while(1)
    {
  		if(receive)
		{
		    handleEvent();
		}
	}
}

bool receive() {
    	if(TinyWireS.available())
    {
        for(int i=0; i<10 && TinyWireS.available(); i++)
        {
            	const uint8_t ddd = TinyWireS.receive();
            myArray[i] = ddd;
        }
        return true;
    }
    else
        return false;
}


bool isPinAllowed(int pin) {
  return true;
  // switch ( pin ) {
  //   case PB1:
  //     return true;
  //     break;
  //   case PB3:
  //     return true;
  //     break;
  //   case PB4:
  //     return true;
  //     break;
  // }
  // return false;
}

void handleEvent() {
  switch ( myArray[0] ) {
    case SERVO_ANGLE:
      {
        if ( servoPin != 0 ) {
          myservo.write(myArray[1]);
        }
      }
      break;
    case SERVO_ATTACH:
      servoPin = myArray[1];
      myservo.attach(myArray[1]);
      break;
    case SERVO_DETACH:
      {
        myservo.detach();
        servoPin = 0;
      }
      break;
    case RELAY_ATTACH:
      if ( isPinAllowed(myArray[1]) ) {
        relayPin = myArray[1];
        //pinMode(relayPin,OUTPUT);
      }
      break;
    case RELAY_DETACH:
      {
        relayPin = 0;
      }
      break;
    case RELAY_HIGH:
      if ( relayPin != 0 ) {        
        //digitalWrite(relayPin,HIGH);
        PORTB |= (1 << PB1);  //PB1 High
      }
      break;
    case RELAY_LOW:
      if ( relayPin != 0 ) {
        //digitalWrite(relayPin,LOW);
        PORTB &= ~(1 << PB1); //PB1 Low
      }
      break;
    default:
      break;
  }

}

