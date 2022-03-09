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
while(temp!=0)      // Enquanto temp for diferente de zero irá ser decrementado
temp=temp-1;
}

/*------------------------------------------------------------------------------
;NOME:     MOSTRA_DISPLAY
;DESCRICAO: APRESENTA EM DOIS DISPLAYS DE 7 SEGMENTOS O VALOR DA DEZENA E UNIDADE
DE QUALQUER NÚMERO DE 2 DÍGITOS DECIMAL
;ENTRADA:   valor - NÚMERO EM HEXADECIMAL QUE SERÁ MOSTRADO
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
P1SEL= 0X00;                // Ação necessária para o programa funcionar
P1SEL2= 0X00;
P2SEL= 0X00;
P2SEL2= 0X00;
P2DIR= P2DIR | 0XFF;        // Configuração das portas como entradas ou saídas
P1DIR= P1DIR & 0X07;        
P1DIR= P1DIR | 0X01;        
P1REN= P1REN | 0XF8;       // Habilita resistores internos do MSP
P1OUT= P1OUT | 0XF8;       // Configura como pull-up
P1OUT= P1OUT & 0XFE;       // Apaga displays
P2OUT= P2OUT & 0x00;
}

/*------------------------------------------------------------------------------
NOME: MONIT_CHAVES
DESCRIÇÃO: MONITORA AS CHAVES DE SELEÇÃO EXTERNAS
ENTRADAS: roletas - VALOR QUE IRÁ PARA A FUNÇÃO MOSTRA_DISPLAY
SAÍDAS: roletas - VALOR QUE IRÁ PARA A FUNÇÃO MOSTRA_DISPLAY
------------------------------------------------------------------------------*/
void monit_chaves (int *roletas)
 
{
switch(P1IN)            // Dependendo da combinação que estara nas chaves externas
  {                     // sera selecionado um dos casos abaixo
  case 0xF6: 
    mostra_display(roletas[1]);         // Nesse caso a função mostra display
      break;                            // mostrará o valor presente na primeira roleta
      
  case 0xEE:
    mostra_display(roletas[2]);         // Nesse caso a função mostra display
      break;                            // mostrará o valor presente na segunda roleta
      
  case 0xE6: 
    mostra_display(roletas[3]);         // Nesse caso a função mostra display
      break;                            // mostrará o valor presente na terceira roleta

  case 0xFE: 
    mostra_display(roletas[0]);         // Nesse caso a função mostra display
    break;                              // mostrará o valor da soma das roletas
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

  // cont[0] é referente a soma das roletas
  // cont[1] é referente a roleta 1 (cont 1)
  // cont[2] é referente a roleta 2 (cont 2)
  // cont[3] é referente a roleta 3 (cont 3)
int cont[4]={0, 0, 0, 0};


while(cont[0] < MAXIMO)         // Enquanto a soma total for menor que o valor
{                               // as chaves e as roletas serao monitoradas
monit_chaves(cont);             // Chama a função que monitora as chaves
 
if((P1IN & 0X80)==0)            // Monitora se os botões foram apertados, se sim
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
TUDO A SEGUIR SE TRATA DO ALERTA DE LOCAÇÃO
------------------------------------------------------------------------------*/

while(1){               // O alerta de locação permanecerá até o programa ser reiniciado
int repet=0;
while(repet<70) {                // Mostrará o alerta no display durante um tempo
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
  {                             // Funciona da mesma maneira que a função monit_chaves
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

