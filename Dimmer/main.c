/*
Prog03:
Elaborar um programa para o kit MSP430F1611 que funcione como um dimmer digital. Utilize as 
orientações de configuração dos registrados contidas na apostila (Prática 19).
Utilizaremos teclado e display a saber:
- Deve-se sempre exibir o ângulo de disparo e o estado de ativação
- Utilizaremos as teclas 
  "Entra" para ativar o dimmer
  "Anula" para desativar o dimmer
  "2" para aumemtar o angulo de disparo em incrementos unitários
  "5" para decrementar o angulo de disparo em decrementos unitários
  "3" para incrementos de 10 unidades de angulo
  "6" para decrementos de 10 unidades de angulo
  
  Tenha atenção aos valores de angulos de disparo mínimo e máximo.
  
  Obs: utilize o tempo de 50us para a duração do pulso de gate do triac.
  
*/


#include "io430.h"
#include "stdlib.h"
  
//Variaveis globais

   unsigned char TECLA=0;
   unsigned char ANGULO=170; // Variavel global
   char Ativado[] = "Ativado         " ; 
   char Desativado[] = "Desativado      "; 
   char limpa[] = "                ";
   char Angulo[] = "Angulo          ";
   char estado=0;


// Agrupe aqui seus defines

#define Fclk 750000
#define     RS BIT7
#define ENABLE BIT6
#define DISPLAY P5OUT 
#define DISPCTL P3OUT
#define _100us 100e-6*Fclk // calcula o num. de pulsos de clolck correspondem a um delay de 100 microsegundos
#define _2ms (int)(2e-3*Fclk) 
#define _5ms 5e-3*Fclk
#define _3s (3*Fclk) 
#define _10ms (10e-3 Flck)
#define _1ms (1e-3 Flck)


// Coloque aqui os protótipos da funções

void EnviaComando(char Comando);
void EnviaString(char *String);
void EnviaDado (unsigned char DADO);
void Envia(void);
void ConfigLCD(void);
void configuraTeclado(void);
unsigned char testar_colunas (unsigned char);
void config_timerA(void); 
void config_dimmer(void); 
void config_clock (void);
void Envia_Angulo (void);
void define_angulo (void);



//Criação da interrupção: interrompe o programa principal quando alguma tecla
// do teclado for pressionada

#pragma vector=PORT2_VECTOR
__interrupt void Teclado() 

{
   unsigned char TAB_TEC[] ={"\0""321654987E0A"};       //Vetor com os valores em ascii
   unsigned char APT_TEC=0;             //Apontador do vetor tab_tec
  __delay_cycles(_5ms);
   
    if ((P2IN & 0x07) == 0x00) {
      P2IFG = P2IFG & ~(BIT0+BIT1+BIT2);        //Reseta os flags de interrupção
      __enable_interrupt();
      return;
    }
    else 
    {
  P3OUT = P3OUT | 0x01;
  P3OUT = P3OUT & 0xF1;
    }
  
    if ((P2IN & 0x07) == 0x00) {        //Se a coluna 1 não é acionada, soma 3 no apt_tec
  P3OUT = P3OUT | 0x02;
  P3OUT = P3OUT & 0xF2;
  APT_TEC = APT_TEC + 3;
     }
    else
    {
      APT_TEC = testar_colunas (APT_TEC);       //Se a coluna é acionada, testa as colunas
        TECLA=TAB_TEC[APT_TEC];      
      return;
    }
      
    if ((P2IN & 0x07) == 0x00) {        //Se a coluna 2 não é acionada, soma 3 no apt_tec
  P3OUT = P3OUT | 0x04;
  P3OUT = P3OUT & 0xF4;
  APT_TEC = APT_TEC + 3;
     }
    else
    {
      APT_TEC = testar_colunas (APT_TEC);       //Se a coluna é acionada, testa as colunas
        TECLA=TAB_TEC[APT_TEC];      
      return;
    }
     
    if ((P2IN & 0x07) == 0x00) {        //Se a coluna 3 não é acionada, soma 3 no apt_tec
  P3OUT = P3OUT | 0x08;
  P3OUT = P3OUT & 0xF8;
  APT_TEC = APT_TEC + 3;
     }
    else
    {
      APT_TEC = testar_colunas (APT_TEC);       //Se a coluna é acionada, testa as colunas
        TECLA=TAB_TEC[APT_TEC];      
      return;
    }
     
    if ((P2IN & 0x07) == 0x00) {                //Se não tem colunas acionadas, reseta os flags
                                                // de interrupção
          TECLA=TAB_TEC[APT_TEC];
          P3OUT = P3OUT | 0x0F;
          P2IFG = P2IFG & ~(BIT0+BIT1+BIT2);
     }
    else
    {
        APT_TEC = testar_colunas (APT_TEC);       //Se a coluna é acionada, testa as colunas
          TECLA=TAB_TEC[APT_TEC]; 
        return;
    }
  }


