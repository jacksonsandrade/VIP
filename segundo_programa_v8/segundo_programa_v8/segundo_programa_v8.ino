/*Orientação
 * 
 * Este programa é parte constituinte de um protótipo de medidor de energia elétrica monófasico, para obter as
 * demais partes, assim como informações complementares, lêr o arquivo README armazenado no mesmo diretório deste
 * programa.
 * 
 * O programa está esquematizado da seguintes maneira:
 * 
 * 1° endereços da EEprom utilizados: são as posições, na base decimal, ocupadas da memória EEPROM.
 * 2° Variaveis do programa editaveis: contém todas as variaveis livres de edição pelo usuario; 
 * 3° Internet, Comunicação Master e Slave,Cartão SD, Tempo, EEPROM, Comunicação I2C: Contém as bibliotecas e suas respectivas configurações 
 * 4° Variaveis do programa não editaveis: são as variaveis inerentes ao programa, no qual recomenda-se atenção caso deseje-se altera-las, pois 
 *                                        podem tornar o programa instavel.
 * 5° O programa em si.                                       
 */
 
/**********************************************************************************************
   endereços da EEprom utilizados
   ICMS = 0, 1, 2;                                       //armazena o valor do ICMS
   PIS = 3, 4, 5;                                        //armazena o valor do PIS
   COFINS = 6, 7, 8;                                     //armazena o valor do COFINS
   tarifa = 96, 97, 98;                                  //armazena o valor da tarifa
   consumo total do medidor: 19, 20, 21, 22;             //armazena o consumo total registrado
   consumo mensal: 30, 31, 32, 33;                       //armazena o consumo do inicio do mês
   consumo diario: 40, 41, 42, 43;                        //armazena o consumo do inicio do dia
   consumo registrado no SD: 50,51,52,53;

   status reinicio do ciclo de medição mensal = 100;
   dia_mesnal = 99.                                    //armazena o dia mensal
   internet = 95;                                      //armazena o status do armzenamento online (0= nâo / 1= sim)
   sd = 94;                                            //armazena o status do armzenamento no SD (0= nâo / 1= sim)
 *********************************************************************************************/

//VARAVEIS DO PROGRAMA EDITAVEIS//////////////////////////////////////////////////////////////////////////////////////
byte mac[] = {0x28, 0xbe, 0x9b, 0x9b, 0x15, 0x3a};         //endereço MAC do arduino Mega
#define slaveAdress 0x08                                   // endereço slave do Uno
int  atu_lcd_m = 60;                                       // frequência de attualização no lcd de V, I e P
int  atu_lcd_v = 60;                                       // frequência de atualização no lcd de custos e consumos
int  medminutos = 1;                                       // tempo de demanda em minutos

//parâmetros do thingspeak
unsigned long myChannelNumber = 178355;                    //obtido no site do ThingSpeak
const char * myWriteAPIKey = "IYZK9CJEHVJ7P4GK";           //obtido no site do ThingSpeak
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
//INTERNET////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ThingSpeak.h"                                    //Adiciona a biblioteca ThingSpeak 
#include <SPI.h>                                           //Adiciona a biblioteca SPI
#include <Ethernet.h>                                      //Adiciona a biblioteca Ethernet
EthernetClient client;
int  atu_internet;                                                
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// COMUNICAÇÃO MASTER E SLAVE//////////////////////////////////////////////////////////////////////////////////////////
int Y;                                                     // Variavel paara comunicação entre o Uno e Mega, ver o TFC
long le_I2C(long, long, long);
float Vrms, Irms, fp, S, P;
int sfp;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CARTÃO SD///////////////////////////////////////////////////////////////////////////////////////////////////////////
// parametros de gravvação no SD.
#include <SdFat.h>                             //inclui a biblioteca Sd.fat
SdFat sdCard;                      
SdFile meuArquivo;
const int chipSelect = 4;
unsigned long soma_consumo;                   // soma o consumo no SD.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//TEMPO/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Time.h>                            //inclui a biblioteca Time.h 
#include <TimeLib.h>                         //inclui a biblioteca TimeLib.h
#include <DS1307RTC.h>                       //inclui a biblioteca do RTC
tmElements_t rtc_variavel;  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//EEPROM////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <EEPROM.h>                          //inclui a biblioteca da EEPROM.h
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//COMUNICAÇÃO I2C////////////////////////////////////////////////////////////////////////////////////////////////
#include <Wire.h>                                                       //Biblioteca da comunicação I2C
#include <LiquidCrystal_I2C.h>                                          //Biblioteca do LCD
LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);          //Endereço slave do LCD
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// VARIAVEIS DO PROGRAMA (Nâo Editaveis)/////////////////////////////////////////////////////////////////////////
char menu = 0;                            
long tarifa;
int dia_mensal;
int aux_ciclo;
int alterar_dia_mes();
int alterar_gravacao();
int impostos = 1;
unsigned long  imposto_vetor[4];
int inicio;
long aux_4;
int sd;
int sd_atualiza;
int internet;
int atualiza = 1000000; 
void menu_imposto();
void menu_configuracao();
void  menu_gravacao();
void menu_medicao();
void telainicial();
void submenu11();
void submenu12();
void submenu13();
void submenu14();
void submenu15();
void submenu16();
void menu_clear();
void submenu21();
unsigned long teste;
void menu_tarifa();
void medicao();
void mudaMenu();
void lcdMenu();
void alteraData();
void alterarHorario();
void alterarDiaSemana();
void displayRelogio();
int testaBotoes();
void print2digits(int);
void Escrever_EEPROM(int, int, int);
void Escrever_EEPROM_6d(long, int, int, int);
int ajustaDigitos( int, int, int, int);
void Escrever_EEPROM_8d(unsigned long, int, int, int, int);
unsigned long le_EEPROM_8d(int, int, int, int);
long le_EEPROM(int, int, int);
void tela_basica();
int consumo_mes;
int consumo_dia;
unsigned long mult_fatura;
int momento;
int mostra_menu_tarifa (int, int, int);
int mostra_menu_imposto (int, int, int, int);
unsigned long alteraimposto();
long alteratarifa();
void rotina_principal();
void gravacao_internet();
void gravacao_sd();
void chama_atualizacao();
int sts_sd;
byte sts_display = 0;
int atu_lcd;
int at_lcd;
void pontoDemanda();
unsigned int auxDemanda;
unsigned int amostra = (medminutos * 60); 
unsigned long d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15, d16, d17, d18, d19, d20, d21, d22, d23, d24, d25, d26, d27, d28, d29,
              d30, d31, d32, d33, d34, d35, d36, d37, d38, d39, d40, d41, d42, d43, d44, d45, d46, d47, d48, d49, d50, d51, d52, d53, d54, d55, d56, 
              d57, d58, d59, d60;
unsigned long demanda;
unsigned long aux_kwh = 278; // 278 = (1/(3600*1000)) * 10^-9
unsigned long aux_dia, aux_mes;
int status_arduino = 0 ;
boolean troca;
const int reset_cs_total = 2;
const int reset_geral = 3;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  Wire.begin();                              // inicia o barramento I2C
  lcd.begin(20, 4);                          // configura o tamanho da tela do LCD.
  pinMode(reset_cs_total, INPUT);            // pino do reset dos consumos
  pinMode(reset_geral, INPUT);               // pino do reset geral
  chama_atualizacao(); 
}

void loop() {

  mudaMenu();                              // chama a função para mudar a pagina do menu principal
  lcdMenu();                               // chama a a função que define o que será mostrado em cada pagina do menu
  rotina_principal();                      // conforme descrita no TFC
}


