// Biblioteca de definições e funções para o kit G2553

#define Fclk   1.2e6           // Freq. de clk do DCO para o G2553
#define _10ms (10e-3 Flck)      //Define o numero necessário para 10ms
#define _5ms 5e-3*Fclk          //Define o numero necessário para 5ms
#define _100us 100e-6*Fclk
#define _500ms 500e-3*Fclk
#define _50ms  50e-3*Fclk
#define _1s    1*Fclk



// Dando nomes mais interessantes para os bits das portas P1 e P2
// Porta P1 - conexões com o LCD 
#define LCDPort      P1OUT                       // Os bits de dados do LCD estão conectados
#define LCDPort_dir  P1DIR                       // na porta P1
#define LCDPort_sel  P1SEL
#define LCDData      (BIT4+BIT5+BIT6+BIT7)       // Barramento de 4 bits do display

// Porta P1 - conexõeos com o teclado
#define LedVm        BIT0                        // Leds que já vêm montado no kit
#define LedVd        BIT1
#define ChS2         BIT3                        // Micro chave que já vem montada no kit
#define TecLins     (TecL1+TecL2+TecL3+TecL4)             
#define TecLin       P1OUT                       // As linhas do teclado estão conectadas na
#define TecLin_dir   P1DIR                       // porta P1
#define TecLin_sel   P1SEL                                                                                
#define TecL1        BIT4                        // Cada uma das linhas do teclado
#define TecL2        BIT5
#define TecL3        BIT6
#define TecL4        BIT7

// Porta P2
#define LCDCtl_ifg   P2IFG
#define LCDCtl_ie    P2IE
#define LCDCtl       P2OUT                       // Os bit En e RS estão conecetados na 
#define LCDCtl_dir   P2DIR                       // porta P2
#define LCDCtl_sel   P2SEL        
#define LCDEn        BIT1                        // Enable  = Bit de controle do LCD
#define LCDRS        BIT7                        // Este bit é compartilhado com a coluna 1
                                                 // do teclado e o pino RS do LCD. Como
                                                 // coluna deve ser uma entrada e como RS
                                                 // deve ser uma saída. Cada um das rotinas 
                                                 // de teclado e display devem fazer a
                                                 // configuração adequada do bit
#define TecCol       P2IN                       // As colunas do teclado estão conectadas 
#define TecCol_dir   P2DIR                       // na porta P2  
#define TecCol_sel   P2SEL        
#define TecC1        BIT0                        // Cada uma das colunas do teclado
#define TecC2        BIT5
#define TecC3        BIT6

#define TecCols      (TecC1+TecC2+TecC3)

// Primeira palavra de configuração do display LCD para o kit LaunchPad
#define LCD_DL       0                           // BIT4 = DL, 1 = barramento de 8 bits, 0 = 4 bits
#define LCD_N        BIT3                        // BIT3 = N, 1 = 2 linhas, 0 = 1 linha
#define LCD_F        0                           // BIT2 = f, 1 = matriz 5x10, 0 = matriz 5x8


//Defines para IO do semáforo
//P1
#define RED1         BIT0
#define GREEN1       BIT1
#define BUZZER       BIT2       //TA0.1
#define RED          BIT3
//P2
#define YELLOW       BIT2
#define GREEN        BIT3
#define SENSOR       BIT4


/*-----------------------------------------------------------------------------------------------------------------------------------------------------
// Nome: ConfigLCD
// Configuração inicial do display LCD*/