//Função principal
//Define qual é a função que deverá ser realizada de acordo com a tecla pressionada

int main( void )
{
  WDTCTL = WDTPW + WDTHOLD; // Desabilita WDT
  
ConfigLCD ();   //Configura LCD
configuraTeclado ();      // Configura teclado
config_dimmer ();        //Configura motor
config_clock ();
config_timerA ();
asm ("EINT");
EnviaComando(0x80);
EnviaString(Angulo);
Envia_Angulo();
EnviaComando(0xC0);
EnviaString (Desativado); 

while (1) {
define_angulo();
}
}

//Função: Dimmer
//Responsável pela interrupção do timer A, variar o angulo de disparo
#pragma vector=TIMERA1_VECTOR
__interrupt void dimmer ()
{
   TACCR1 = TACCR0 + (ANGULO*185);
   TACCR0 = TACCR1 + 200;
   TACCTL0&=~CCIFG;
 }

void define_angulo ()
{
    if (TECLA!=0) {
    if (TECLA == 'E') {         //Caso aperte E, ativa o dimmer
       if (estado != 1) 
       {
       estado = 1;
       ANGULO = 10;
       EnviaComando(0x8B);
       Envia_Angulo();
       EnviaComando(0xC0);
       EnviaString (Ativado);   
       }
    }
    if (TECLA == 'A') {         //Caso aperte A, desativa o dimmer
       if (estado != 0) 
       {
       estado = 0;
       ANGULO = 170;
       EnviaComando(0x8B);
       Envia_Angulo();
       EnviaComando(0xC0);
       EnviaString (Desativado);   
       }
    }
    if (TECLA == '2') {         //Caso aperte 2, angulo de disparo é incrementado
        if (ANGULO <= 170)
       {
       ANGULO = ANGULO + 1;
       EnviaComando(0x8B);
       Envia_Angulo();  
       }
    }
    if (TECLA == '5') {         //Caso aperte 5, angulo de disparo é decrementado
        if (ANGULO >= 10)
       {
       ANGULO = ANGULO - 1;
       EnviaComando(0x8B);
       Envia_Angulo();  
       }
    }
    if (TECLA == '3') {         //Caso aperte 3, angulo de disparo é incrementado em 10 unidades
        if (ANGULO <= 160)
       {
       ANGULO = ANGULO + 10;
       EnviaComando(0x8B);
       Envia_Angulo(); 
    }
    }
    if (TECLA == '6') {         //Caso aperte 6, angulo de disparo é decrementado em 10 unidades
        if (ANGULO >= 20)
       {
       ANGULO = ANGULO - 10;
       EnviaComando(0x8B);
       Envia_Angulo();  
       }
    }
    TECLA=0;            //Zera a variável tecla
  }
}

//Função: Envia_Angulo
//Envia o angulo para o display
void Envia_Angulo ()
{
  EnviaString(limpa);
  unsigned int centena = 0;
  unsigned int dezena = 0;
  unsigned int unidade = 0;
  unsigned int angulo=ANGULO;
  while (angulo>=100)
  {
    angulo = angulo - 100;
    centena = centena + 1;
  }
  while (angulo>=10)
  {
    angulo = angulo -10;
    dezena = dezena +1;
  }
    angulo = unidade;
  
  if (centena!=0)
  {
  EnviaComando(0x8B);
  EnviaDado(centena + 0x30);
  }
  if (dezena!=0)
  {
  EnviaComando(0x8C);
  EnviaDado(dezena + 0x30);
  }
  if (unidade!=0)
  {
  EnviaComando(0x8D);
  EnviaDado(unidade + 0x30);
  }
  EnviaComando(0x80);  
  EnviaString(Angulo);

}

//Função: ConfiguraClock
//Configuração inicial do clock */
void config_clock()
{
 BCSCTL1 &=~XT2OFF;
 BCSCTL2 |= SELS;
}

//Função: ConfiguraTimerA
//Configuração inicial do timer a
void config_timerA() 
{
 TACCTL0 = CM_1 + CCIS_0 + SCS + CAP + CCIE;
 TACCTL1 = OUTMOD_3;
 TACTL = TASSEL_2 + MC_2 + TACLR;
}

