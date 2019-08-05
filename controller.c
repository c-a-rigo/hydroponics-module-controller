#include <16f887.h>
#include <regs_887.h>
#use delay(clock=4MHz)
#fuses INTRC_IO


void inicializa_timer1(void);
void inicializa_ad(void);
void coleta_dados (void);
void interrupcao_externa (void);
void controla_umidade(void);
void controla_temperatura(void);
void liga_ventilacao (int duty);
void liga_aquecimento (int potencia);


int meio_segundo=0, dois_segundos=0, temperatura=0, umidade=0, minuto=0, quinze=0, quinze_anterior=0, doze=0, troca=0, controle=0, potencia_configurada=22;
void main(void)
{
//ISSO
//Configura��o das portas
TRISA=0b00000111; TRISB=0b00011001; TRISC=0; TRISD=0; TRISE=0;
PORTA=0; PORTB=0; PORTC=0; PORTD=0; PORTE=0;
CCP1CON=0;
CCP2CON=0;

//Inicializa��es

inicializa_ad();
inicializa_timer1();

INTEDG=1; INTE=1; GIE=1; //Habilita��o da interrup��o externa, com borda de subida.
TMR1IE=1; PEIE=1; //Habilita��o interrup��o do timer1, para rel�gio.


WPUB=0xFF; //Resistores de pull-up, para impedir que pinos fiquem 'flutuando'
IOCB=0xFF;
RD7=0; //Desabilita sa�da do TCA
RD1=1; RD3=1;
coleta_dados(); //Aquisi��o da temperatura e umidade

while (true)
{
if (RA2==1){RD2=0; liga_aquecimento(0); liga_ventilacao(0);RD1=0;while(RA2);}
RD3=~RD3; 

if (doze==0 || doze==2) // MODO DIA
   {
   RD2=1; //Liga ilumina��o artificial
   RB1=0; RD4=0; //garante solenoides fechadas
   controla_temperatura();
   if (quinze!=quinze_anterior){ RD1=!RD1; RD0=!RD1;quinze_anterior=quinze;} //Fica ligando e desligando a bomba de circula��o e oxigena��o a cada 15min

   
   }

if (doze==1 || doze==3) // MODO NOITE
   {
   RD2=0; //Desliga ilumina��o artificial
      RB1=0; RD4=0; //garante solenoides fechadas
   if (quinze!=quinze_anterior) //Liga circula��o de nutrientes por 15 min a cada duas horas
      {
      
      RD1=0; // Desliga bomba de circula��o
      RD0=!RD0; // Liga e desliga a oxigena��o a cada 15 min
      controle++;
      if(controle==7){controle=0; RD1=1;}
      quinze_anterior=quinze;  }

   
   }


if ((meio_segundo-dois_segundos)==4)//coleta temperatura e umidade cada 2 seg
{dois_segundos=meio_segundo;
coleta_dados();  } 
//!controla_umidade();
}//Fim while true
}//Fim do main

void controla_temperatura(void){

   if (temperatura<=18) liga_aquecimento(100);

   if ((temperatura>18)&&(temperatura<=22)) liga_aquecimento(70);

   if ((temperatura>22)&&(temperatura<=28)) liga_aquecimento(50);

   if ((temperatura>28)&&(temperatura<=30)) liga_aquecimento(30);

   if (temperatura>30) liga_aquecimento(0); //Desliga aquecimento

   if ((temperatura>35)&&(temperatura<=40)) {liga_ventilacao(20);liga_aquecimento(0);}

   if ((temperatura>40)&&(temperatura<45)) {liga_ventilacao(40);liga_aquecimento(0);}

   if (temperatura>=45) {liga_ventilacao(80);liga_aquecimento(0);}



   if ((temperatura<35)&&(umidade<70))
   {
      if ((minuto>11)||(minuto<9)) liga_ventilacao (0); //Desliga ventila��o;
   }


} // Fim de controle de temperatura

void controla_umidade(void){

   if (umidade<=45) RB2=1; //Liga nebuliza��o

   if (umidade>=55) RB2=0; //Desliga nebuliza��o

   if ((umidade>70)&&(umidade<80)){

   if (temperatura<30) liga_ventilacao(20);}

   if ((umidade>80)&&(temperatura<30)) liga_ventilacao(40);

} //Fim de controle umidade


