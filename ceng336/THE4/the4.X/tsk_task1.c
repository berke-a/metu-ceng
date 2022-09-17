#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/
extern char receiveBuffer[20];
extern int receiveFlag;
extern char transmitBuffer[20];
extern unsigned int hunger;
extern unsigned int thirst;
extern unsigned int happy;
extern unsigned int money;
extern unsigned int transmitIndex;
extern unsigned int inTransmit;
extern int goFlag;

/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/


TASK(TASK1) 
{
    PIE1bits.RC1IE = 1;	// enable USART receive interrupt
    PIE1bits.TX1IE = 1;	// enable USART transmit interrupt
    
    //We set an alarm in order to send Check command in fixed intervals.
    SetRelAlarm(ALARM_TSK1, 100, 50);
    
	while(1) {
        
        // Wait for alarm.
        WaitEvent(ALARM_EVENT);
        ClearEvent(ALARM_EVENT);
        
        // If the Tamagotchi is not in IDLE mode then send Check command.
        if(goFlag && !inTransmit){
            transmitBuffer[0]='{';
            transmitBuffer[1]='C';
            transmitBuffer[2]='}';
            TXSTA1bits.TXEN=1;
        }
        
        if(receiveFlag==1) {
            
            //If a command is received, check if it is GO. If so Set IDLE_EVENT and goFlag.
            
            receiveFlag=0;
            
            if(!goFlag){
                if(receiveBuffer[2]=='G' && receiveBuffer[3]=='O'){
                    money=(receiveBuffer[4]<<8) | (receiveBuffer[5]);
                    goFlag=1;
                    SetEvent(TASK0_ID,IDLE_EVENT);
                }
                else{
                    continue;
                }
            }
             
        }
        
	}
     
	TerminateTask();
}

/* End of File : tsk_task1.c */