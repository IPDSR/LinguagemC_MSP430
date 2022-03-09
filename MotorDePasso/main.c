


#include "io430.h"
#include "stdlib.h"
  
//Variaveis globais

unsigned char TECLA=0;
//char velocidade [] = {"123456789"};
//char apt_vel = 4;
int GIRO;


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
#define Parado 0
#define Esquerda -1
#define Direita 1


// Coloque aqui os protótipos da funções

void EnviaComando(char Comando);
void EnviaString(char *String);
void EnviaDado (unsigned char DADO);
void Envia(void);
void ConfigLCD(void);
void configuraTeclado(void);
unsigned char testar_colunas (unsigned char);
void config_motor (void);
void Passo (int);
void nDelay (int);



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
          P2IFG = P2IFG & (BIT0+BIT1+BIT2);
     }
    else
    {
        APT_TEC = testar_colunas (APT_TEC);       //Se a coluna é acionada, testa as colunas
          TECLA=TAB_TEC[APT_TEC];        
    }
  }


//Função principal
//Define qual é a função que deverá ser realizada de acordo com a tecla pressionada

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
char esquerda[] = "Esquerda        ";   // Strings que serão enviadas para o display
char direita [] = "Direita         ";
char parado [] = "Parado          ";
char velocidade[] = "  Velocidade    ";
char indicador = '5';

  
ConfigLCD ();   //Configura LCD
configuraTeclado ();      // Configura teclado
config_motor ();        //Configura motor
asm ("EINT");
EnviaString (velocidade);       //Define velocidade inicial
EnviaComando(0xC0);     //Define estado inicial
EnviaString (parado);
EnviaComando(0x80);
EnviaDado (indicador);


while (1) {
  if (TECLA!=0) {
    if (TECLA == '5') {         //Caso aperte 5, motor fica parado
       if (GIRO != Parado) 
       {
       GIRO = Parado;
       EnviaComando(0xC0);
       EnviaString (parado);   
       }
    }
    if (TECLA == '2') {         //Caso aperte 2, velocidade aumenta
       if (indicador < '9')
       {
       indicador = indicador + 1;
       //indicador = velocidade [apt_vel];
       EnviaComando(0x80);
       EnviaDado (indicador);
      }
    }
    if (TECLA == '8') {         //Caso aperte 8, velocidade diminui
        if (indicador > '1')
       {
       indicador = indicador - 1;
       EnviaComando(0x80);
       EnviaDado (indicador);   
       }
    }
    if (TECLA == '4') {         //Caso aperte 5, motor gira pra esquerda
       if (GIRO != Esquerda)
       {
       GIRO = Esquerda;
       EnviaComando(0xC0);
       EnviaString (esquerda);
       }
    }
    if (TECLA == '6') {         //Caso aperte 5, motor gira pra direita
       if (GIRO != Direita)
       {
       GIRO = Direita;
       EnviaComando(0xC0);
       EnviaString (direita);
       }
    }
    TECLA=0;            //Zera a variável tecla
  }
  else
  {
    Passo(GIRO);
    nDelay(58-indicador); 
  }
  }
}



//Função: Passo
//Define a direção do passo
void Passo (int GIRO)
{
  static int Step = 0;
  static int Estado[2][4]= {0x00,0x00,0x00,0x00,  //Parado
                            0x0C,0x06,0x03,0x09}; //seq.crescente=Esquerda
                                                  //sec.descrescente=Direita
  P1OUT = Estado [abs(GIRO)][Step];
  Step = (GIRO+Step) & (BIT1+BIT0);
}

//Função nDelay
//Define a velocidade do motor

void nDelay (int REPETE)
{
    while (REPETE > 0)
    {
      REPETE = REPETE - 1;
      __delay_cycles (_2ms);
    }
}
  
  


//Função: config_motor
//Configura as saídas do MSP que vão pro motor
void config_motor (void)
{
  P1DIR = P1DIR | (0x0F);
  P1OUT = P1OUT & (0xF0);
  P1SEL = P1SEL & (0xF0);
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