void mudaMenu() {                                  // modificar o menu atual
  int valbotao;                                    // variavel que recebe o valor lido na porta A0
  int max_paginas = 3;                             // nesse caso, o nosso menu tem 4 paginas principais, incluindo o 0.
  valbotao = analogRead(0);                        // leitura do valor da porta analógica A0
  if (valbotao >= 80 && valbotao < 180) {           // Se a leitura do pino analógico está nessa faixa (80 a  179)
    while (valbotao >= 80 && valbotao < 180) {     // esse while espera o usuario soltar o botão
      valbotao = analogRead(0);
      delay(20);
    }
    sts_display = 0;                                //usado para indicar que houve troca de menu
    menu--;                                         // decrementa o menu - botão 2 - para cima
    if (menu < 0) menu = max_paginas;            // se o menu chegar na primeira pagina, volta para a terceira
  }
  else if (valbotao >= 180 && valbotao < 350) {     // Se a leitura do pino analógico está nessa faixa (180 a  349)
    while (valbotao >= 180 && valbotao < 350) {     // esse while espera o usuario soltar o botão
      valbotao = analogRead(0);
      delay(20);
    }
    lcd.clear();                                    // limpa o lcd
    sts_display = 0;
    menu++;                                         // incrementa o menu - botão 3 - para baixo
  }
}
void lcdMenu() {                          // mostrar o menu atual

  int auxiliar, aux_1, y, z, aux_5;
  long aux_3 ;
  long aux_4, x;

  switch (menu) {                         // controle da variavel menu

    case 0:
      if (inicio == 0) {
        telainicial();
      }
      tela_basica();
      inicio = 1;
      break;

    case 1:                            //
     menu_medicao(); 
      break;                              // sai da função

    case 2:                            // caso 2 - menu 2: gravação de dados
      menu_gravacao();
      auxiliar = testaBotoes();
      if ( auxiliar == 5) menu = 21;
      else if (auxiliar == 2) menu--;
      else if (auxiliar == 3) menu ++;
      break;                              // sai da função

    case 3:                            // caso 3 - menu 3: alterar horÃ¡rio
      menu_configuracao();                    // limpa o lcd
      auxiliar = testaBotoes();
      if ( auxiliar == 5) menu = 11;
      else if (auxiliar == 2) menu--;
      else if (auxiliar == 3) menu = 0;
      break;                              // sai da função

    case 11:                            // caso 1 - menu 1:imposto
      submenu11();                   // chama a função para mostrar imposto
      auxiliar = testaBotoes();
      if ( auxiliar == 5) {
        lcd.clear();
        menu_imposto();
      }
      else if (auxiliar == 2) menu = 3;
      else if (auxiliar == 3) menu ++;
      lcd.clear();
      break;                              // sai da função

    case 12:                            //caso 12 - submenu 12: alterarHorario
      submenu12();
      auxiliar = testaBotoes();
      if ( auxiliar == 5) {
        alterarHorario();
      }
      else if (auxiliar == 2) menu--;
      else if (auxiliar == 3) menu ++;
      break;                              // sai da função

    case 13:                            // alterar data
      submenu13();
      auxiliar = testaBotoes();
      if ( auxiliar == 5) {
        alteraData();
      }
      else if (auxiliar == 2) menu--;
      else if (auxiliar == 3) menu ++;
      break;                              // sai da função

    case 14:                            //alterar dia do reinicio do ciclo de medição mensal
      submenu14();
      auxiliar = testaBotoes();

      if ( auxiliar == 5) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Dia do Mes");
        lcd.setCursor(0, 2);
        lcd.print("atual: ");
        lcd.print(dia_mensal);
        auxiliar = testaBotoes();

        if (auxiliar == 5) {
          lcd.clear();
          aux_1 = alterar_dia_mes();

          if (aux_1 != -1) {
            dia_mensal = aux_1;
            EEPROM.write(99, dia_mensal); //posição 99 para não dar conflito de endereço, podia ser qualquer nÃºmero.
            chama_atualizacao();
          }
        }
      }
      else if (auxiliar == 2) menu--;
      else if (auxiliar == 3) menu ++;

      break;                              // sai da função

    case 15:                            // caso 15 - submenu 15: tarifa

      submenu15();
      auxiliar = testaBotoes();
      if ( auxiliar == 5) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Tarifa");
        lcd.setCursor(0, 2);
        lcd.print("Tar=");
        tarifa = le_EEPROM(96, 97, 98);
        if (tarifa == 0) lcd.print ("0,0000BRL/KWh");
        else {
          lcd.print("0,");
          lcd.print(tarifa);
          lcd.print("BRL/KWh");
        }
        auxiliar = testaBotoes();

        if (auxiliar == 5) {
          lcd.clear();
          aux_3 = alteratarifa();
          //aux_3 = 123456;
          if (aux_1 != -1) {

            Escrever_EEPROM_6d(aux_3, 96, 97, 98 ); //posição 96, 97, 98.  para não dar conflito de endereço, podia ser qualquer nÃºmero.
            chama_atualizacao();
          }
        }
      }

      else if (auxiliar == 2) menu --;
      else if (auxiliar == 3) menu  ++;

      break;                              // sai da função

    case 16:
      submenu16();
      auxiliar = testaBotoes();
      if ( auxiliar == 5) {
        menu_clear();
      }
      else if (auxiliar == 2) menu--;
      else if (auxiliar == 3) menu = 3;

      break;

    case 21:
      submenu21 ();
      break;
  }
}

void rotina_principal() { // atualiza dados de leitura do slave, de potência, gravação de sd, internet e outros.
  unsigned long aux_ct, consumo;
  //unsigned long aux_dia, aux_mes;
  float valor;

  if (atu_lcd == atu_lcd_v) atu_lcd = 0 ;
  atu_lcd++;

  if (at_lcd == atu_lcd_m) at_lcd = 0 ;
  at_lcd++;

  RTC.read(rtc_variavel);                //Busca as informações do relógio

  if (momento != rtc_variavel.Second) {

    for (int i = 0; i < 4; i++) {             //Busca as informações das medições realizadas no Uno
      Y = i;
      Wire.beginTransmission(slaveAdress);
      Wire.write(Y); // envia um byte contendo o estado do LED
      Wire.endTransmission(); // encerra a transmissao

      Wire.requestFrom(slaveAdress, 3);

      if (Wire.available()) {
        int part1 = Wire.read();
        int part2 = Wire.read();
        int part3 = Wire.read();

        valor = le_I2C(part1, part2, part3);
      }
      switch (i) {

        case 0:
          Vrms = valor / 100;
          break;

        case 1:
          Irms = valor / 100;
          if ( Irms <= 0.49) Irms = 0;  // Utilizado para zerar a corrente para valores muito peqnenos
           break;

        case 2:
          fp = valor / 100;
          break;

        case 3:
          sfp = valor;
          break;
      }
    }
    S = Vrms * Irms;
    P = S * fp;

    pontoDemanda (); // Chama a função para armazenas as amostras
     
    if ( sd == 0 && sts_sd == 1) gravacao_sd (); // è para fechar o cartão SD

       if (amostra == auxDemanda) { //Calcula Demanda e Consumo

      demanda = d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9 + d10 + d11 + d12 + d13 + d14 + d15 + d16 + d17 + d18 + d19 + d20 + d21 + d22
                 + d23 + d24 + d25 + d26 + d27 + d28 + d29 + d30 + d31 + d32 + d33 + d34 + d35 + d36 + d37 + d38 + d39 + d40 +
                d41 + d42 + d43 + d44 + d45 + d46 + d47 + d48 + d49 + d50 + d51 + d52 + d53 + d54 + d55 + d56 + d57 + d58 + d59 + d60;

      consumo = demanda;
      demanda = demanda / amostra;
      auxDemanda = 0;
//////SD ///////////////////////////////////////////////////////////////////////////////////////////////////////
if ( sd == 1 || sts_sd == 1) gravacao_sd ();  //  Chama a rotina para armazenar dados no SD

////Internet //////////////////////////////////////////////////////////////////////////////////////////////////
      if ( internet == 1 ) {                          //Chama a rotina para armazenar dados no servidor WEB
        if ( rtc_variavel.Minute != atu_internet) {
          atu_internet = rtc_variavel.Minute;
          gravacao_internet();
        }
      }
      aux_ct = le_EEPROM_8d(19, 20, 21, 22);
      aux_ct = aux_ct + consumo;
      Escrever_EEPROM_8d ( aux_ct, 19, 20, 21, 22); // salva o consumo total medido na EEPROM
    }

    if (aux_ciclo != rtc_variavel.Day) {                  // registra o consumo diario na EEPROM

      EEPROM.write(100, rtc_variavel.Day);                // salva o número do dia na memoria 100da EEPROM (no aux_ciclo)
      aux_dia = le_EEPROM_8d(19, 20, 21, 22) / 1000;
      Escrever_EEPROM_8d (aux_dia, 40, 41, 42, 43);

      if (rtc_variavel.Day == dia_mensal) {              // registra o consumo mensal na EEPROM
        aux_mes = le_EEPROM_8d(19, 20, 21, 22) / 1000;
        Escrever_EEPROM_8d (aux_mes, 30, 31, 32, 33);
      }
      chama_atualizacao();
    }
    momento = rtc_variavel.Second;
  }
}

