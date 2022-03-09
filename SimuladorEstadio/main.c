#include "io430.h"
//                      0    1    2    3    4    5    6    7    8    9
unsigned char cod[]= {0X3F,0X06,0X5B,0X4F,0X66,0X6D,0X7D,0X07,0X7F,0X6F};
#define MAXIMO 15       // Define a palavra MAXIMO como um valor fixo

/*------------------------------------------------------------------------------
;NOME:     TEMPO
;DESCRICAO: FAZ UMA CONTAGEM DE UM NUMERO PRE DEFINIDO.
;ENTRADA:   temp - NUMERO PRE DEFINIDO PARA CONTAGEM
;SAIDA:     -               
;------------------------------------------------------------------------------*/ 
void tempo(unsigned int temp)
{
while(temp!=0)      // Enquanto temp for diferente de zero ir� ser decrementado
temp=temp-1;
}

/*------------------------------------------------------------------------------
;NOME:     MOSTRA_DISPLAY
;DESCRICAO: APRESENTA EM DOIS DISPLAYS DE 7 SEGMENTOS O VALOR DA DEZENA E UNIDADE
DE QUALQUER N�MERO DE 2 D�GITOS DECIMAL
;ENTRADA:   valor - N�MERO EM HEXADECIMAL QUE SER� MOSTRADO
;SAIDA:     -                
;------------------------------------------------------------------------------ */ 
void mostra_display(unsigned char valor)
{
  
unsigned char unidade=valor,dezena=0;
while (unidade > 9)     // Separa o valor em dezenas e unidades
{
   dezena=dezena+1;
unidade=unidade-10;
}
    P2OUT= cod[unidade];        // Mostra as unidades no display das unidades
    P1OUT= P1OUT | 0X01;        // Acende display
    P2OUT= P2OUT & 0X7F;
    tempo(0X0500);
    P1OUT= P1OUT & 0XFE;        // Apaga display

    P2OUT= cod[dezena];         // Mostra as dezenas no display das dezenas
    P2OUT= P2OUT | 0X80;        // Acende display
    P1OUT= P1OUT & 0XFE;
    tempo(0X0500);
    P2OUT= P2OUT & 0X7F;        // Apaga display
}

/*------------------------------------------------------------------------------
;NOME:     CONFIG_DISPLAY
;DESCRICAO: CONFIGURA AS PORTAS P1 E P2 DO MSP430 COMO SAIDAS OU ENTRADAS
;ENTRADA:   -
;SAIDA:     -               
;------------------------------------------------------------------------------ */ 
void config_display(void)
{
P1SEL= 0X00;                // A��o necess�ria para o programa funcionar
P1SEL2= 0X00;
P2SEL= 0X00;
P2SEL2= 0X00;
P2DIR= P2DIR | 0XFF;        // Configura��o das portas como entradas ou sa�das
P1DIR= P1DIR & 0X07;        
P1DIR= P1DIR | 0X01;        
P1REN= P1REN | 0XF8;       // Habilita resistores internos do MSP
P1OUT= P1OUT | 0XF8;       // Configura como pull-up
P1OUT= P1OUT & 0XFE;       // Apaga displays
P2OUT= P2OUT & 0x00;
}

/*------------------------------------------------------------------------------
NOME: MONIT_CHAVES
DESCRI��O: MONITORA AS CHAVES DE SELE��O EXTERNAS
ENTRADAS: roletas - VALOR QUE IR� PARA A FUN��O MOSTRA_DISPLAY
SA�DAS: roletas - VALOR QUE IR� PARA A FUN��O MOSTRA_DISPLAY
------------------------------------------------------------------------------*/
void monit_chaves (int *roletas)
 
{
switch(P1IN)            // Dependendo da combina��o que estara nas chaves externas
  {                     // sera selecionado um dos casos abaixo
  case 0xF6: 
    mostra_display(roletas[1]);         // Nesse caso a fun��o mostra display
      break;                            // mostrar� o valor presente na primeira roleta
      
  case 0xEE:
    mostra_display(roletas[2]);         // Nesse caso a fun��o mostra display
      break;                            // mostrar� o valor presente na segunda roleta
      
  case 0xE6: 
    mostra_display(roletas[3]);         // Nesse caso a fun��o mostra display
      break;                            // mostrar� o valor presente na terceira roleta

  case 0xFE: 
    mostra_display(roletas[0]);         // Nesse caso a fun��o mostra display
    break;                              // mostrar� o valor da soma das roletas
  default:
      tempo(0XFFFF);
    break;
  }
}

/* Programa principal*/

int main( void )
{

  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

config_display();

  // cont[0] � referente a soma das roletas
  // cont[1] � referente a roleta 1 (cont 1)
  // cont[2] � referente a roleta 2 (cont 2)
  // cont[3] � referente a roleta 3 (cont 3)
int cont[4]={0, 0, 0, 0};


while(cont[0] < MAXIMO)         // Enquanto a soma total for menor que o valor
{                               // as chaves e as roletas serao monitoradas
monit_chaves(cont);             // Chama a fun��o que monitora as chaves
 
if((P1IN & 0X80)==0)            // Monitora se os bot�es foram apertados, se sim
{                               // incrementa sua contagem e a contagem total
  cont[1]++;
  cont[0]++;
  tempo(0XFFFF);
}

if((P1IN & 0X40)==0)
{
  cont[2]++;
  cont[0]++;
  tempo(0XFFFF);
}

if((P1IN & 0X20)==0) 
{
  cont[3]++;
  cont[0]++;
  tempo(0XFFFF);
}
}

/*------------------------------------------------------------------------------
TUDO A SEGUIR SE TRATA DO ALERTA DE LOCA��O
------------------------------------------------------------------------------*/

while(1){               // O alerta de loca��o permanecer� at� o programa ser reiniciado
int repet=0;
while(repet<70) {                // Mostrar� o alerta no display durante um tempo
    P2OUT= 0X38;
    P1OUT= P1OUT | 0X01;
    P2OUT= P2OUT & 0X7F;
    tempo(0X0500);
    P1OUT= P1OUT & 0XFE;
    P2OUT= P2OUT | 0X80;
    P1OUT= P1OUT & 0XFE;
    tempo(0X0500);
    P2OUT= P2OUT & 0X7F;
repet++;
}
repet=0;

while (repet<70) {              // Monitora as chaves e mostra no display o
    switch(P1IN)                // respectivo valor durante um tempo
  {                             // Funciona da mesma maneira que a fun��o monit_chaves
  case 0xF6: 
    if(cont[1]<MAXIMO) {
      mostra_display(cont[1]); }
      break;
      
  case 0xEE:
   if(cont[2]<MAXIMO) {
     mostra_display(cont[2]); }
      break;
      
  case 0xE6: 
   if(cont[3]<MAXIMO) {
     mostra_display(cont[3]); }
      break;

  case 0xFE:
    break;
    
  default:
      tempo(0XFFFF);
    break;
  }
  repet++; }
}
}

