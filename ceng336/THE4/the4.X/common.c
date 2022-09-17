#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 * We implemented our own RingBuffer's.
 **********************************************************************/
 char transmitBuffer[20];
 char receiveBuffer[20];
 char rcvd_value;
 char tsmt_value;
 int receiveIndex;
 int transmitIndex;
unsigned int money;
unsigned int happy;
unsigned int hunger;
unsigned int thirst;
unsigned int receiveFlag;
unsigned int inTransmit;
int goFlag;
 
/**********************************************************************
 * ----------------------- GLOBAL FUNCTIONS ---------------------------
 **********************************************************************/

 
void transmitData(){
    
    char toSend;
    int i;
        
        // If current char is '-', transmission is finished.
        if(transmitBuffer[transmitIndex]=='-'){
            transmitIndex=0;
            inTransmit=0;
            for(i=0;i<20;i++) transmitBuffer[i]='-';
            TXSTA1bits.TXEN=0;
            
        } // Else continue transmitting and increment the index.
        else{
            inTransmit=1;
            toSend=transmitBuffer[transmitIndex];
            transmitBuffer[transmitIndex]='-';
            transmitIndex++;
            TXREG1=toSend;
            TXSTA1bits.TXEN=1;
        }
   }

/* Invoked when receive interrupt occurs; meaning that data is received */
void dataReceived(){
    int i;
    rcvd_value=RCREG1;
        if(rcvd_value=='{'){ //If the received char is '{', this means new command transmission started.
            receiveIndex=0;
            for( i=0;i<20;i++){
                receiveBuffer[i]='-';
            }
            receiveBuffer[receiveIndex]=rcvd_value;
        }
        if(rcvd_value=='}'){ // Command transmission is ended. Parse the command and clear goFlag or SetEvent
            receiveFlag=1;
            receiveBuffer[++receiveIndex]=rcvd_value;
            if(receiveBuffer[2]=='E' && receiveBuffer[3]=='N' && receiveBuffer[4]=='D'){
                goFlag=0;
            }
            else SetEvent(TASK0_ID,ACTIVE_EVENT);
        } 
        // If it is neither of the two cases, it means that it is in the middle of the command.
        // Just append and increment the index.
        receiveBuffer[++receiveIndex]=rcvd_value;
      }

/* End of File : common.c */