void ConfigLCD(void)     // Configuração dos pinos de IO utilizados pelo display LCD
{
  
  P1SEL = P1SEL & 0x0F;         // Configura como io  
  P1DIR = P1DIR | 0xF0;         // configura os bits do display
  P1OUT = P1OUT & 0x0F;

  P2SEL = P2SEL & ~(BIT7+BIT1);         //configura os bits 7 e 1 como normais
  P2DIR = P2DIR | (BIT7+BIT1);   // configura bits 7 e 1 como saídas
  P2OUT = P2OUT & ~(BIT1);

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------
//NOME: envia_comando 
//FUNÇÃO: envia atividade para o display realizando quando E=0
//ENTRADA: palavra
//SAIDA: -
//----------------------------------------------------------------------------

void EnviaComando(unsigned char palavra)
{
  P2IE = P2IE & ~(TecC1+TecC2+TecC3);      // Desabilita interrupcao
  LCDCtl_dir = LCDCtl_dir | (LCDRS);
  LCDCtl &= ~LCDRS;
  Envia(palavra);
}

//----------------------------------------------------------------------------
//NOME: envia_dado      
//FUNÇÃO: envia atividade para o display realizando quando E=1
//ENTRADA: palavra      
//SAIDA: -
//----------------------------------------------------------------------------

void EnviaDado(unsigned char palavra)
{
  P2IE = P2IE & ~(TecC1+TecC2+TecC3);      // Desabilita interrupcao
  LCDCtl_dir = LCDCtl_dir | (LCDRS);  
  LCDCtl |= LCDRS;
  Envia(palavra);
 
}


//----------------------------------------------------------------------------
// Função para o display LCD - Conexão de 4bits
// Nome: Envia
// Funnção: envia um byte para ser exibido no display, que deve corresponder
//          a um dos códigos da tabela ASCII
//          Utiliza a conexão de 4 bits com o display, sendo:
//          D7 = P1.7
//          D6 = P1.6
//          D5 = P1.5
//          D4 = P1.4
//----------------------------------------------------------------------------

void Envia(unsigned char palavra)
{
  LCDPort = (LCDPort & 0x0F) + (palavra & 0xF0); // Envia apenas o nibble superior
  LCDCtl &= ~LCDEn;           //Gera pulso de nível alto no pino Enable do display
  LCDCtl |= LCDEn;
  LCDCtl &= ~LCDEn;
  __delay_cycles(_100us);
  
  LCDPort = (LCDPort & 0x0F) + ((palavra<<4) & 0xF0); // Envia apenas o nibble inferior
  LCDCtl |= LCDEn;            //Gera pulso de nível alto no pino Enable do display
  LCDCtl &= ~LCDEn;
  __delay_cycles(_100us);
  P2IFG = P2IFG & ~(TecC1+TecC2+TecC3); 
  P2IE = P2IE | (TecC1+TecC2+TecC3);             //habilita interrupcao  
  TecLin = TecLin | (TecL1+TecL2+TecL3+TecL4);        //Ativa os 4 primeiros pinos da porta  
}


// Adicione a rotina de teclado e faça as devidas alterações

/* ---------------------------------------------------------------------------------------------------------------------------------------------
Criação da interrupção do teclado: interrompe o programa principal quando alguma tecla
do teclado for pressionada e mostra o valor da tecla na variavel TECLA */

#pragma vector=PORT2_VECTOR     //linha de programação da interrupcao da porta 2
__interrupt void Teclado()      //funcao interrupcao

{
  unsigned char TAB_TEC[] ={"\0""123456789*0#"};       //Vetor com os valores em ascii
  unsigned char APT_TEC=0;             //Apontador do vetor tab_tec
  
  P2IE = P2IE & ~(TecC1+TecC2+TecC3);      // Desabilita interrupcao
  P2IFG = P2IFG & ~(TecC1+TecC2+TecC3);    //Desliga flag de interrupcao
  __enable_interrupt();                 //habilita interrupcao
  
  __delay_cycles(_5ms);
  
  if ((TecCol & (TecC1+TecC2+TecC3)) == (0x00)) {
    P2IFG = P2IFG & ~(TecC1+TecC2+TecC3);        //Reseta os flags de interrupção
    P2IE = P2IE | (TecC1+TecC2+TecC3);             //habilita interrupcao
    return;
  }
  else 
  {
    TecLin = TecLin | (TecL1);       //Liga a primeira linha
    TecLin = TecLin & ~(TecL2+TecL3+TecL4);       //Desliga as outras linhas
  }
  
  if ((TecCol & (TecC1+TecC2+TecC3)) == (0x00)) {        //Se a coluna 1 não é acionada, soma 3 no apt_tec
    TecLin = TecLin | (TecL2);               //Liga a segunda linha
    TecLin = TecLin & ~(TecL1+TecL3+TecL4);               //desliga as outras linnhas
    APT_TEC = APT_TEC + 3;              //Soma 3 no apontador do tab_tec
  }
  else
  {
    APT_TEC = testar_colunas (APT_TEC);       //Se a coluna é acionada, testa as colunas
    TECLA=TAB_TEC[APT_TEC];             //o valor dentro do vetor tab_tec vai para a variavel tecla
    return;
  }
  
  if ((TecCol & (TecC1+TecC2+TecC3)) == (0x00)) {        //Se a coluna 2 não é acionada, soma 3 no apt_tec
    TecLin = TecLin | (TecL3);               //Liga a terceira linha
    TecLin = TecLin & ~(TecL1+TecL2+TecL4);               //Desliga as outras linhas
    APT_TEC = APT_TEC + 3;              //Soma 3 no apontador do tab_tec
  }
  else
  {
    APT_TEC = testar_colunas (APT_TEC);       //Se a coluna é acionada, testa as colunas
    TECLA=TAB_TEC[APT_TEC];                     //o valor dentro do vetor tab_tec vai para a variavel tecla
    return;
  }
  
  if ((TecCol & (TecC1+TecC2+TecC3)) == (0x00)) {        //Se a coluna 3 não é acionada, soma 3 no apt_tec
    TecLin = TecLin | (TecL4);               //Liga a quarta linha
    TecLin = TecLin & ~(TecL1+TecL2+TecL3);               //Desliga as outras linhas
    APT_TEC = APT_TEC + 3;              //Soma 3 no apontador do tab_tec
  }
  else
  {
    APT_TEC = testar_colunas (APT_TEC);       //Se a coluna é acionada, testa as colunas
    TECLA=TAB_TEC[APT_TEC];                     //o valor dentro do vetor tab_tec vai para a variavel tecla
    return;
  }
  
  if ((TecCol & (TecC1+TecC2+TecC3)) == ~(TecC1+TecC2+TecC3)) {                //Se não tem colunas acionadas, reseta os flags de interrupção
    TECLA=TAB_TEC[APT_TEC];                     //o valor dentro do vetor tab_tec vai para a variavel tecla
    TecLin = TecLin | (TecL1+TecL2+TecL3+TecL4);                       //liga todas as linhas
    P2IFG = P2IFG & ~(TecC1+TecC2+TecC3);          //reseta os flags de interrupção
  }
  else
  {
    APT_TEC = testar_colunas (APT_TEC);       //Se a coluna é acionada, testa as colunas
    TECLA=TAB_TEC[APT_TEC];                     //o valor dentro do vetor tab_tec vai para a variavel tecla
    return;
  }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

/*-----------------------------------------------------------------------------------------------------------------------------------------------------
Função: testar_colunas
Le a tecla que foi pressionada no teclado e envia para o display*/

unsigned char testar_colunas(unsigned char APT_TEC) {
   
        if ((TecCol&(TecC1))==(TecC1))          //Se a coluna 1 estiver acionada
        {
     
          APT_TEC = APT_TEC + 1;        //Se a coluna 1 é acionada soma 1 no apt_tec
          TecLin = TecLin | (TecL1+TecL2+TecL3+TecL4);         //Liga todas as linhas
          P2IFG = P2IFG &~ (TecC1+TecC2+TecC3);    //Reseta os flags de interrupcao
          P2IE = P2IE | (TecC1+TecC2+TecC3);       //Habilita a interrupcao
        }
          
        else if ((TecCol&(TecC2))==(TecC2))     //Se a coluna 2 estiver acionada
        {
          APT_TEC = APT_TEC + 2;        //Se a coluna 2 é acionada soma 1 no apt_tec
          TecLin = TecLin | (TecL1+TecL2+TecL3+TecL4);         //Liga todas as linhas
          P2IFG = P2IFG &~ (TecC1+TecC2+TecC3);    //Reseta os flags de interrupcao
          P2IE = P2IE | (TecC1+TecC2+TecC3);       //Habilita a interrupcao
        }
          
        else if ((TecCol&(TecC3))==(TecC3))     //Se a coluna 3 estiver acionada
        {
          APT_TEC = APT_TEC + 3;        //Se a coluna 3 é acionada soma 1 no apt_tec
          TecLin = TecLin | (TecL1+TecL2+TecL3+TecL4);         //Liga todas as linhas
          P2IFG = P2IFG &~ (TecC1+TecC2+TecC3);    //Reseta os flags de interrupcao
          P2IE = P2IE | (TecC1+TecC2+TecC3);       //Habilita a interrupcao
        }
          
        else            //Caso nenhuma das colunas estejam acionadas
        {
          TecLin = TecLin | (TecL1+TecL2+TecL3+TecL4);         //Se não, zera os flags de interrupção
          P2IFG = P2IFG &~ (TecC1+TecC2+TecC3);//Reseta os flags de interrupcao
          P2IE = P2IE | (TecC1+TecC2+TecC3);       //Habilita a interrupcao
        } 
        return APT_TEC;         // Retorna o valor de apt_tec
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------        
