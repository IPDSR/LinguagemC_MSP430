/* Proposta de projeto: Sinal inclusivo
Este projeto consiste em um sinal de transito, representados por leds verde, vermelho e amarelo. Há também o sinal de pedestres com os leds verde e vermelho. A simultaniedade ocorre de forma temporizada.
Caso o sinal para pedestres estiver aberto, um aviso sonoro (buzzer) indicará a um deficiente visual que a passagem é possível.
Além disso, o sinal de carros conta com um sensor de ultrapassagens que aplica uma multa ao carro que passao enquanto o sinal estiver fechado.
O projeto utiliza o micro controlador MSP 430 para acionar um display, 5 leds, um buzzer e controlar um sensor de movimento.

---------------------------------------------------------------------------------------------------------------------------------------------
*/

#include "io430.h"      //Incli a biblioteca do msp 430
#include "stdlib.h"     //Inclui a biblioteca de instrucoes matemáticas
#include "stdio.h"      //Inclui a biblioteca de instrucoes print


// Insira abaixo os defines que não depemdem do tipo de processador --------------------------------------------------------------------------

#define _100us 100e-6*Fclk // calcula o num. de pulsos de clolck correspondem a um delay de 100 microsegundos
#define _2ms (int)(2e-3*Fclk) //Define o numero necessário para 2ms
#define _5ms 5e-3*Fclk          //Define o numero necessário para 5ms
#define _3s (3*Fclk)            //Define o numero necessário para 3s
#define _10ms (10e-3 Flck)      //Define o numero necessário para 10ms
#define _1ms (1e-3 Flck)        //Define o numero necessário para 1ms

// -------------------------------------------------------------------------------------------------------------------------------------------


// Crie abaixo as variáveis globais. ---------------------------------------------------------------------------------------------------------
   unsigned char TECLA=0;        //Variavel global do valor da tecla
   int timer =35;
   char AUTOMOVEIS[] = "   AUTOMOVEIS   ";
   char PEDESTRES[] = "   PEDESTRES    ";
   char ATENCAO[] = "    ATENCAO     ";
   char SEGUNDOS[] = "        s       ";
   char TIMER[2];
   char LIMPA[] = "                ";   //Frase para limpar o display
   
 // ------------------------------------------------------------------------------------------------------------------------------------------  
   
// Coloque aqui os protótipos da funções -----------------------------------------------------------------------------------------------------

void EnviaComando(unsigned char);        //prototipo da funcao enviacomando
void EnviaString(char *String);         //prototipo da funcao enviastring
void EnviaDado (unsigned char);    //prototipo da funcao enviadado
void Envia(unsigned char);                       //prototipo da funcao envia
void ConfigLCD(void);                   //prototipo da funcao configlcd
void Init_LCD(void);            //prototipo da funcao que inicializa o lcd
void configuraTeclado(void);             //prototipo da funcao configurateclado
unsigned char testar_colunas (unsigned char);   //prototipo da funcao testarcolunas
void ler_tecla(void);                   //prototipo da funcao lertecla
void ler_pino(void);                    //prototipo da funcao lerpino
void config_io (void);                  //prototipo da funcao configio
void envia_meio_comando(unsigned char palavra); //prototopo da funcao enviameiocomando
void config_watchdog_timer (void);      //prototipo da funcao configwatchdogtimer
void config_timerA (void);              //prototipo da funcao configtimerA

//----------------------------------------------------------------------------------------------------------------------------------------------
 
#if defined (__MSP430F1611__)
#include "Lib_F1611.h"        // Biblioteca de funções do kit F1611

#elif defined (__MSP430F149__)
#include "Lib_F149.h"         // Biblioteca de funções do kit F149

#elif defined (__MSP430G2553__)
#include "Lib_G2553.h"         // Biblioteca de funções do kit G2553

#else
#error "****** Voce esqueceu de selecionar o processador adequado. *****"
#endif

/*------------------------------------------------------------------------------------------------------------------------------------------------------
Função principal
Chama as outras funções, habilita a interrupção e realiza algumas ações iniciais */

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
ConfigLCD ();                      //Configura display LCD
Init_LCD();                       //Inicializa o display LCD
configuraTeclado ();             // Configura teclado matricial de 12 teclas
config_io();                //configura as entradas e saídas
config_watchdog_timer();        //configura o timer de 1 segundo no watchdog
config_timerA();
asm ("EINT");                   //Habilita interrupção