void chama_atualizacao() {                           // chama atualização de dados na memória e atualiza status de funções

  unsigned long  aux;                                //variavél
  
  imposto_vetor[1] = le_EEPROM(0x00, 0x01, 0x02);    //atualiza o valor do imposto 1
  imposto_vetor[2] = le_EEPROM(0x03, 0x04, 0x05);    //atualiza o valor do imposto 2
  imposto_vetor[3] = le_EEPROM(0x06, 0x07, 0x08);    //atualiza o valor do imposto 3
  sd               = EEPROM.read(94);                //atualiza o status da gravação SD.
  aux_dia = le_EEPROM_8d(40, 41, 42, 43);            //atualiza com base na medição_diaria
  aux_mes = le_EEPROM_8d(30, 31, 32, 33);            //atualiza com base na medição_mês
  internet   = EEPROM.read(95);                      //atualiza o status da internet
  tarifa     = le_EEPROM(96, 97, 98) / 10;           //atualiza o valor da tarifa
  dia_mensal = EEPROM.read(99);                      //atualiza o dia de reset do consumo mensal
  aux_ciclo  = EEPROM.read(100);                     //variavel do sistema 

  if ( digitalRead(reset_cs_total) == HIGH) {              //realiza o reset dos consumos 
    Escrever_EEPROM_8d (00000000000, 19, 20, 21, 22);
    Escrever_EEPROM_8d (00000000000, 40, 41, 42, 43);
    Escrever_EEPROM_8d (00000000000, 30, 31, 32, 33);
  }

  if ( digitalRead(reset_geral) == HIGH) {
       for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
    }
  }
  aux = (1 * 1000000) - ((imposto_vetor[1] + imposto_vetor[2] + imposto_vetor[3])); //1000000 é para trabalhar só com número inteiro. O resultado é 753662
  aux = 10000000000 / aux; //Para corrigir o numero de casas ates da vírigula // o resultado e 13268
   mult_fatura = tarifa * aux;            // o resultado  é 6735500220 ; o real é 0,6735500220
  mult_fatura = mult_fatura / 100000;    //100000 é para reduzir o resultado para 6735
}

void gravacao_sd () { // rotina de gravação no SD

  if (sts_sd == 0) {
    sts_sd = 1;
    Escrever_EEPROM_8d ( 0, 50, 51, 52, 53);
    if (!sdCard.begin(chipSelect, SPI_HALF_SPEED))sdCard.initErrorHalt();    //Inicializa o módulo SD
      if (!meuArquivo.open("medicao.txt", O_RDWR | O_CREAT | O_AT_END))       //Determina o nome do arquivo emtre os parentêses (obrigatório o .txt), ex: "medição.txt" e o abre
    {
      sdCard.errorHalt("Erro na abertura do arquivo .TXT");                  //Imprimi mensagem de erro caso o arquivo não abra
      delay(2000);
      sts_display = 0;
    }

    RTC.read(rtc_variavel);      //Busca as informações de calendário e horário no RTC

    meuArquivo.println();                                                            // Inicio do Cabeçalho do SD 
    meuArquivo.println("////////////////Inicio/////////////////////////");           // Imprimi texto no LCD
    meuArquivo.println("Medidor_de_Energia V.xx");                                   // Imprimi texto no LCD
    meuArquivo.println();                                                            // Pula uma linha
    meuArquivo.print("Data_da_Leitura: ");           // Imprimi texto no LCD

    meuArquivo.print(rtc_variavel.Day);              // Imprimi o dia
    meuArquivo.print("/");                           // Imprimi texto no LCD
    meuArquivo.print(rtc_variavel.Month);            // Imprimi o mês 
    meuArquivo.print("/");                           // Imprimi texto no LCD
    meuArquivo.println((rtc_variavel.Year + 1970));  // Imprimi o ano 
    meuArquivo.print("Hora_da_Leitura: ");           // Imprimi texto no LCD
    meuArquivo.print(rtc_variavel.Hour);             // Imprimi a hora
    meuArquivo.print(":");                           // Imprimi texto no LCD
    meuArquivo.print(rtc_variavel.Minute);           // Imprimi o minuto
    meuArquivo.print(":");                           // Imprimi texto no LCD
    meuArquivo.println((rtc_variavel.Second));       // Imprimi o segundo 

    meuArquivo.print("Intervalo_de_Integração: ");   // Imprimi texto no LCD
    meuArquivo.print(amostra);                       // Imprimi a variavel 
    meuArquivo.println(" s");                        // Imprimi texto no LCD
    meuArquivo.println();                            // Pula uma linha no SD
    meuArquivo.print("Data");                        // Imprimi texto no LCD
    meuArquivo.print(" Horario");                    // Imprimi texto no LCD  
    meuArquivo.print(" Demanda(W)");                 // Imprimi texto no LCD
    meuArquivo.print(" Tensão(V)");                  // Imprimi texto no LCD
    meuArquivo.print(" Corrente(A)");                // Imprimi texto no LCD
    meuArquivo.print(" FP");                         // Imprimi texto no LCD
    meuArquivo.print(" SFP(1-cap;2-ind)");           // Imprimi texto no LCD
    meuArquivo.print(" Consumo(W)");                 // Último item do cabeçalho do SD
    meuArquivo.close();                         //Fecha o SD
  }

  if (!sdCard.begin(chipSelect, SPI_HALF_SPEED))sdCard.initErrorHalt();
  if (!meuArquivo.open("medicao.txt", O_RDWR | O_CREAT | O_AT_END)) //Determina o nome do arquivo emtre os parentêses (obrigatório o .txt), ex: "medição.txt" e o abre
  {
    sdCard.errorHalt("Erro na abertura do arquivo .TXT");
    delay(2000);
    sts_display = 0;
  }

  meuArquivo.println();
  RTC.read(rtc_variavel);
  meuArquivo.print(rtc_variavel.Day);
  meuArquivo.print("/");
  meuArquivo.print(rtc_variavel.Month);
  meuArquivo.print("/");
  meuArquivo.print((rtc_variavel.Year + 1970));
  meuArquivo.print(" ");
  meuArquivo.print(rtc_variavel.Hour);
  meuArquivo.print(":");
  meuArquivo.print(rtc_variavel.Minute);
  meuArquivo.print(":");
  meuArquivo.print(rtc_variavel.Second);
  meuArquivo.print("  ");
  meuArquivo.print(demanda);
  meuArquivo.print("  ");
  meuArquivo.print(Vrms);
  meuArquivo.print("  ");
  meuArquivo.print(Irms);
  meuArquivo.print("  ");
  meuArquivo.print(fp);
  meuArquivo.print("  ");
  meuArquivo.print(sfp);

  meuArquivo.print("  ");
  soma_consumo = demanda * amostra;
  soma_consumo = le_EEPROM_8d(50, 51, 52, 53) + soma_consumo;
  Escrever_EEPROM_8d ( soma_consumo, 50, 51, 52, 53);
  meuArquivo.print(soma_consumo);
  meuArquivo.close();
  sd_atualiza++;

  if (sd == 0 )
  {
    if (!sdCard.begin(chipSelect, SPI_HALF_SPEED))sdCard.initErrorHalt();
    // Abre o arquivo LER_POT.TXT

    if (!meuArquivo.open("medicao.txt", O_RDWR | O_CREAT | O_AT_END))
    {
      sdCard.errorHalt("Erro na abertura do arquivo .TXT");
      delay(2000);
      sts_display = 0;
    }

    meuArquivo.println();
    meuArquivo.println();
    meuArquivo.print("///////////////////Fim /////////////////////");

    lcd.clear();
    lcd.print("Retire o SD!");
    meuArquivo.close();

    EEPROM.write(94, 0);             // muda o sd para 0
    chama_atualizacao();
    sts_sd = 2;
    sd_atualiza = 0;
    delay(10000);
    sts_display = 0;
  }
}

