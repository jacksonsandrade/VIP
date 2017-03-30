/*
   VIP - Biblioteca para o medidor de energia via arduino Due
  Criado por jackson S. Andrade, 24/03/2017
  A  partir da biblioteca Emon.h criada por Trystan Lea.
  Recomenda-se a leitura do trabalho do Lea em https://learn.openenergymonitor.org/electricity-monitoring/ac-power-theory/introductionhttps://learn.openenergymonitor.org/electricity-monitoring/ac-power-theory/introduction
  Também recomenda-se a leitura do meu trabalho em https://github.com/jacksonsandrade/VIP

  A VIP é uma versão otimizada da Emonlib para minha aplicação,
  onde foram retiradas as linhas de programação não utilizadas
  e possui comentários em alguns pontos em português do Brasil,
  principalmente naqueles em que fiz alterações, porém com o
  objetivo de não distorcer o trabalho original com possíveis
  traduções incorretas, foi mantido os comentários originais,
  em inglês.
*/


#ifndef VIP_h
#define VIP_h

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#else

#include "WProgram.h"

#endif

// define theoretical vref calibration constant for use in readvcc()
// 1100mV*1024 ADC steps http://openenergymonitor.org/emon/node/1186
// override in your code with value for your specific AVR chip
// determined by procedure described under "Calibrating the internal reference voltage" at
// http://openenergymonitor.org/emon/buildingblocks/calibration
#ifndef READVCC_CALIBRATION_CONST
#define READVCC_CALIBRATION_CONST 1126400L
#endif

// to enable 12-bit ADC resolution on Arduino Due, 
// include the following line in main sketch inside setup() function:
//  analogReadResolution(ADC_BITS);
// otherwise will default to 10 bits, as in regular Arduino-based boards.
#if defined(__arm__)
#define ADC_BITS    12
#else
#define ADC_BITS    10
#endif

#define ADC_COUNTS  (1<<ADC_BITS)


class VIP
{
  public:

    void voltage(unsigned int _inPinV, double _VCAL, double _PHASECAL);
    void current(unsigned int _inPinI, double _ICAL);

   // void voltageTX(double _VCAL, double _PHASECAL);
 //   void currentTX(unsigned int _channel, double _ICAL);

    void calcVI(unsigned int crossings, unsigned int timeout);
    double calcIrms(unsigned int NUMBER_OF_SAMPLES);
    void serialprint();

    long readVcc();
    //Useful value variables
    double apparentPower,
           Vrms,
           Irms,
           realPower,
           powerFactor;
    //----------------------------------------------------------------------
    
    //Adicionail
    //-------------------------------------------------------------------
    int sfp ;                                             //Sinal do fator de potencia ----> 1 = positivo, 2 = negativo. 
    //        

  private:

    //Set Voltage and current input pins
    unsigned int inPinV;
    unsigned int inPinI;
    //Calibration coefficients
    //These need to be set in order to obtain accurate results
    double VCAL;
    double ICAL;
    double PHASECAL;

    //--------------------------------------------------------------------------------------
    // Variable declaration for emon_calc procedure
    //--------------------------------------------------------------------------------------
	int sampleV;  							 //sample_ holds the raw analog read value
	int sampleI;                     

	double lastFilteredV,filteredV;          //Filtered_ is the raw analog value minus the DC offset
	double filteredI;                  
	double offsetV;                          //Low-pass filter output
	double offsetI;                          //Low-pass filter output               

	double phaseShiftedV;                             //Holds the calibrated phase shifted voltage.

	double sqV,sumV,sqI,sumI,instP,sumP;              //sq = squared, sum = Sum, inst = instantaneous

	int startV, startI;                                       //Instantaneous voltage at start of sample window.

	boolean lastVCross, checkVCross;                  //Used to measure number of times threshold is crossed.






};

#endif

