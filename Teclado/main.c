
#include "io430.h"
  
//Variaveis globais
   unsigned char TECLA=0;

// Agrupe aqui seus defines
#define Fclk 750000
#define     RS BIT7
#define ENABLE BIT6
#define DISPLAY P5OUT 
#define DISPCTL P3OUT
#define _100us 100e-6*Fclk // calcula o num. de pulsos de clolck correspondem a um delay de 100 microsegundos
#define _2ms (2e-3*Fclk) 
#define _5ms 5e-3*Fclk
#define _3s (3*Fclk) 
#define _10ms (10e-3 Flck)


// Coloque aqui os protótipos da funções
void EnviaComando(char Comando);
void EnviaString(char *String);
void EnviaDado (unsigned char DADO);
void Envia(void);
void ConfigLCD(void);
void configuraTeclado(void);
unsigned char testar_colunas (unsigned char);



//Criação da interrupção: interrompe o programa principal quando alguma tecla
// do teclado for pressionada

#pragma vector=PORT2_VECTOR
__interrupt void Teclado() 

{
   unsigned char TAB_TEC[] ={"\0""321654987E0A"};
   unsigned char APT_TEC=0;
  __delay_cycles(_5ms);
   
    if ((P2IN & 0x07) == 0x00) {
      P2IFG = P2IFG & ~(BIT0+BIT1+BIT2);
      return;
    }
    else 
    {
  P3OUT = P3OUT | 0x01;
  P3OUT = P3OUT & 0xF1;
    }
  
    if ((P2IN & 0x07) == 0x00) {
  P3OUT = P3OUT | 0x02;
  P3OUT = P3OUT & 0xF2;
  APT_TEC = APT_TEC + 3;
     }
    else
    {
      APT_TEC = testar_colunas (APT_TEC);
        TECLA=TAB_TEC[APT_TEC];      
      return;
    }
      
    if ((P2IN & 0x07) == 0x00) {
  P3OUT = P3OUT | 0x04;
  P3OUT = P3OUT & 0xF4;
  APT_TEC = APT_TEC + 3;
     }
    else
    {
      APT_TEC = testar_colunas (APT_TEC);
        TECLA=TAB_TEC[APT_TEC];      
      return;
    }
     
    if ((P2IN & 0x07) == 0x00) {
  P3OUT = P3OUT | 0x08;
  P3OUT = P3OUT & 0xF8;
  APT_TEC = APT_TEC + 3;
     }
    else
    {
      APT_TEC = testar_colunas (APT_TEC);
        TECLA=TAB_TEC[APT_TEC];      
      return;
    }
     
    if ((P2IN & 0x07) == 0x00) {
       
          TECLA=TAB_TEC[APT_TEC];
          P3OUT = P3OUT | 0x0F;
          P2IFG = P2IFG & (BIT0+BIT1+BIT2);
     }
    else
    {
        APT_TEC = testar_colunas (APT_TEC);
          TECLA=TAB_TEC[APT_TEC];        
    }
  }


     
int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
ConfigLCD ();
configuraTeclado ();
asm ("EINT");

while (1) {
  if (TECLA!=0) {
    EnviaDado(TECLA);
    TECLA=0;
    }
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
     
          APT_TEC = APT_TEC + 1;
          P3OUT = P3OUT | 0x0F;
          P2IFG = P2IFG &~ (BIT0+BIT1+BIT2);
        }
          
        else if ((P2IN&0x07)==0x02) 
        {
          APT_TEC = APT_TEC + 2;
          P3OUT = P3OUT | 0x0F;
          P2IFG = P2IFG &~ (BIT0+BIT1+BIT2);
        }
          
        else if ((P2IN&0x07)==0x04)
        {
          APT_TEC = APT_TEC + 3;
          P3OUT = P3OUT | 0x0F;
          P2IFG = P2IFG &~ (BIT0+BIT1+BIT2);
        }
          
        else
        {
          P3OUT = P3OUT | 0x0F;
          P2IFG = P2IFG &~ (BIT0+BIT1+BIT2);
        } 
        return APT_TEC;
}