void gravacao_internet() {                //rotina de envio de dados para internet

  ThingSpeak.writeField(myChannelNumber, 1, P, myWriteAPIKey);
}

void telainicial () {                    // tela de introdução

  lcd.setCursor(0, 0);
  lcd.print("Medidor de Energia");
  lcd.setCursor(0, 2);
  lcd.print("versao master v.final");
  delay(4000);
}

void tela_basica() {                        // Menu 0

  unsigned long custo_dia, custo_mes, atu;
  unsigned long aux, aux1, aux_lcd ;

  if (sts_display == 0) {

    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("V");
    lcd.setCursor(19, 1);
    lcd.print("A");
    lcd.setCursor(8, 1);
    lcd.print("f");
    lcd.setCursor(0, 2);
    lcd.print("Custo Dia = ");
    lcd.print("R");
    lcd.write(B00100100);                           //SÍMBOLO DE MOEDA
    lcd.setCursor(0, 3);
    lcd.print("Custo Mes = ");
    lcd.print("R");
    lcd.write(B00100100);                           //SÍMBOLO DE MOEDA
  }

  displayRelogio();                                  //Imprimi o relógio 

  if (at_lcd == atu_lcd_m || sts_display == 0) {

    troca = !troca;
    
    if (troca == true) {

      if (sd == 1) {
        lcd.setCursor(9, 0);
        lcd.write(B00100001);
      }
      if (internet == 1) {
        lcd.setCursor(10, 0);
        lcd.write(B00100011);
      }
      lcd.setCursor(8, 1);
      lcd.print("f");
    }
    else {

      lcd.setCursor(9, 0);
      lcd.print("  ");
      lcd.setCursor(8, 1);
      if (fp != 1) {
        if (sfp == 2)  lcd.write(B00101011);
        if (sfp == 1)  lcd.write(B00101101);
      }
    }
    lcd.setCursor(0, 1);
    lcd.print("      ");
    lcd.setCursor(0, 1);
    aux_lcd = Vrms * 100;
    lcd.print((aux_lcd / 100));
    lcd.print(",");
    lcd.print(aux_lcd % 100);
    lcd.setCursor(9, 1);
    aux_lcd = fp * 100;
    lcd.print("     ");
    lcd.setCursor(9, 1);

    if (aux_lcd == 100) {
      lcd.print(fp);
    }
    else {
      lcd.print("0,");
      lcd.print(aux_lcd % 100);
    }
    lcd.setCursor(14, 1);
    lcd.print("     ");
    lcd.setCursor(14, 1);
    aux_lcd = Irms * 100;
    lcd.print(aux_lcd / 100);
    lcd.print(",");
    lcd.print(aux_lcd % 100);
  }

  if (sts_display == 0 || atu_lcd == atu_lcd_v) {

    aux =  le_EEPROM_8d(19, 20, 21, 22) / 1000;
    custo_dia = ((aux - aux_dia) * aux_kwh) / (1000000);
    custo_dia = (custo_dia * mult_fatura);
    lcd.setCursor (14, 2);
    lcd.print (custo_dia / 10000);
    lcd.print (",00");
     custo_mes = ((aux - aux_mes) * aux_kwh) / (1000000);
    custo_mes = (custo_mes * mult_fatura);
    lcd.setCursor (14, 3);
    lcd.print (custo_mes / 10000);
    lcd.print (",00");
  }
  sts_display = 1;
}


void menu_medicao () {   // Menu 01

  unsigned long consumo_total, aux_ins, aux_total, tt;
  unsigned long lcd_dia, lcd_mes;


  if (sts_display == 0) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cons. Ins.= ");
    lcd.setCursor(0, 1);
    lcd.print("Cons. Dia = ");
    lcd.setCursor(17, 1);
    lcd.print("Wh");
    lcd.setCursor(0, 2);
    lcd.print("Cons. Mes = ");
    lcd.setCursor(17, 2);
    lcd.print("KWh");
    lcd.setCursor(0, 3);
    lcd.print("Cons. Tot.= ");
    lcd.setCursor(17, 3);
    lcd.print("KWh");
  }

  if (sts_display == 0 || atu_lcd == atu_lcd_v) {

    aux_total =  le_EEPROM_8d(19, 20, 21, 22) / 1000; 
    consumo_total = (aux_total * aux_kwh) / (1000000);
    lcd_dia = ((aux_total - aux_dia) * aux_kwh) / (1000);
    lcd_mes = ((aux_total - aux_mes) * aux_kwh) / (1000000);
    lcd.setCursor (12, 1);
    lcd.print (lcd_dia);
    lcd.setCursor (12, 2);
    lcd.print (lcd_mes / 100);
    lcd.print (",");
    lcd.print (lcd_mes % 100);
    lcd.setCursor (12, 3);
    lcd.print (consumo_total);
  }

  lcd.setCursor(12, 0);
  if (at_lcd == atu_lcd_m || sts_display == 0) {

    if ( P < 1000 ) {
      lcd.print (P);
      //lcd.print (aux_ins);
      lcd.setCursor(17, 0);
      lcd.print("W");
    }
    else {
      lcd.print (P / 1000);
      lcd.setCursor(17, 0);
      lcd.print("kW");
    }  
  }
  sts_display = 1;
}

int alterar_gravacao() { //Esta função monta um numero que é utilizado para determninar o status das gravações e SD

int digit_dez, digit_uni, limite_superior, limite_inferior, aux_gravacao;     // variaveis utilizadas

entra_dez:
  digit_dez = ajustaDigitos(0, 1, 19, 2);               // chama uma função para ajustar o dígito da dezena
  if (digit_dez == -1)
    return -1;

entra_uni:
  digit_uni = ajustaDigitos(0, 1, 19, 3);               // chama uma função para ajustar o dígito da unidade
  if (digit_uni == -1)                                  //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return -1;
  else if (digit_uni == -2)                             //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_dez;

 aux_gravacao = (digit_uni) + (digit_dez * 10);         //monta o número
 return aux_gravacao;                                   //retorna o número
}

