
/*Orientação
 * 
 * Este programa é parte constituinte de um protótipo de medidor de energia elétrica monófasico, para obter as
 * demais partes, assim como informações complementares, lêr o arquivo README armazenado no mesmo diretório deste
 * programa
 *
 * O programa está esquematizado da seguintes maneira:
 * 
 * 1° Variaveis do programa editaveis: contém todas as variaveis livres de edição pelo usuario; 
 * 2° bibliotecas: Contém as bibliotecas e suas respectivas configurações 
 * 3° Variaveis do programa não editaveis: são as variaveis inerentes ao programa, no qual recomenda-se atenção caso deseje-se altera-las, pois 
 *                                        podem tornar o programa instavel.
 * 4° O programa em si.                                       
 */
 
 
//BIBLIOTECAS////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "VIP.h"             // Inclui a Biblioteca VIP
VIP emon1;                   // Cria uma instância
#include "Wire.h"            // Inclui a Biblioteca Wire é a utlizada para a comunicação I2c
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//VARAVEIS DO PROGRAMA EDITAVEIS////////////////////////////////////////////////////////////////////////////////////////
#define myAdress 0x08 // endereco do Uno como slave.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//VARAVEIS DO PROGRAMA NÃO EDITAVEIS//////////////////////////////////////////////////////////////////////////////////
long aux;
int digito;
long aux2;
boolean ledState = LOW;  //Determina o estado do LED
long realPower;         //armazena a potência ativa
long apparentPower;    //armazena a potência aparente
float powerFactor;     //armazena o fator de potência
float Vrms;            //armazena a tensão rms
float Irms;            //armazena a potência ativa
int sfp;               //armazena o sinal do fator de potência
void receiveEvent();
void requestEvent();
void escrever_I2C(long);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {

  Wire.begin(myAdress);             // configura o barramento I2C com o endereço do Uno
  Wire.onRequest(requestEvent);     // Função ativada quando o Master solicita uma informação.
  Wire.onReceive(receiveEvent);     // Função ativada quando o Master envia uma informação
  emon1.voltage(0, 142, 1.73);     // Tensão: pino de entrada, calibração da tensão, calibração do fator de potência
  emon1.current(1, 125);           // Current: pino de corrente, calibração.

  pinMode(13, OUTPUT);            //Seleciona o pino 13 como sinalizador de ocorrência de comunicação
}

void loop() {                     //rotina normal do programa (TFC- identifcado como primeiro programa)

  emon1.calcVI(20, 2000);        //Entra na rotina da biblioteca VIP, para realização as medições
  powerFactor     = emon1.powerFactor;      //estrai o valor dentro da biblioteca VIP para a variavel
  Vrms            = emon1.Vrms;             //estrai o valor dentro da biblioteca VIP para a variavel
  Irms            = emon1.Irms;             //estrai o valor dentro da biblioteca VIP para a variavel
  sfp             = emon1.sfp;              //estrai o valor dentro da biblioteca VIP para a variavel

 }

void receiveEvent() {        // Rotina ativada quando o Mega envia uma informação ao Uno
  if (Wire.available()) {    // se a comunicação estiver disponível
    digito = Wire.read();       // recebe a varivavel Y do Mega
    switch (digito) {                                // Y = ?
      case 0:
        aux2 = Vrms * 100;
        aux = aux2;                                  //aux = Vrms
        break;

      case 1:
        aux2 = Irms * 100;
        aux = aux2;                                  //aux = Vrms
        break;

      case 2:
        aux2 = powerFactor * 100;
       // if ( aux2 >= 99) aux2 = 100;               // opcional
        aux = aux2;                                  //aux = Vrms
        break;                                       

      case 3:
        aux = sfp;                                  //aux = sinal do fator de potência
        break;
    }
  }
}

void requestEvent() {      //Rotina ativada quando o Mega solicita uma informação ao Uno

  ledState = !ledState;              //
  digitalWrite(13, ledState);        // Muda o status do LED (ligado ou desligado)
  escrever_I2C(aux);                 // chama a função para enviar a informação ao Mega

}

void escrever_I2C(long valor) {                  //função que desmonta e envia a informação ao Mega

  int part1, part2, part3;                      //variaveis
                                                //exemplo numero 123456
  part1 = (valor / 65536);                      // part1 = 12
  part2 = ((valor % 65536) / 256);              // part2 = 34
  part3 = ((valor % 65536) % 256);              // par3 =  56

  Wire.write(part1);                            //envia 12
  Wire.write(part2);                            //envia 34
  Wire.write(part3);                            //envia 56

}




