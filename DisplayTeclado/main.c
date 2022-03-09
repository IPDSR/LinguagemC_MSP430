/* Segue em anexo o PDF com a lista de material e as orientações para a confecção e montagem da placa mãe que vai hospedar o kit LaunchPad EXP430G2ET.

Cada grupo deve montar uma placa mãe, que deve receber os nomes dos integrantes do grupo e o ano (2019) estampados, em cobre, na face dos filetes (bottom).

Após montada a placa, o aluno deverá elaborar um programa de teste que:
1. Exiba na primeira linha os nomes dos integrantes do grupo (de forma resumida, se for necessário) por um intervalo de tempo de 3 segundos. Em seguida a linha deve ser apagada;
2. Agora, na primeira linha, devem ser exibidos os caracteres digitados no teclado. Quando o display estiver todo preenchido (16 caracteres digitados) ao ser digitado o 17º caractere, a linha deve ser apagada e o caractere exibido na 1ª posição, repetindo-se o procedimento indefinidamente.
3. O bit P1.3 de ser configurado como entrada e o seu estado deve ser exibido na margem esquerda da segunda linha com a seguinte formatação "P1.3 = x", onde 'x' é o seu estado lógico. Lembre-se que a micro-switch localizada na borda esquerda do kit G2ET está conectada a este pino.
4. O bit P1.0 deve ser configurado como saída. O estado lógico de P1.0 deve ser o inverso da entrada P1.3.

A avaliação da montagem e do funcionamento da placa mãe está marcada para a semana de 5 a 9 de agosto, no dia e horário da aula de laboratório. Apesar da data de entrega, esta atividade será pontuada em 4,0 pt a serem computados na nota do segundo bimestre.

*/

#include "io430.h"      //Incli a biblioteca do msp 430
#include "stdlib.h"     //Inclui a biblioteca de instrucoes matemáticas


// Insira abaixo os defines que não depemdem do tipo de processador

#define _100us 100e-6*Fclk // calcula o num. de pulsos de clolck correspondem a um delay de 100 microsegundos
#define _2ms (int)(2e-3*Fclk) //Define o numero necessário para 2ms
#define _5ms 5e-3*Fclk          //Define o numero necessário para 5ms
#define _3s (3*Fclk)            //Define o numero necessário para 3s
#define _10ms (10e-3 Flck)      //Define o numero necessário para 10ms
#define _1ms (1e-3 Flck)        //Define o numero necessário para 1ms


// Crie abaixo as variáveis globais.
   unsigned char TECLA=0;        //Variavel global do valor da tecla
   int posicao=0;               // Variavel global da posicao no display
   char NOMES[] = "Fla, Gio e Isa " ; //Frase dos nomes a serem exibidos
   char ESTADO[] = "P1.3 =          ";  // Frase do pino a ser exibido
   char LIMPA[] = "                ";   //Frase para limpar o display
   
// Coloque aqui os protótipos da funções -----------------------------------------------------------------------------------------------------

void EnviaComando(unsigned char);        //prototipo da funcao enviacomando
void EnviaString(char *String);         //prototipo da funcao enviastring
void EnviaDado (unsigned char);    //prototipo da funcao enviadado
void Envia(unsigned char);                       //prototipo da funcao envia
void ConfigLCD(void);                   //prototipo da funcao configlcd
void configuraTeclado(void);             //prototipo da funcao configurateclado
unsigned char testar_colunas (unsigned char);   //prototipo da funcao testarcolunas
void ler_tecla(void);                   //prototipo da funcao lertecla
void ler_pino(void);                    //prototipo da funcao lerpino
void config_io (void);                  //prototipo da funcao configio
void envia_meio_comando(unsigned char palavra);
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
asm ("EINT");                   //Habilita interrupção
EnviaComando(0x80);            //Coloca escrita do display na primeira linha
EnviaString(NOMES);           //Envia os nomes para o display
__delay_cycles(_3s);
EnviaComando(0x01);           // limpa display e retorna o cursor para 1 posição 
__delay_cycles(_2ms);        //Apaga os nomes do display
config_io();                //configura as entradas e saídas

while (1)       //Loop das funções ler tecla e ler pino
{
  ler_tecla();  //Chama a função que lê a tecla e manda pro display
  ler_pino();  //Chama a função que lê o pino e mostra no display
}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------


/*----------------------------------------------------------------------------------------------------------------------------------------------------
Função ler_tecla
Mostra na primeira linha do display as teclas que são pressionadas, ao terminar
a linha, ela é apagada e novamente é preenchida a medida que as teclas são pressionadas*/

void ler_tecla (void)    //inicio da funcao ler tecla
{
  
    if (TECLA!=0) {             //se existir um valor em ascii na tecla
      if (posicao<=15)          //se existir espaco no display
      {
        EnviaComando(0x80+posicao);      //seleciona a posicao que a tecla irá no display
        EnviaDado (TECLA);              //envia o valor em ascii da tecla pressionada      
      }
      else              //se não existir mais espaço no display
      {
        EnviaComando(0x80);
        EnviaString(LIMPA);     //limpa a primeira linha do display
        posicao=0;              //volta o mostrador para a primeira posicao
        EnviaComando(0x80+posicao);      //seleciona a posicao que a tecla irá no display
        EnviaDado (TECLA);              //envia o valor em ascii da tecla pressionada
      }
    posicao = posicao + 1;      //a posicao avança 1 espaco
    TECLA=0;                  //Zera a variável tecla
    __delay_cycles(_1s);
  }

}

//----------------------------------------------------------------------------------------------------------------------------------------------

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
  P1DIR = P1DIR & 0xF7;         //Configura o pino 3 da porta 1 como entrada
  P1DIR = P1DIR | 0x01;         //Configura o pino 0 da porta 1 como saída
  EnviaComando(0xC0);            //Seleciona a primeira posicao da segunda linha do display
  EnviaString(ESTADO);    //Envia a frase para identificar o estado do pino
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

