#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#include "common.h"
#include "LCD.h"

/*
 * Berke Sina Ahlatc?
 * At?nç Utku Alparslan
 * Mustafa Mert Köse
 * Murat Akkoyun
 * Group65
 * 
 * Merhablar hocam, 2 gün boyunca sadece karakter göndermeye ve almaya u?ra?t?k ama ba?ar?l? olamad?k.
 * Ödevin son günü Merve hocam?z laba u?rad???nda bu durumu ona ilettik ama yine de çözüm bulamad?k ve kendisi bize yeni bir board verdi.
 * Sonras?nda denedi?imizde yeni boardun çal??t???n? ve eski boardun hatal? çal??t???n? farkettik ve 4 saatte bu kadar?n? yapabildik.
 * Elimizde olmayan bu sebeplerden dolay? ödevimizde bu kadar sorun ya?amak bizi çok üzdü. Ödeve baya u?ra?mak istiyorduk ama bu durum bizi çok demorilize etti.
 * Grup olarak Ankara'da daha fazla duramayac???m?z için ödeve daha fazla devam edemeyece?iz.
 * Lütfen de?erlendirirken bu durumumuzu göz önünde bulundurursan?z çok seviniriz. Te?ekkürler, iyi günler.
 */

/**********************************************************************
 * Definition dedicated to the local functions.
 **********************************************************************/
#define DEFAULT_MODE       0

extern char transmitBuffer[20];
extern char receiveBuffer[20];
extern char rcvd_value;
extern char tsmt_value;
extern int receiveIndex;
extern int transmitIndex;
extern int lcdState;
extern unsigned int money;
extern unsigned int happy;
extern unsigned int hunger;
extern unsigned int thirst;
extern int receiveFlag;
extern int goFlag;
extern int inTransmit;
extern union Timers Tmr0;

/**********************************************************************
 * Function prototypes.
 **********************************************************************/
void main (void);
void Init(void);
void StartupHook(void);
void ShutdownHook(StatusType error);
void ErrorHook(StatusType error);
void PreTaskHook(void);
void PostTaskHook(void);


AppModeType SelectedMode;

/**********************************************************************
 * -------------------------- main function ---------------------------
 *
 * Setup the different alarms and start the kernel.
 *
 **********************************************************************/
void main(void)
{
  STKPTR = 0;
  SelectedMode = DEFAULT_MODE;
  Init();
  
  while(1) {
    StartOS(SelectedMode);
  }
}

 void Init(void)
{
    //Initialize essential variables
    // Used char '-' for default value.
     int i;
    for(i=0;i <20 ;i++){
        transmitBuffer[i]='-';
        receiveBuffer[i]='-';
    }
    receiveFlag=0;
    receiveIndex=0;
    transmitIndex=0;
    goFlag=0;
    inTransmit=0;
    //
    
	FSR0H = 0;
	FSR0L = 0;
	
	/* User setting : actual PIC frequency */
	Tmr0.lt = _40MHZ;
	
	/* Timer OFF - Enabled by Kernel */
	T0CON = 0x08;
	TMR0H = Tmr0.bt[1];
	TMR0L = Tmr0.bt[0];
	
	/* configure I/O ports */
    TRISCbits.RC7 = 1; // TX1 and RX1 pin configuration
    TRISCbits.RC6 = 0;
    
	
	/* configure USART transmitter/receiver */
	SPBRG1 = 21;		// for 40 MHz, to have 115200 baud rate, it should be 21
	TXSTA1 = 0x04;      // (= 00000100) 8-bit transmit, transmitter NOT enabled,TXSTA1.TXEN not enabled!
                        // asynchronous, high speed mode
	RCSTA1 = 0x90;      // (= 10010000) 8-bit receiver, receiver enabled,
                        // continuous receive, serial port enabled RCSTA.CREN = 1
	
	/* configure the interrupts */
	INTCON = 0;			// clear interrupt register completely
	PIE1bits.TX1IE = 1;	// enable USART transmit interrupt
	PIE1bits.RC1IE = 1;	// enable USART receive interrupt
	PIR1 = 0;			// clear all peripheral flags
	
	INTCONbits.PEIE = 1;// enable peripheral interrupts
	INTCONbits.GIE = 1;	// globally enable interrupts
}
/**********************************************************************
 * Clear all RAM memory and set PORTB to output mode.
 *
 * @return void
 **********************************************************************/


/**********************************************************************
 * Hook routine called just before entering in kernel.
 *
 * @param error      IN The new error stored in buffer
 * @return error     Error level
 **********************************************************************/
void StartupHook(void)
{
}

/**********************************************************************
 * Hook routine called just after leaving the kernel.
 *
 * @param error      IN The last error detected by OS
 * @return void
 **********************************************************************/
void ShutdownHook(StatusType error)
{
}

/**********************************************************************
 * Store a new error in a global buffer keeping a track of the 
 * application history.
 *
 * @param error      IN The new error stored in buffer
 * @return void
 **********************************************************************/
void ErrorHook(StatusType error)
{
}

/**********************************************************************
 * Hook routine called just before entering in a task.
 *
 * @return void
 **********************************************************************/
void PreTaskHook(void)
{
}

/**********************************************************************
 * Hook routine called just after leaving a task.
 *
 * @return void
 **********************************************************************/
void PostTaskHook(void)
{
}

/* End of File : main.c */
