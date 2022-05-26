#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>

#include "HD44780.h"

uint64_t counter;
uint32_t distance;
char buffer[33];

void wait_ms(uint32_t ms);
void wait_ten_us();

int main()
{
  DDRB |= (1 << PB1) | (1 << PB0);

  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);
 
  TIMSK0 |= (1 << TOIE0);
  TCCR0A = 0;

  EICRA |= (1 << ISC00) | (1 << ISC01);
  EIMSK |= (1 << INT0);
  sei();

  LCD_Initalize();

  while(1)
  {  
    PORTB |= (1 << PB0);
    _delay_us(10);
    PORTB &= ~(1 << PB0);

    itoa(distance, buffer, 10);
    LCD_Clear();
    LCD_GoTo(0, 0);
    LCD_WriteText(buffer);
    _delay_ms(200);
  }
}

void wait_ms(uint32_t ms)
{
  uint32_t n = 0;
  TCCR2A |= (1 << WGM21);

  while(1)
  {
    OCR2A = 250;
    TCCR2B |= (1 << CS22);

    while (!(TIFR2 & (1 << OCF2A)));
    
    TCCR2B &= ~(1 << CS22);
    TIFR2 |= (1 << OCF2A);
    
    if(n == ms)
    {
      n = 0;
      break;
    }
    else
    {
      n++;
    }
  }
}

void wait_ten_us()
{
    uint32_t n = 0;
    TCCR2A |= (1 << WGM21);
    
    while(1)
    {
      OCR2A = 160;
      TCCR2B |= (1 << CS20);

      while (!(TIFR2 & (1 << OCF2A)));
      
      TCCR2B &= ~(1 << CS20);
      TIFR2 |= (1 << OCF2A);
      
      if(n == 100000)
      {
        n = 0;
        break;
      }
      else
      {
        n++;
      }
    }
}

ISR(TIMER0_OVF_vect)
{
  counter++;
}

ISR(INT0_vect)
{
  if(EICRA & (1 << ISC00))
  {
     TCCR0B |= (1 << CS00);
     EICRA &= ~(1 << ISC00);
  }
  else
  {
    EICRA |= (1 << ISC00);
    distance = (counter*16)/58;
    TCCR0B &= ~(1 << CS00);
    TCNT0 = 0;
    counter = 0;
  }
}