EnviaComando(0x01);           // limpa display e retorna o cursor para 1 posição 
__delay_cycles(_2ms);        //Apaga os nomes do display

while (1)       //Loop das funções ler tecla e ler pino
{
  ler_pino();  //Chama a função que lê o pino e mostra no display
}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------------------------
/*Funcao config_timer A
Configura um PWM que aciona o buzzer com frequencia de 1 Hz e ciclo de trabalho
50% usando timer A*/
void config_timerA ()
{
TACCTL1 = 0x0060; //CM_0 + CCIS_0 + OUTMOD_3
TACCR0 = 32768; //Frequencia de 1 Hz
TACCR1 = 16384; //Ciclo de trabalho 50%
TACTL = 0x0104; //TACLR + MC_0 + TASSEL_0
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------------------------
/*Funcao config_watchdog_timer
Configura uma base de tempo 1 segundo utilizando o watchdog*/
void config_watchdog_timer ()
{
WDTCTL = WDTSSEL + WDTCNTCL + WDTTMSEL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------------------------
/*Interrupcao do watchdog timer
Responsável pela temporização do semáforo, ou seja, acende os leds, durante o
tempo correto*/
#pragma vector=WDT_VECTOR
__interrupt void semaforo ()
{
  if (timer >= 35 )
  {
    P1OUT = P1OUT | (RED1);
    P1OUT = P1OUT & ~(RED + GREEN1);
    P2OUT = P2OUT | GREEN;
    P2OUT = P2OUT & ~YELLOW;
    EnviaComando(0x80);
    EnviaString(AUTOMOVEIS);
  }
  if (timer >= 30)
  {
    P2OUT = P2OUT | (YELLOW);
    P2OUT = P2OUT & ~GREEN;
    P1OUT = P1OUT & ~(RED + GREEN1);
    EnviaComando(0x80);
    EnviaString(ATENCAO);
  }
  if (timer>=0)
  {
    if(timer<=5)
    {
      P1OUT = P1OUT ^RED1;
      TACTL = 0x0104; //TACLR + MC_0 + TASSEL_0
      P1OUT = P1OUT | BUZZER;
    }
    else
    {
    P1OUT = P1OUT | (RED + GREEN1);
    P1OUT = P1OUT & ~(RED1);
    P2OUT = P2OUT & ~(GREEN + YELLOW);
    EnviaComando(0x80);
    EnviaString(PEDESTRES);
    TACTL = 0x0114; //TACLR + MC_1 + TASSEL_0
    }
  }
  else
  {
    timer=65;
  }
  EnviaComando(0xC0);
  EnviaString(SEGUNDOS);
  sprintf(TIMER, "%2i", timer);
  EnviaComando(0xC6);
  EnviaString(TIMER);
  timer--;
}


//-----------------------------------------------------------------------------
//NOME: Init_LCD
//FUNÇÃO: inicializa o display
//ENTRADA: -
//SAIDA: -
//----------------------------------------------------------------------------

void Init_LCD()
{
  __delay_cycles(_50ms);
  envia_meio_comando(0x30);      // Envia apenas o nibble superior 0011 por 3 vezes seguidas
  __delay_cycles(_5ms);
  envia_meio_comando(0x30);
  __delay_cycles(_100us);
  envia_meio_comando(0x30);

  envia_meio_comando(0x20);      // Envia apenas o nibble superior 0010

  EnviaComando(0x28);           // 8 bits de dados- duas linhas- matriz 5x8
  __delay_cycles(_100us); 
  EnviaComando(0x0C);           //display e cursor ativos sem piscar
  __delay_cycles(_100us);  
  EnviaComando(0x06);           // deslocamento cursor para direita
  __delay_cycles(_100us);  
  EnviaComando(0x01);           // limpa display e retorna o cursor para 1 posição 
  __delay_cycles(_2ms); 
}

//----------------------------------------------------------------------------
//NOME: envia_meio_comando
//FUNÇÃO: Faz RS = 0 e envia o nibble superir para o display que será interpretado
//        como comando
//        Esta função é utilizada na inicialzação do display com a interface de
//        4 bits. Neste caso o nibble 0010 deve ser enviado uma vez sozinho e
//        depois combinado com o nibble inferior. Este último envio é feito
//        com a função envia_comando já existente.
//ENTRADA: palavra
//SAIDA: -
//----------------------------------------------------------------------------
void envia_meio_comando(unsigned char palavra)
{
  LCDCtl_ie &= ~TecCols;                        // Desabilita as INTs das colunas do teclado
  LCDCtl &= ~LCDRS;                         // Faz RS = 0 = comando

  // Envia apenas o nibble superior
  LCDPort = (LCDPort & 0x0F) + (palavra & 0xF0);
  LCDCtl &= ~LCDEn;           //Gera pulso de nível alto no pino Enable do display
  LCDCtl |= LCDEn;
  LCDCtl &= ~LCDEn;
  __delay_cycles(_100us);
  LCDCtl_ifg &= ~TecCols;                       // Reseta os flags de INT que podem ter sido ativados
  LCDCtl_ie |= TecCols;                         // Rehabilita as INTs das colunas do teclado

  // Restabelece os 4 bits das linhas do teclado com nível alto.
  LCDPort |= TecLins;

}

/*----------------------------------------------------------------------------------------------------------------------------------------------
Funcao ler_pino
Le o valor que estará no pino definido e mostra na segunda linha do display
e coloca o valor invertido em outro pino*/


void ler_pino (void)            //Inicio da funcao ler pino
{
  EnviaComando(0xC8);            //Seleciona a oitava posicao da segunda linha do display
  if ((P1IN & 0x08)==0x00)      //Se o valor lido for zero
  {
    EnviaDado ('0');            //Envia o zero para ser mostrado no display
    P1OUT = P1OUT | 0X01;       //Coloca 1 na saída do pino 0
  }
  else                          //Se o valor lido foi 1
  {
    EnviaDado('1');             //Envia o um para ser mostrado no display
    P1OUT = P1OUT & 0xFE;       //Coloca 0 na saída do pino 0

  }
      __delay_cycles(_2ms);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------

/*-----------------------------------------------------------------------------------------------------------------------------------------------------
Funcao config_io
Configura os pinos do msp utilizados como entrada ou saida*/

void config_io (void)
{
  P1DIR = P1DIR | 0x0F;         //Configura pinos
  P2DIR = P2DIR | 0x0C;         //Configura pinos
  P2DIR = P2DIR & 0xEF;         //Configura pinos
  P1SEL = P1DIR | BUZZER;
  P1SEL = P1SEL & ~(RED + GREEN1 + RED1);
  P2SEL = P2SEL & ~(GREEN + YELLOW + SENSOR);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------

/*-----------------------------------------------------------------------------------------------------------------------------------------------------
Função: configuraTeclado
Configura IO do teclado*/

void configuraTeclado (void)
{
  TecLin_dir = TecLin_dir | (TecL1+TecL2+TecL3+TecL4);        //Configura os 4 primeiros pinos da porta como saidas
  TecLin_sel = TecLin_sel & ~(TecL1+TecL2+TecL3+TecL4);       //Configura os 4 primeiros pinos da porta como normais
  TecLin = TecLin | (TecL1+TecL2+TecL3+TecL4);        //Ativa os 4 primeiros pinos da porta
  TecCol_dir = TecCol_dir & ~(TecC1+TecC2+TecC3);            //Configura os 3 primeiros pinos como entradas
  TecCol_sel = TecCol_sel & ~(TecC1+TecC2+TecC3);            //Configura os 3 primeiros pinos como normais
  
  P2IES = P2IES & ~(TecC1+TecC2+TecC3);            //Desabilita interrupçao
  P2IFG = P2IFG & ~(TecC1+TecC2+TecC3);            //Reseta os flags de interrupcao
  P2IE = P2IE | (TecC1+TecC2+TecC3);               //Habilita a interrupcao
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------

/*-----------------------------------------------------------------------------------------------------------------------------------------------------
Função: EnviaString
Envia uma frase para o display*/

void EnviaString(char *String)
{
    while(*String)      //Enquanto existirem caracteres na frase
      {
        EnviaDado(*String);     //Envia um caracter por vez da frase
        String++;               //Passa para o proximo caracter
      }
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------