void menu_imposto() {                                     //busca o valor do imposto

  int auxiliar, aux_alarme, loop1 = 1 , flag;
  impostos = 1;
  while (loop1 == 1) {
    switch (impostos) {

      case 1:
        flag = mostra_menu_imposto (1, 0x00, 0x01, 0x02);
        if (flag == 1)  loop1 = 0;
        break;

      case 2:
        flag = mostra_menu_imposto (2, 0x03, 0x04, 0x05);
        if (flag == 1)  loop1 = 0;
        break;

      case 3:
        flag = mostra_menu_imposto (3, 0x06, 0x07, 0x08);
        if (flag == 1)  loop1 = 0;
        break;

    }
  }
  return;
}

int mostra_menu_imposto(int num_imposto, int end_1, int end_2, int end_3) {    //configura o armazenamento dos impostos 

  int auxiliar, flag = 0, aux_imp;              //variaveis
  unsigned long aux_imposto;                    //variaveis

  lcd.setCursor(0, (num_imposto - 1));
  
  switch (num_imposto) {

    case 1:
      lcd.print("ICMS = ");
      break;

    case 2:
      lcd.print("PIS");
      break;

    case 3:
      lcd.print("COFINS =");
      break;

  }

  if (imposto_vetor[num_imposto] == 0000) {
    lcd.setCursor(15, (num_imposto - 1));
    lcd.print("Vazio");
  }
  else {
    lcd.setCursor(7, (num_imposto - 1));
    print2digits(imposto_vetor[num_imposto] / 10000);
    lcd.print(",");

    print2digits(imposto_vetor[num_imposto] % 10000);

    lcd.print("%");
  }
  auxiliar = testaBotoes ();
  if (auxiliar == 5) {
    aux_imposto = alteraimposto();

    if (aux_imposto == -1) {
      flag = 1;
    }
    else {
      imposto_vetor[num_imposto] = aux_imposto;
      Escrever_EEPROM_6d(imposto_vetor[num_imposto], end_1, end_2, end_3);
      flag = 1;
    }
  }
  else if (auxiliar == 3) {
    impostos = num_imposto + 1;

    if (impostos > 3)
      impostos = 1;
    lcd.clear();
  }

  else if (auxiliar == 2) {
    impostos = num_imposto - 1;
    if (impostos < 1)
      impostos = 3;
    lcd.clear();
  }

  else if (auxiliar == 4) {
    lcd.clear();
    //lcdMenu();
    flag = 1;
  }
  return flag;
}

