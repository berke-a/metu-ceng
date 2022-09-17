#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/
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

extern int goFlag;

/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/

TASK(TASK0) 
{
    int i,j=0;
    char str[8], hash[16];
	//SetRelAlarm(ALARM_TSK0, 100, 5000);
    
    PIE1bits.RC1IE = 1;	// enable USART receive interrupt
    PIE1bits.TX1IE = 1;	// enable USART transmit interrupt
	
    // Init variables
    hunger=100;
    thirst=100;
    happy=100;
    
    // Wait for the GO command
    WaitEvent(IDLE_EVENT);
    ClearEvent(IDLE_EVENT);

	while(1) {
        WaitEvent(ACTIVE_EVENT);
        ClearEvent(ACTIVE_EVENT);
        
        if(!goFlag) continue;
        
        // receiveBuffer characters starts at index 2
        if(receiveBuffer[2]=='S'){
            
            // Get Status from S command and insert values accordingly.
            
            hunger = receiveBuffer[3];
            happy = receiveBuffer[4];
            thirst = receiveBuffer[5];
                        
            if((happy<=0 || hunger<=0) || thirst<=0){
                goFlag=0;
                continue;
            }
            
        }
        
        
        /**
         * If the funds are enough for the operations, send appropriate command according to cat's needs.
         * We check if the status values are below the threshold in order to not waste money.
         */
        if(hunger<40 && money>=80){
            money-=80;
            transmitBuffer[0]='{';
            transmitBuffer[1]='F';
            transmitBuffer[2]='}';
            TXSTA1bits.TXEN=1;
            continue;
        }
        if(thirst<50 && money>=30){
            money-=30;
            transmitBuffer[0]='{';
            transmitBuffer[1]='W';
            transmitBuffer[2]='}';
            TXSTA1bits.TXEN=1;
            continue;
        }
        if(happy<20 && money>=150){
            money-=150;
            transmitBuffer[0]='{';
            transmitBuffer[1]='P';
            transmitBuffer[2]='}';
            TXSTA1bits.TXEN=1;
            continue;
        }
        
           
	}
	TerminateTask();
}


/* End of File : tsk_task0.c */