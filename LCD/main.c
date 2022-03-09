/*Prog01: Elaborar um programa que monitore os bits P1.1 e P1.0. Cada combina��o destes 
  bits implicar� no envio de uma mensagem diferente para o display, a saber:
  P1.1    P1.0
   0       0    = Frase1 sendo exibida na primeira linha
   0       1    = Frase2 sendo exibida na segunda linha
   1       0    = Frase3 sendo exibida na duas linhas
   1       1    = Alternar as frases anteriores a cada 3s.
*/

#include "io430.h"


// Crie aqui suas vari�veis globais
// Tome cuidado para n�o criar vari�veis desnecess�rias!!!
// "Global, s� quando for extritamente necess�rio."


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

// Coloque aqui os prot�tipos da fun��es
void EnviaComando(char Comando);
void EnviaDado(char Dado);
void EnviaString(char *String);
void Envia(void);
void ConfigLCD(void);
                                                                    
 int main( void )
{
  // Crie aqui suas vari�veis locais
  char Turma[]=   "ELTRETA <3    ";
  char Bancada[]= "Isa e Flavinha";
  char Discip[]=  "Nos temos     ";
  char Apaga[]=   "                ";
  char anterior = 0xFF;
  
  
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
   // Configurar P1.0 e P1.1 como entradas
  // Atente para fazer opera��es apenas no bits pertinentes
  P1DIR = P1DIR & ~(BIT0+BIT1);
  P1SEL = P1SEL & ~(BIT0+BIT1);
  
  // Configura��o ou inicializa��o dos perif�ricos
  ConfigLCD();
  
  
  // Testando se a configura��o foi bem feita


    
  //Loop principal
  // Abaixo
 // while(1)  // loop infinito
  //{

   
  /* Esta � uma das possibilidades de implementa��o do teste das combina��es 
  //  dos bit da porta P1
    
  if ((P1IN &(BIT0+BIT1))==03)
  {
  
    
  }
  else if ((P1IN &(BIT0+BIT1))==02)
    {
    
      
    }  
  else if ((P1IN &(BIT0+BIT1))==01)
  {
  
  
  }
  else ((P1IN &(BIT0+BIT1))==00)
  {
  
  
  }

  
  }
  */
    
    /* Esta � uma outra possibilidade, agora utulizando switch case */
 while (1)
 {
 
   if((P1IN &(BIT0+BIT1)) != anterior)
   {
     anterior = P1IN &(BIT0+BIT1);
     
     switch (P1IN &(BIT0+BIT1))
     {
     case 03:
       
       EnviaComando(0x80); 
       EnviaString (Apaga);
       EnviaComando(0x80);
       EnviaString(Turma);
       __delay_cycles(_3s);
       
       EnviaComando (0xC0); 
       EnviaString (Apaga);
       EnviaComando (0xC0); 
       EnviaString(Turma);
       EnviaComando(0x80);
       EnviaString (Apaga);
       EnviaComando(0x80);
       EnviaString(Bancada);
       __delay_cycles(_3s);
       
       EnviaComando (0xC0);
       EnviaString (Apaga);
       EnviaComando (0xC0); 
       EnviaString(Bancada);
       EnviaComando(0x80);
       EnviaString (Apaga);
       EnviaComando(0x80);
       EnviaString(Discip);
       __delay_cycles(_3s);
       
       EnviaComando (0xC0); 
       EnviaString (Apaga);
       EnviaComando (0xC0); 
       EnviaString(Discip); 
       __delay_cycles(_3s);
       
       
       break;
       
       
     case 02:
       EnviaComando(0x80);
       EnviaString (Apaga);
       EnviaComando(0x80);
       EnviaString (Discip);
       EnviaComando (0xC0);
       EnviaString (Apaga);
       EnviaComando (0xC0);
       EnviaString (Discip);
       
       break;
       
     case 01:
       EnviaComando (0xC0);  
       EnviaString(Apaga);
       EnviaComando(0xC0);
       EnviaString(Bancada);
       EnviaComando(0x80);
       EnviaString (Apaga);
       
       
       break;
       
       
     case 00: 
       EnviaComando(0x80);
       EnviaString(Apaga);
       EnviaComando(0x80);
       EnviaString(Turma);
       EnviaComando(0xC0);
       EnviaString (Apaga);
       
       
       
       break;
       
     }
   }     
 }        
  return 0;
}


// �rea das fun��es          
 
// Nome: ConfigLCD
// Configura��o inicial do display LCD
void ConfigLCD(void)
{
  // Configura��o dos pinos de IO utilizados pelo display LCD
  P5DIR = 0XFF;         // configura toda a P5 como sa�da
  P5SEL = 0x00;         // Configura toda a P5 como I/O
  P3DIR = P3DIR | (BIT6+BIT7);   // configura bits 6 e 7 como sa�das
  P3SEL = P3SEL & ~(BIT6+BIT7);
    
  EnviaComando(0x38);
  __delay_cycles(_5ms);
  
  EnviaComando(0x0E);
  EnviaComando(0x06);
  EnviaComando(0x01);
  __delay_cycles(_2ms);
  
}

// Fun�ao EnviaComando
// Envia um byte de configura��o para o display
void EnviaComando(char Comando)
{
  DISPLAY = Comando;   //ou P5OUT = Comando
  DISPCTL = DISPCTL & ~RS;  // executa o clear bit do pino RS (P3.7)
  // ou pode ser escrito da seguinte forma
  // DISPCTL &= ~RS;
  Envia();
  
}


// Fun��o: Envia
// Gera o pulso do pino de Enable do Display
void Envia(void)
{
  DISPCTL = DISPCTL & ~ENABLE;   // clear bit Enable
  DISPCTL = DISPCTL | ENABLE;    // set bit Enable
  DISPCTL = DISPCTL & ~ENABLE;   // clear bit Enable
  __delay_cycles(_100us);
  
}

//Fun��o: EnviaString
//Envia uma frase para o display
void EnviaString(char *String)
{
    while(*String) 
      {
        EnviaDado(*String);
        String++;
      }
  }

  

// Fun�ao EnviaDado
// Envia um byte de configura��o para o display
void EnviaDado(char Dado)
{
  DISPLAY = Dado;   //ou P5OUT = Comando
  DISPCTL = DISPCTL | RS;  // executa o set bit do pino RS (P3.7)
  Envia();
  
}