long alteratarifa() {    //monta o valor da tarifa

/* Esta função monta o valor da tarifa de 6 casas na seguinte ordem: 
 *  1°2°3°4°5°6° 
 */

  int digit_mil, digit_cen, digit_dez, digit_uni, digit_uni_5,    // variaveis utilizadas
      digit_uni_6, limite_superior, limite_inferior, dia_on;     // variaveis utilizadas
  long aux_tarifa;                                               //armazena o valor da tarifa

  lcd.clear();                            // limpa o lcd
  lcd.setCursor(0, 2);                    //posiciona o curssor
  lcd.print("tarifa = 0,");               //Imprimi texto no LCD
                                                                   
entra_mil:
  digit_mil = ajustaDigitos(0, 9, 11, 2);  // chama uma função para ajustar o dígito 1°
  if (digit_mil == -1)
    return -1;

entra_cen:
  digit_cen = ajustaDigitos(0, 9, 12, 2); // chama uma função para ajustar o dígito da unidade da centena
  if (digit_cen == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return -1;
  else if (digit_cen == -2)                //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_mil;

entra_dez:
  digit_dez = ajustaDigitos(0, 9, 13, 2);  // chama uma função para ajustar o dígito 2°
  if (digit_dez == -1)
    return -1;
  else if (digit_dez == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_cen;

entra_uni:
  digit_uni = ajustaDigitos(0, 9, 14, 2); // chama uma função para ajustar o dígito da 3°
  if (digit_uni == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return -1;
  else if (digit_uni == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_dez;

entra_uni_5:
  digit_uni_5 = ajustaDigitos(0, 9, 15, 2); // chama uma função para ajustar o dígito da 4°
  if (digit_uni_5 == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return -1;
  else if (digit_uni == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_uni;

entra_uni_6:
  digit_uni_6 = ajustaDigitos(0, 9, 16, 2); // chama uma função para ajustar o digito da 5°
  if (digit_uni_6 == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return -1;
  else if (digit_uni == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_uni_5;

  aux_tarifa = (  (digit_uni_6 * 1) + (digit_uni_5 * 10) + (digit_uni * 100) + (digit_dez * 1000) + (digit_cen * 10000) + (digit_mil * 100000)); //Monta o valor da tarifa

  return aux_tarifa;
}

unsigned long alteraimposto() {       //monta o valor do imposto
  long digit_imp_uni_1, digit_imp_uni_2, digit_imp_uni_3, digit_imp_uni_4, digit_imp_uni_5, digit_imp_uni_6;
  unsigned  long imposto;

  lcd.clear();
  lcd.setCursor(1, 2);
  lcd.print("xx,xxxx %");

 entra_imp_uni_1:
  digit_imp_uni_1 = ajustaDigitos(0, 9, 0, 1);

  if (digit_imp_uni_1 == -1)
    return -1;

entra_imp_uni_2:
  digit_imp_uni_2 = ajustaDigitos (0, 9, 1, 1);

  if (digit_imp_uni_2 == -1)
    return -1;
  else if (digit_imp_uni_2 == -2)
    goto entra_imp_uni_1;

entra_imp_uni_3:

  digit_imp_uni_3 = ajustaDigitos(0, 9, 2, 1);

  if (digit_imp_uni_3 == -1)
    return -1;
  else if (digit_imp_uni_3 == -2)
    goto  entra_imp_uni_2;

entra_imp_uni_4:

  digit_imp_uni_4 = ajustaDigitos(0, 9, 3, 1);

  if (digit_imp_uni_4 == -1)
    return -1;
  else if (digit_imp_uni_4 == -2)
    goto  entra_imp_uni_3;

entra_imp_uni_5:

  digit_imp_uni_5 = ajustaDigitos(0, 9, 4, 1);

  if (digit_imp_uni_5 == -1)
    return -1;
  else if (digit_imp_uni_5 == -2)
    goto  entra_imp_uni_4;

entra_imp_uni_6:

  digit_imp_uni_6 = ajustaDigitos(0, 9, 5, 1);

  if (digit_imp_uni_6 == -1)
    return -1;
  else if (digit_imp_uni_6 == -2)
    goto  entra_imp_uni_5;

  imposto = digit_imp_uni_1 * 100000 + digit_imp_uni_2 * 10000 + digit_imp_uni_3 * 1000 + digit_imp_uni_4 * 100 + digit_imp_uni_5 * 10 + digit_imp_uni_6 * 1;

  return imposto;

}

int alterar_dia_mes() {         //monta a valor do dia do mês

  int digit_dez, digit_uni, limite_superior, limite_inferior, dia_on;     // variaveis utilizadas
  lcd.clear();                                                            // limpa o lcd
  lcd.setCursor(1, 1);                                                    // Posciona o cursor
  lcd.print("Dia:");                                                      // Imprimi texto no LCD

  // AJUSTE DO DIA
entra_dez:
  digit_dez = ajustaDigitos(0, 3, 6, 1);  // chama uma função para ajustar o digito da dezena do mês (0 - 3)
  if (digit_dez == -1)
    return -1;
  if (digit_dez == 3) limite_superior = 1;  // se a dezena for 3, o limite superior é 1
  else  limite_superior = 9;                // else, o limite superior é 9
entra_uni:
  digit_uni = ajustaDigitos(0, limite_superior, 7, 1); // chama uma função para ajustar o digito da unidade do mês (de 0 -9)
  if (digit_uni == -1)                       //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return -1;
  else if (digit_uni == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_dez;

  dia_on = (digit_uni) + (digit_dez * 10);  //soma a dezena do mês com a uidade do mês
  return dia_on;

}

void submenu11 () {              //texto da tela do submenu Imposto
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("Imposto");
  lcd.setCursor(0, 2);
  lcd.print("Mostrar/Alterar");
}

void submenu12 () {             //texto da tela do submenu Alterar Horario
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Alterar Horario ");
}

void submenu13 () {        //texto da tela do submenu Alterar Data
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Alterar Data");
}

void submenu14 () {    //texto da tela do submenu Alterar dia do ciclo de medicao mensal 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alterar dia do ciclo de medicao mensal");
}


void submenu15 () {   //texto da tela do submenu mostrar ou alterar tarifa
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("tarifa");
  lcd.setCursor(0, 2);
  lcd.print("Mostrar/Alterar");
}

void submenu16() { //texto da tela do submenu apagar consumo

  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.write("Apagar Consumo");
}

void menu_clear() {                   // submenu de reset do consumo mes e consumo dia

  int  aux_1, aux_2, aux_3;
  unsigned long aux_clear;

  lcd.setCursor(0, 2);
  lcd.write("Cons. Dia =");
  lcd.setCursor(0, 3);
  lcd.write("Cons. Mes =");

  aux_1 = alterar_gravacao();

  if (aux_1 != -1) {
    aux_2 = aux_1 / 10;
    aux_3 = aux_1 % 10;

    if (aux_2 == 1 || aux_3 == 1) {
      lcd.clear();
      aux_clear = le_EEPROM_8d(19, 20, 21, 22) / 1000;

      if (aux_2 == 1) {
        Escrever_EEPROM_8d ( aux_clear, 40, 41, 42, 43);

        lcd.setCursor(0, 1);
        lcd.print("consumo dia apagado");
      }

      if (aux_3 == 1) {
        Escrever_EEPROM_8d ( aux_clear, 30, 31, 32, 33);
        lcd.setCursor(0, 2);
        lcd.print("consumo mes apagado");
      }
      delay(5000);
      chama_atualizacao();
    }
  }
  lcd.clear();
}

void submenu21() { // submenu gravação SD e Internet

  int auxiliar, aux_1;

  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.write("Gravacao");
  lcd.setCursor(0, 2);
  lcd.write("Cartao SD =");
  lcd.setCursor(17, 2);

  if (sd == 1) {
    lcd.write("sim");
  }
  else {
    lcd.write("nao");
  }
  lcd.setCursor(0, 3);
  lcd.write("Internet =");
  lcd.setCursor(17, 3);

  if (internet == 1) {
    lcd.write("sim");
  }
  else {
    lcd.write("nao");
  }

  auxiliar = testaBotoes();

  if (auxiliar == 5) {
    lcd.clear();
    lcd.setCursor(0, 6);
    lcd.write("Gravacao");
    lcd.setCursor(0, 2);
    lcd.write("Cartao SD =");
    lcd.setCursor(0, 3);
    lcd.write("Internet =");

    aux_1 = alterar_gravacao();

    if (aux_1 != -1) {

      EEPROM.write(94, (aux_1 / 10)); // gravação no sd(1= sim, 0 = não);
      if (aux_1 == 10) sts_sd = 0; // reseta o cabeçalho no arquivo do sd.

      EEPROM.write(95, (aux_1 % 10));
      chama_atualizacao();
    }
   }
  else if (auxiliar == 4) menu = 2;
  lcd.clear();
}

void menu_gravacao () {        //texto da tela do menu gravação

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("menu gravacao");
}

void menu_configuracao () {    //texto da tela do menu configuração
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("menu configuracao");

}

void alteraData() {                       // função para alterar a data salva no RTC
  int digit_dez, digit_uni, dia, mes, ano, limite_superior, limite_inferior;  // variaveis utilizadas
  lcd.clear();                            // limpa o LCD
  lcd.setCursor(1, 2);
  lcd.print("x.xx.xx  DATA");

  // AJUSTE DO DIA

entra_dez_dia:                           // etiqueta
  digit_dez = ajustaDigitos(0, 3, 0, 1);  // chama uma função para ajustar o digito da dezena do dia (de 01-31)

  if (digit_dez == -1)                   //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar a data
    return;

  if (digit_dez == 3) {                   // se a dezena for 3, significa que os limites tem que ser 0-1
    limite_superior = 1;
    limite_inferior = 0;
  }
  else if (digit_dez == 0) {              // se a dezena for 0, significa que os limites tem que ser 1-9
    limite_superior = 9;
    limite_inferior = 1;
  }
  else  {                                 // para a dezena 2, os limites tem que ser 0-9
    limite_superior = 9;
    limite_inferior = 0;
  }

entra_uni_dia:
  digit_uni = ajustaDigitos(limite_inferior, limite_superior, 1, 1); // ajusta o dígito da unidade do dia (de 01-31)

  if (digit_uni == -1)                  //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar a data
    return;
  else if (digit_uni == -2)            //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_dez_dia;

  dia = (digit_dez * 10) + digit_uni;     // soma o dígito da dezena com o da unidade

  // AJUSTE DO MES

entra_dez_mes:
  digit_dez = ajustaDigitos(0, 1, 3, 1);  // chama uma função para ajustar o dígito da dezena do mes (de 01-12)

  if (digit_dez == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar a data
    return;
  else if (digit_dez == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_uni_dia;

  if (digit_dez == 1) {                   // se a dezena for 1, significa que os limites tem que ser 0-2
    limite_superior = 2;
    limite_inferior = 0;
  }
  else  {                                 // se a dezena for 0, significa que os limites tem que ser 1-9
    limite_superior = 9;
    limite_inferior = 1;
  }

entra_uni_mes:
  digit_uni = ajustaDigitos(limite_inferior, limite_superior, 4, 1); // ajusta o dígito da unidade do dia (de 01-31)

  if (digit_uni == -1)                  //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar a data
    return;
  else if (digit_uni == -2)            //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_dez_mes;

  mes = (digit_dez * 10) + digit_uni;     // soma o dígito da dezena com o da unidade

  // AJUSTE DO ANO
entra_dez_ano:
  digit_dez = ajustaDigitos(0, 9, 6, 1);  // chama a função para ajustar a dezena do ano (00 - 99) não tem restrições

  if (digit_dez == -1)                   //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar a data
    return;
  else if (digit_dez == -2)              //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_uni_mes;

  digit_uni = ajustaDigitos(0, 9, 7, 1);  // chama a função para ajustar a unidade do ano (00 - 99)

  if (digit_uni == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar a data
    return;
  else if (digit_uni == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_dez_ano;

  ano = (digit_dez * 10) + digit_uni;     // soma o digito da dezena com o da unidade

  RTC.read(rtc_variavel);                 // faz a leitura dos valores do RTC pois algumas variaveis não serão alteradas
  rtc_variavel.Day = dia;                 // altera o valor do dia no RTC
  rtc_variavel.Month = mes;               // altera o valor do mes no RTC
  rtc_variavel.Year = CalendarYrToTm(2000 + ano);  // alterar o valor do ano no RTC
  RTC.write(rtc_variavel);                // escreve os valores alterados de volta no RTC
}
void alterarHorario() {                   // funçãoo para alterar o horario salvo no RTC
  int digit_dez, digit_uni, horas, minutos, segundos, limite_superior, limite_inferior;     // variaveis utilizadas
  lcd.clear();                            // limpa o lcd
  lcd.setCursor(1, 2);
  lcd.print("x:xx:xx HORARIO");

  // AJUSTE DA HORA
entra_dez_hora:
  digit_dez = ajustaDigitos(0, 2, 0, 1);  // chama uma função para ajustar o digito da dezena das horas (de 00-23)

  if (digit_dez == -1)
    return;

  if (digit_dez == 2) limite_superior = 3;// se a dezena for 2, o limite superior é 3
  else  limite_superior = 9;              // se a dezena for 0 ou 1, o limite superior é 9

entra_uni_hora:
  digit_uni = ajustaDigitos(0, limite_superior, 1, 1); // chama uma função para ajustar o digito da unidade das horas (de 00-23)

  if (digit_uni == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return;
  else if (digit_uni == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_dez_hora;

  horas = (digit_dez * 10) + digit_uni;   // soma os digitos da unidade e da dezena

  // AJUSTE DOS MINUTOS
entra_dez_min:
  digit_dez = ajustaDigitos(0, 5, 3, 1);  // chama uma funçao para ajustar o digito da dezena dos minutos (de 00-59)

  if (digit_dez == -1)                   //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return;
  else if (digit_dez == -2)              //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_uni_hora;

entra_uni_min:
  digit_uni = ajustaDigitos(0, 9, 4, 1);  // chama uma função para ajustar o digito da unidade dos minutos (de 00-59)

  if (digit_uni == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return;
  else if (digit_uni == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_dez_min;

  minutos = (digit_dez * 10) + digit_uni; // soma os digitos da unidade e da dezena

  // AJUSTE DOS SEGUNDOS
entra_dez_seg:
  digit_dez = ajustaDigitos(0, 5, 6, 1);  // chama uma função para ajustar o digito da dezena dos segundos (de 00-59)

  if (digit_dez == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return;
  else if (digit_dez == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_uni_min;

  digit_uni = ajustaDigitos(0, 9, 7, 1);  // chama uma função para ajustar o digito da dezena dos segundos (de 00-59)

  if (digit_uni == -1)                    //  checa se ocorreu um erro, se o ocorreu um erro, sai da funcao sem ajustar o horario
    return;
  else if (digit_uni == -2)               //  checa se o usuario quer voltar para ajustar o digito anterior, se sim, volta para o digito anterior
    goto entra_dez_seg;

  segundos = (digit_dez * 10) + digit_uni;// soma os digitos da unidade e da dezena

  RTC.read(rtc_variavel);                 // faz a leitura dos valores do RTC pois algumas variaveis não serão alteradas
  rtc_variavel.Hour = horas;              // altera o valor das horas no RTC
  rtc_variavel.Minute = minutos;          // altera o valor dos minutos no RTC
  rtc_variavel.Second = segundos;         // altera o valor dos segundos no RTC
  RTC.write(rtc_variavel);                // escreve os valores alterados de volta no RTC
  lcd.clear();                            // limpa o lcd
}

void displayRelogio() {                   // mostra o relogio no display
  RTC.read(rtc_variavel);                   // Lê os valores de cada variavel do relogio

  if (sts_display == 0) {
    lcd.setCursor(2, 0);
    lcd.print(':');                           // imprime :
    lcd.setCursor(5, 0);
    lcd.print(':');                           // imprime :
    lcd.setCursor(14, 0);
    lcd.print('/');                           // imprime /
    lcd.setCursor(17, 0);
    lcd.print('/');                           // imprime /
  }
  lcd.setCursor(0, 0);                      // move o cursor para a posição da primeira linha e primeira coluna do display
  print2digits(rtc_variavel.Hour);          // imprime os dois digitos das horas
  lcd.setCursor(3, 0);                      // move o cursor para a posiçao da primeira linha e primeira coluna do display
  print2digits(rtc_variavel.Minute);        // imprime os dois digitos dos minutos
  lcd.setCursor(6, 0);
  print2digits(rtc_variavel.Second);        // imprime os dois digitos dos segundos
  lcd.setCursor(12, 0);                      // move o cursor para a posição da segunda linha e primeira coluna do display
  print2digits(rtc_variavel.Day);           // imprime os dois digitos do dia do mes
  lcd.setCursor(15, 0);
  print2digits(rtc_variavel.Month);         // imprime os dois digitos do mês
  lcd.setCursor(18, 0);
  print2digits(tmYearToY2k(rtc_variavel.Year)); // imprime os dois ultimos digitos do ano
 }
int ajustaDigitos( int limite_inf, int limite_sup, int posicao_coluna, int posicao_linha) { // para alterar um digito no display
  int loop_inf = 1, digito, erro = 0, volta = 0;                 // variaveis utilizadas                        
  digito = limite_inf;                                // o digito impresso começa com o limite inferior                                                       
  lcd.setCursor(0, 0);
  lcd.print("-  seleciona  +");                     // aperta direita ou esquerda para ajustar o digito.
  lcd.setCursor(posicao_coluna, posicao_linha);     // de acordo com a posição do digito no display
  lcd.print(digito);                                // imprime o digito
  while (loop_inf) {                                // fica no loop enquanto o usuario não selecionar o digito
    lcd.setCursor(posicao_coluna, posicao_linha);     // de acordo com a posiçãoo do digito no display
    switch (testaBotoes()) {                        // chama a funçãoo para ler a porta A0 e retornar o botao pressionado
      case 1: digito++; break;                        // botão 1 (direita) incrementa o digito
      case 4: digito--; break;                        // botão 4 (esquerda) decrementa o digito
      case 2: erro = 1; loop_inf = 0; break;          // botão 2 (para cima) para sair do ajuste digito com erro
      case 3: volta = 1; loop_inf = 0; break;         // botão 3 (para baixo) sai da funcao e volta para re-ajustar o digito anterior
      case 5: loop_inf = 0; break;                    // botão 5 (select) sai da funcao
      default: ;
    }
    if (digito > limite_sup) digito = limite_inf;    // no incremento, se o digito chegar ao limite superior, o proximo será o limite inferior
    if (digito < limite_inf) digito = limite_sup;    // no decremento, se o digito chegar ao limite inferior, o proximo será o limite superior
    lcd.print(digito);                               // imprime o digito
  }
  if (erro == 1) {                                   // se a flag de erro esta alta, retorna da funcao -1 para indicar o erro, e imprime na tela que o ajuste nao foi completado.
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ajuste nao");
    lcd.setCursor(0, 1);
    lcd.print("implementado");
    delay(5000);
    return -1;
  }
  else if (volta == 1) {                            // indica que o usuario quer voltar para ajustar o digito anterior, por isso colo de volta o x no ponto onde estava o cursor
    lcd.setCursor(posicao_coluna, posicao_linha);
    lcd.print('x');
    return -2;
  }
  else
    return digito;                                   // retorna o digito selecionado
}

int testaBotoes() {                                 // função que testa o pino A0 para identificar qual botao foi pressionado
  //botao 1 = direita
  //botao 2 = cima
  //botao 3 = baixo
  //botao 4 = esquerda
  //botao 5 select
  int valbotao, botao1, loop1 = 1;                  // variaveis utilizadas
  while (loop1) {                                   // fica no loop até um botao ser pressionado
    delay(100);                                     // intervalo
    valbotao = analogRead(0);                       // leitura do valor da porta analogica A0
    if (valbotao < 80) {                            // se a leitura da porta A0 for menor que 80
      while (valbotao < 80) {                       // espera o usuario soltar o botao
        valbotao = analogRead(0);
        delay(20);
      }
      loop1 = 0;
      botao1 = 1;                                   // botão 1 pressionado - sai do loop
    }
    else if (valbotao >= 80 && valbotao < 180) {    // se a leitura da porta A0 for entre 80 - 179
      while (valbotao >= 80 && valbotao < 180) {    // espera o usuario soltar o botão
        valbotao = analogRead(0);
        delay(20);
      }
      loop1 = 0;
      botao1 = 2;                                   // botão 2 pressionado - sai do loop
    }
    else if (valbotao >= 180 && valbotao < 350) {   // se a leitura da porta A0 for entre 180 - 349
      while (valbotao >= 180 && valbotao < 350) {   // espera o usuario soltar o botao
        valbotao = analogRead(0);
        delay(20);
      }
      loop1 = 0;
      botao1 = 3;                                   // botão 3 pressionado - sai do loop
    }
    else if (valbotao >= 350 && valbotao < 875) {   // se a leitura da porta A0 for entre 350 - 874
      while (valbotao >= 350 && valbotao < 875) {   // espera o usuario soltar o botão
        valbotao = analogRead(0);
        delay(20);
      }
      loop1 = 0;
      botao1 = 4;                                   // botão 4 pressionado - sai do loop
    }
    else if (valbotao >= 875 && valbotao < 890) {   // se a leitura da porta A0 for entre 875 - 889
      while (valbotao >= 875 && valbotao < 890) {   // espera o usuario soltar o botão
        valbotao = analogRead(0);
        delay(20);
      }
      loop1 = 0;
      botao1 = 5;                                   // botão 5 pressionado - sai do loop
    }
  }
  return botao1;                                    // retorna o botão pressionado
}

void print2digits(int number) {           // função para imprimir o 0 à esquerda caso o número seja menor que 10

  if (number >= 0 && number < 10)         // testa se o número é menor que 10
    lcd.print('0');                       // se ele for, imprime um 0 antes do número
  lcd.print(number);                      // se ele não for, imprime somente o número
}

void  pontoDemanda () {  // Armazena as amostras de potência ativa

  auxDemanda ++;

  switch (auxDemanda) {  

    case 1:
      d1 = P;
      break;

    case 2:
      d2 = P;
      break;

    case 3:
      d3 = P;
      break;

    case 4:
      d4 = P;
      break;

    case 5:
      d5 = P;
      break;

    case 6:
      d6 = P;
      break;

    case 7:
      d7 = P;
      break;

    case 8:
      d8 = P;
      break;

    case 9:
      d9 = P;
      break;

    case 10:
      d10 = P;
      break;

    case 11:
      d11 = P;
      break;

    case 12:
      d12 = P;
      break;

    case 13:
      d13 = P;
      break;

    case 14:
      d14 = P;
      break;

    case 15:
      d15 = P;
      break;

    case 16:
      d16 = P;
      break;

    case 17:
      d17 = P;
      break;

    case 18:
      d18 = P;
      break;

    case 19:
      d19 = P;
      break;

    case 20:
      d20 = P;
      break;

    case 21:
      d21 = P;
      break;

    case 22:
      d22 = P;
      break;

    case 23:
      d23 = P;
      break;

    case 24:
      d24 = P;
      break;

    case 25:
      d25 = P;
      break;

    case 26:
      d26 = P;
      break;

    case 27:
      d27 = P;
      break;

    case 28:
      d28 = P;
      break;

    case 29:
      d29 = P;
      break;

    case 30:
      d30 = P;
      break;

    case 31:
      d31 = P;
      break;

    case 32:
      d32 = P;
      break;

    case 33:
      d33 = P;
      break;

    case 34:
      d34 = P;
      break;

    case 35:
      d35 = P;
      break;

    case 36:
      d36 = P;
      break;

    case 37:
      d37 = P;
      break;

    case 38:
      d38 = P;
      break;

    case 39:
      d39 = P;
      break;

    case 40:
      d40 = P;
      break;

    case 41:
      d41 = P;
      break;

    case 42:
      d42 = P;
      break;

    case 43:
      d43 = P;
      break;

    case 44:
      d44 = P;
      break;

    case 45:
      d45 = P;
      break;

    case 46:
      d46 = P;
      break;

    case 47:
      d47 = P;
      break;

    case 48:
      d48 = P;
      break;

    case 49:
      d49 = P;
      break;

    case 50:
      d50 = P;
      break;

    case 51:
      d51 = P;
      break;

    case 52:
      d52 = P;
      break;

    case 53:
      d53 = P;
      break;

    case 54:
      d54 = P;
      break;

    case 55:
      d55 = P;
      break;

    case 56:
      d56 = P;
      break;

    case 57:
      d57 = P;
      break;

    case 58:
      d58 = P;
      break;

    case 59:
      d59 = P;
      break;

    case 60:
      d60 = P;
      break;
  }
}

long le_I2C(long part11, long  part22, long  part33) { // recebe as tres dezenas do Uno e recomenta informação

  long aux;

  aux = (part11 * 65536) + part22 * 256 + part33;

  return aux;
}

long le_EEPROM(int end_1, int end_2, int end_3) { //remonta uma informação que foi quebrada em 3 pedaços e armzenada na eeprom
  int  par_2, par_3;
  long par_1, valor;

  par_1 = EEPROM.read(end_1);  //
  par_2 = EEPROM.read(end_2);  //
  par_3 = EEPROM.read(end_3);  //


  valor = (par_1 * 10000) + (par_2 * 100) + par_3 * 1;
   return valor;
}

void Escrever_EEPROM(int numero, int end_1, int end_2) { //quebra uma informação em 2 pedaço e salva na eeprom
  // numero salva em porcentagens, xx,xx %.
  int part1, part2;

  part1 = numero / 100;  // part1 = horas (dezenas e unidades)
  part2 = numero % 100;  // part2 = minutos (unidecimos e decimos)

  EEPROM.write(end_1, part1);  // salva a parte 1 no endereco end_1
  EEPROM.write(end_2, part2);  // salva a parte 2 no endereco end_2
}

void Escrever_EEPROM_6d ( long numero, int end_1, int end_2, int end_3) { //quebra uma informação em 3 pedaços e salva na eeprom
  // numero exemplo 123456
  int part1, part2, part3;

  part1 = (numero / 10000); // part1 = 12
  part2 = ((numero % 10000) / 100); // part2 = 34
  part3 = ((numero % 10000) % 100); // par3 =  56
  EEPROM.write(end_1, part1);  // salva a parte 1 no endereco end_1
  EEPROM.write(end_2, part2);  // salva a parte 2 no endereco end_2
  EEPROM.write(end_3, part3); // salva a parte 2 no endereco end_3
}

void Escrever_EEPROM_8d ( unsigned long numero, int end_0, int end_1, int end_2, int end_3) { //quebra uma informação em 4 pedaços e salva na eeprom
  // numero exemplo 12345678
  int part0, part1, part2, part3;

  part0 = (numero / 1000000); // part0 =12
  part1 = (numero % 1000000) / 10000; // part1 = 34
  part2 = ((numero % 1000000) % 10000) / 100; // part2 = 56
  part3 = ((numero % 1000000) % 10000) % 100; // par3 =  78

  EEPROM.write(end_0, part0);  // salva a parte 0 no endereco end_0
  EEPROM.write(end_1, part1);  // salva a parte 1 no endereco end_1
  EEPROM.write(end_2, part2);  // salva a parte 2 no endereco end_2
  EEPROM.write(end_3, part3); // salva a parte 2 no endereco end_3
}

unsigned long le_EEPROM_8d(int end_0, int end_1, int end_2, int end_3) { //remonta uma informação que foi quebrada em 4 pedaços e armzenada na eeprom

  int  par_3;
  unsigned long par_1, par_0, par_2, valor;

  par_0 = EEPROM.read(end_0);  //
  par_1 = EEPROM.read(end_1);  //
  par_2 = EEPROM.read(end_2);  //
  par_3 = EEPROM.read(end_3);  //

  valor = (par_0 * 1000000) + (par_1 * 10000) + (par_2 * 100) + par_3 * 1;
  return valor;
  }