void liga_aquecimento (int potencia)
{ 
GIE=0; //Desabilita interrup��es para evitar erro
int j=0;

if ((potencia==0)&&(potencia_configurada!=0))
{RD7=0; RD5=0;  potencia_configurada=0;}

if ((potencia==100)&&(potencia_configurada!=100))
{
potencia_configurada=100;
RD6=0; //Seleciona potenciometro para descer tens�o
for (j=0;j<100;j++)//Garante tens�o na sa�da do potenciometro igual a zero
{
  RD5=1;
  delay_us(5);
  RD5=0;
  delay_us(5);
}


RD7=1; // Aciona a lampada
} //Fim da pot 100

if((potencia==70)&&(potencia_configurada!=70))
{
potencia_configurada=70;
RD6=0; //Seleciona potenciometro para descer tens�o
for (j=0;j<100;j++)//Garante tens�o na sa�da do potenciometro igual a zero
{
  RD5=1;
  delay_us(5);
  RD5=0;
  delay_us(5);
}

   RD6=1; //Seleciona potenciometro para subir tens�o
    for (j=0;j<42;j++)//Garante tens�o na sa�da do potenciometro de 2,1 volts (54graus)
    {
    RD5=1;
    delay_us(5);
    RD5=0;
    delay_us(5);
   }
   RD7=1; // Aciona lampada
}

if ((potencia==50)&&(potencia_configurada!=50))
{
potencia_configurada=50;
RD6=0; //Seleciona potenciometro para descer tens�o
for (j=0;j<100;j++)//Garante tens�o na sa�da do potenciometro igual a zero
{
  RD5=1;
  delay_us(5);
  RD5=0;
  delay_us(5);
}
   RD6=1; //Seleciona potenciometro para subir tens�o
    for (j=0;j<70;j++)//Garante tens�o na sa�da do potenciometro de 3,5 volts (90graus)
    {
    RD5=1;
    delay_us(5);
    RD5=0;
    delay_us(5);
   }
   RD7=1; // Aciona lampada
}

if ((potencia==30)&&(potencia_configurada!=30))
{
potencia_configurada=30;
RD6=0; //Seleciona potenciometro para descer tens�o
for (j=0;j<100;j++)//Garante tens�o na sa�da do potenciometro igual a zero
{
  RD5=1;
  delay_us(5);
  RD5=0;
  delay_us(5);
}
   RD6=1; //Seleciona potenciometro para subir tens�o
    for (j=0;j<98;j++)//Garante tens�o na sa�da do potenciometro de 4,9 volts (126graus)
    {
    RD5=1;
    delay_us(5);
    RD5=0;
    delay_us(5);
   }
   RD7=1; // Aciona lampada
}

GIE=1; // Reabilita interrup��es

}//Fim de liga aquecimento


void liga_ventilacao (int duty){
   GIE=0;

   if (duty==0)
   {
    /*
    * Configura��o dos registradores de PWM
    * Fosc = 4000000 Hz
    * Fpwm = 1000.00 Hz
    * Duty Cycle = 0 %
    * Resolu��o 10 bits
    *Prescaler de 4
    */
    PR2 = 0b11111001;
    T2CON = 0b00000101 ;
    CCPR1L = 0b00000000 ;
    CCP1CON = 0b00001100 ;
   }


   if (duty==20)
   {
    /*
    * Configura��o dos registradores de PWM
    * Fosc = 4000000 Hz
    * Fpwm = 1000.00 Hz
    * Duty Cycle = 20 %
    * Resolu��o 10 bits
    *Prescaler de 4
    */
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR1L = 0b00110001 ;
    CCP1CON = 0b00111100 ;
   }

   if (duty==40)
   {
   /*
    * Configura��o dos registradores de PWM
    * Fosc = 4000000 Hz
    * Fpwm = 1000.00 Hz
    * Duty Cycle = 40 %
    * Resolu��o 10 bits
    *Prescaler de 4
    */
   PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR1L = 0b01100011 ;
    CCP1CON = 0b00111100 ;
   }

   if (duty==60)
   {
   /*
    * Configura��o dos registradores de PWM
    * Fosc = 4000000 Hz
    * Fpwm = 1000.00 Hz
    * Duty Cycle = 60 %
    * Resolu��o 10 bits
    *Prescaler de 4
    */
   PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR1L = 0b10010101 ;
    CCP1CON = 0b00111100 ;
   }

   if (duty==80)
   {
   /*
    * Configura��o dos registradores de PWM
    * Fosc = 4000000 Hz
    * Fpwm = 1000.00 Hz
    * Duty Cycle = 80 %
    * Resolu��o 10 bits
    *Prescaler de 4
    */
   PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR1L = 0b11000111 ;
    CCP1CON = 0b00111100 ;
   }

   if (duty==100)
   {
   /*
    * Configura��o dos registradores de PWM
    * Fosc = 4000000 Hz
    * Fpwm = 1000.00 Hz
    * Duty Cycle = 100 %
    * Resolu��o 10 bits
    *Prescaler de 4
    */
   PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR1L = 0b11111001 ;
    CCP1CON = 0b00111100 ;

   }

GIE=1;
}//Fim de liga ventila��o

