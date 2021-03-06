#include "HardPWM.h"

void HardPWM::write(uint8_t channel, uint16_t value) {
  if(channel >= pwmCount) {
    return;
  }
  
  m_pwmValues[channel] = value;

  cli();
  switch(channel) {
    case 0:
      OCR1C = value >> 2;
      break;
    case 1:
      TC4H = (value >> 14) & 0x03;
      OCR4D = (value >> 6) & 0xFF;
      break;
    case 2:
      OCR1A = value >> 2;
      break;
    case 3:
      OCR1B = value >> 2;
      break;
    case 4:
      OCR3A = value >> 2;
      break;
    case 5:  //Note: This PWM output is 10-bit
      TC4H = (value >> 14) & 0x03;
      OCR4A = (value >> 6) & 0xFF;
      break;
  }
  sei();
}

uint16_t HardPWM::read(uint8_t channel) {
  if(channel >= pwmCount) {
    return 0;
  }
  
  return m_pwmValues[channel];
}

void HardPWM::begin() {
  cli();
  
    // Configure timer1 for 14-bit, Phase and Frequency Correct PWM operation at 488Hz
    // Note that this timer is capable of up to 16-bit operation, 
    bitSet(DDRB, 7);  // OCR1C, PB7 (PWM0)
    bitSet(DDRB, 5);  // OCR1A, PB5 (PWM2)
    bitSet(DDRB, 6);  // OCR1B, PB6 (PWM3)
  
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (0 << COM1B0) | (1 << COM1C1) | (0 << COM1C0) | (0 << WGM11) | (0 << WGM10);
    TCCR1B = (1 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
    ICR1 = 0x3FFF;
  
    // Configure timer3 for 14-bit, Phase and Frequency Correct PWM operation at 488Hz
    bitSet(DDRC, 6);  // OCR3A, PC6 (PWM4)
  
    TCCR3A = (1 << COM3A1) | (0 << COM3A0) | (0 << WGM31) | (0 << WGM30);
    TCCR3B = (1 << WGM33) | (0 << WGM32) | (0 << CS32) | (0 << CS31) | (1 << CS30);
    ICR3 = 0x3FFF;
  
    // configure timer4 for 10-bit, Phase and Frequency Correct PWM operation at 488Hz
    bitSet(DDRD, 7);  // OCR4D, PD7 (PWM1)
    bitSet(DDRC, 7);  // OCR4A, PC7 (PWM5)

    TCCR4C = (1 << COM4D1) | (0 << COM4D0) | (1 << PWM4D); // Note: set TCCR4C before TCCR4A because of shadow bits.
    TCCR4A = (1 << COM4A1) | (0 << COM4A0) | (1 << PWM4A) | (0 << PWM4B);
    TCCR4B = (0 << CS43) | (1 << CS42) | (0 << CS41) | (1 << CS40);
    TCCR4D = (0 << WGM41) | (1 << WGM40);
  
    TC4H = 0x03;
    OCR4C = 0xFF;  // OCR4C contains the TOP value

  sei();
  
  // Clear the initial output values
  for(uint8_t i = 0; i < pwmCount; i++) {
    write(i, 0);
  }
}
