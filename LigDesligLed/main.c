
#include "io430.h"

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

  P1DIR |=0X01;			//Configura P1.0 como saída
  P1DIR &=0XF7;			//Configura P1.3 como entrada
  P1REN |=0X08;                 //Habilita o resistor de pull-up/pull-down do P1.3
  P1OUT |=0X08;                 //Configura o resistor do P1.3 como pull-up
  while(1)
  {
	if (P1IN&0x08)
	  P1OUT|=0X01;
	else
	  P1OUT&=0XFE;
  }
  return 0;
}