void coleta_dados (void){
PEIE=0; // desabilita interrup��o timer 1 para evitar erro
int j=0;
int16 palavra=0;


// Canal 0 do AD � temperatura e canal 1 � a umidade
CHS3=0;CHS2=0;CHS1=0;CHS0=0; // Seleciona canal 0 (RA0) para convers�o.


//********DADOS********
for (j=0; j<2; j++)
{
  GO_DONE=1; // Inicia convers�o.
  while (GO_DONE); // Aguarda convers�o terminar.
  palavra=ADRESH*256+ADRESL;
  ADCON0=ADCON0+4; //seleciona pr�ximo canal para convers�o.

  if (j==0) // TEMPERATURA
  {
   temperatura=(palavra*50.0)/1023; //Transforma leitura do AD na temperatura correspondente
   }

  else if (j==1) // UMIDADE
  {
     umidade=((32.26*5.0*palavra)/1023)-25.806; //Transforma leitura do AD na umidade relativa correspondente
   }
}
PEIE=1; // Reabilita interrup�ao timer 1
} //Fim da coleta de dados



void inicializa_timer1(void)
{
TMR1CS=0; // uso do clock de m�quina.
T1OSCEN=0; // circuito oscilador desligado.
T1CKPS1=1; T1CKPS0=1; // prescaler 1:8.
TMR1H=0x0B; TMR1L=0xDC; // timer1 valor inicial 3036 (sobra 62500). meio segundo!
TMR1ON=1; // liga timer.
} //Fim da inicializa��o do timer1

void inicializa_ad(void)
{
ANSEL=0x0F; ANSELH=0x00; // Define RA como entrada anal�gica.
ADCS1=1; ADCS0=1; // RC interno.
ADFM=1; // Justifica � direita.
VCFG1=0; // Vref- => VSS.
VCFG0=0; // Vref+ => VCC.
ADON=1; // Liga sistema AD.
}//Fim da inicializa��o do AD


// Tratamento interrup��o timer1
#INT_TIMER1
void interrupcao_timer1 (void)
{
TMR1ON=0; // desliga timer.
TMR1IF=0; // zera a flag.

meio_segundo++;
if (meio_segundo==120)  {meio_segundo=0; minuto++;}
if (minuto==15)   {minuto=0; quinze++; RB7=~RB7;}
if (minuto==9) liga_ventilacao (20); //liga ventila��o para controle de co2 2 min on
if (minuto==11)  liga_ventilacao (0); // desliga ventila��o para controle de co2
if (quinze==48)   {quinze=0; doze++; liga_aquecimento(0); troca++;}
if (doze==4) doze=0;
if (troca==4) {troca=0; RD3=1;}

TMR1H=0x0B; TMR1L=0xDC; // timer1 valor inicial 3036 (sobra 62500). Meio segundo!!
TMR1ON=1; // liga timer.
}//Fim da interrup��o timer1

// tratamento da interrup��o externa em RB0
#INT_EXT
void interrupcao_externa (void)
{int j=0;
GIE=0; //Desabilita interrup��o
INTF=0; // Zera flag de interrup�ao
RD3=0; //Desliga led sinaliza��o troca
troca=0; //zera flag de troca
RD4=1; // Aciona solenoide de sa�da da solu��o
for (j=0; j<180; j++) {delay_ms(1000);} //Espera agua sair (3min)
RD4=0; //Fecha solenoide de sa�da da solu��o
RB1=1;//Abre solenoide de entrada da solu��o
while(!RB4); //Aguarda encher
RB1=0; //Fecha solenoide de entrada da solu��o
GIE=1; //Habilita interrup��es
}//Fim do tratamento de interrup��o externa