//Função: config_dimmer
//Configura as saídas do MSP que vão pro dimmer
void config_dimmer (void)
{
{
P1SEL |= ( BIT1 + BIT2 );
P1DIR &=~ BIT1;
P1DIR |= BIT2;
}
}



//Função: configuraTeclado
//Configura IO do teclado
void configuraTeclado (void)
{
  P3DIR = P3DIR | (BIT0+BIT1+BIT2+BIT3);
  P3SEL = P3SEL & ~(BIT0+BIT1+BIT2+BIT3);
  P3OUT = P3OUT | (BIT0+BIT1+BIT2+BIT3);
  P2DIR = P2DIR & ~(BIT0+BIT1+BIT2);
  P2SEL = P2SEL & ~(BIT0+BIT1+BIT2);
  P2IES = P2IES & ~(BIT0+BIT1+BIT2);
  P2IFG = P2IFG & ~(BIT0+BIT1+BIT2);
  P2IE = P2IE | (BIT0+BIT1+BIT2);
  
}


// Nome: ConfigLCD
// Configuração inicial do display LCD
void ConfigLCD(void)
{
  // Configuração dos pinos de IO utilizados pelo display LCD
  P5DIR = 0XFF;         // configura toda a P5 como saída
  P5SEL = 0x00;         // Configura toda a P5 como I/O
  P3DIR = P3DIR | (BIT6+BIT7);   // configura bits 6 e 7 como saídas
  P3SEL = P3SEL & ~(BIT6+BIT7);
    
  EnviaComando(0x38);
  __delay_cycles(_5ms);
  
  EnviaComando(0x0E);
  EnviaComando(0x06);
  EnviaComando(0x01);
  __delay_cycles(_2ms);
  
}



// Funçao EnviaComando
// Envia um byte de configuração para o display
void EnviaComando(char Comando)
{
  DISPLAY = Comando;   //ou P5OUT = Comando
  DISPCTL = DISPCTL & ~RS;  // executa o clear bit do pino RS (P3.7)
  // ou pode ser escrito da seguinte forma
  // DISPCTL &= ~RS;
  Envia();
  
}

// Funçao EnviaDado
// Envia alguma informação para ser mostrada no display
void EnviaDado(unsigned char DADO)
{
  DISPLAY = DADO;   //ou P5OUT = Comando
  DISPCTL = DISPCTL | RS;  // executa o set bit do pino RS (P3.7)
  Envia();
  
}

//Função: EnviaString
//Envia uma frase para o display
void EnviaString(char *String)
{
    while(*String) 
      {
        EnviaDado(*String);
        String++;
      }
  }



// Função: Envia
// Gera o pulso do pino de Enable do Display
void Envia(void)
{
  DISPCTL = DISPCTL & ~ENABLE;   // clear bit Enable
  DISPCTL = DISPCTL | ENABLE;    // set bit Enable
  DISPCTL = DISPCTL & ~ENABLE;   // clear bit Enable
  __delay_cycles(_100us);
  
}

//Função: testar_colunas
// Le a tecla que foi pressionada no teclado e envia para o display

unsigned char testar_colunas(unsigned char APT_TEC) {
   
        if ((P2IN&0x07)==0x01) 
        {
     
          APT_TEC = APT_TEC + 1;        //Se a coluna 1 é acionada soma 1 no apt_tec
          P3OUT = P3OUT | 0x0F;
          P2IFG = P2IFG &~ (BIT0+BIT1+BIT2);
        }
          
        else if ((P2IN&0x07)==0x02) 
        {
          APT_TEC = APT_TEC + 2;        //Se a coluna 2 é acionada soma 1 no apt_tec
          P3OUT = P3OUT | 0x0F;
          P2IFG = P2IFG &~ (BIT0+BIT1+BIT2);
        }
          
        else if ((P2IN&0x07)==0x04)
        {
          APT_TEC = APT_TEC + 3;        //Se a coluna 3 é acionada soma 1 no apt_tec
          P3OUT = P3OUT | 0x0F;
          P2IFG = P2IFG &~ (BIT0+BIT1+BIT2);
        }
          
        else
        {
          P3OUT = P3OUT | 0x0F;         //Se não, zera os flags de interrupção
          P2IFG = P2IFG &~ (BIT0+BIT1+BIT2);
        } 
        return APT_TEC;         // Retorna o valor de apt_tec
}