/*
 * GROUP 65 *
 Berke Sina Ahlatc? 2468502
 At?nç Utku Alparslan 2380061
 Murat Akkoyun 2380020
 Mustafa Mert Köse 2381705
 */
#include <xc.h>
#include "Includes.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#define _XTAL_FREQ 40000000
#pragma config OSC = HSPLL


// Predefined set of characters.
volatile char PREDEFINED[] = " abcdefghijklmnopqrstuvwxyz0123456789";

char sentence[16];  // to store sentence on LCD

int custom_sentence[16];  // to store sentence on LCD

void seven_segment();

void SendBusContents(uint8_t data);

int old_pos = 0;  // cursor position

int currentIndex=0;  // for character cycles
int customIndex=-1;

unsigned int result;

int row=1; // CDM row/col positions. row increments as powers of 2.
int col=0;

int flag=0; // checks if RD6 meant to be on

uint8_t customChars[8][8]; // array for custom charmap

int charNum=0;  // number of custom characters.

void TEM();

void CDM();

void TSM();


void __interrupt(high_priority) highPriorityISR(void) {
	if (PIR1bits.ADIF) { // ADC interrupt
        if (ADCON0bits.GODONE == 0) { // the ADC process finished
            
            PIR1bits.ADIF=0;

            result = (ADRESH << 8) + ADRESL; // Get the result;

            result /= 64;
            if(result>old_pos){
                currentIndex=0; // reset indexes
                customIndex=-1;
                while(old_pos!=result){
                    PORTBbits.RB2 = 0;
                    SendBusContents(0x14); // right shift the cursor
                    //__delay_us(30);
                    old_pos++;
                }
            }
            if(result<old_pos){
                currentIndex=0; // reset indexes
                customIndex=-1;
                while(old_pos!=result){
                    PORTBbits.RB2 = 0;
                    SendBusContents(0x10); // left shift the cursor
                    //__delay_us(30);
                    old_pos--;
                }
            }
            
        }    
    }
}

void __interrupt(low_priority) lowPriorityISR(void) {
	if (PIR1bits.TMR1IF)  seven_segment();
}

void seven_segment(){  // __delay_ms(1) is necessary for seven segment display to work properly. + We did the same in previous homework.
    PIR1bits.TMR1IF = 0;
	LATH = 0x01;
	LATJ = SSEGMENT_NUMBERS_GLYPHS[charNum];
	__delay_ms(1);
    LATJ=0x00;
	LATH = 0x04;
	LATJ = SSEGMENT_NUMBERS_GLYPHS[col];
	__delay_ms(1);
    LATJ=0x00;
	LATH = 0x08;
	LATJ = SSEGMENT_NUMBERS_GLYPHS[(int) log2(row)];
	__delay_ms(1);
    LATJ=0x00;
}

void tmr1_init(){
    TMR1H = (30000 >> 8) & 0xff;
	TMR1L = 30000 & 0xff;
    T1CON = 0xC9;
}

void adc_init(){
    TRISH = 0x10; // AN12 input RH4
  // Set LCD Outputs
    TRISB = 0x00; // LCD Control RB2/RB5
    TRISD = 0x00; // LCD Data  RD[4-7]
    // Configure ADC
    ADCON0 = 0x31; // Channel 12; Turn on AD Converter
    ADCON1 = 0x00; // All analog pins
    ADCON2 = 0xAA; // Right Align | 12 Tad | Fosc/32
    ADRESH = 0x00;
    ADRESL = 0x00;
}

void Pulse(void){
    PORTBbits.RB5 = 1;
    __delay_us(30);
    PORTBbits.RB5 = 0;
    __delay_us(30);
}


void InitLCD(void) {
    __delay_ms(20);
    PORTD = 0x30;
    Pulse();
    
    __delay_ms(6);
    PORTD = 0x30;
    Pulse();
    
    __delay_us(300);
    PORTD = 0x30;
    Pulse();
    
    __delay_ms(2);
    PORTD = 0x20;
    Pulse();
    PORTBbits.RB2 = 0;
    SendBusContents(0x2C);
    SendBusContents(0x0E);
    SendBusContents(0x01);
}

void SendBusContents(uint8_t data){
  PORTD = PORTD & 0x0F;           // Clear bus
  PORTD = PORTD | (data&0xF0);     // Put high 4 bits
  Pulse();                        
  PORTD = PORTD & 0x0F;           // Clear bus
  PORTD = PORTD | ((data<<4)&0xF0);// Put low 4 bits
  Pulse();
}

void LAT_to_Arr(){
    
    uint8_t custom_char[8];
    uint8_t a_result;
    uint8_t b_result;
    uint8_t c_result;
    uint8_t d_result;
    
    
    // This function transforms LAT values to charmap by shifting values accordingly.

    a_result = LATA & 0b00000001;
    a_result = a_result << 4;
    b_result = LATB & 0b00000001;
    b_result = b_result << 3;
    c_result = LATC & 0b00000001;
    c_result = c_result << 2;
    d_result = LATD & 0b00000001;
    d_result = d_result << 1;
    customChars[charNum-1][0] = a_result + b_result + c_result + d_result;

    a_result = LATA & 0b00000010;
    a_result = a_result << 3;
    b_result = LATB & 0b00000010;
    b_result = b_result << 2;
    c_result = LATC & 0b00000010;
    c_result = c_result << 1;
    d_result = LATD & 0b00000010;
    customChars[charNum-1][1] = a_result + b_result + c_result + d_result;

    a_result = LATA & 0b00000100;
    a_result = a_result << 2;
    b_result = LATB & 0b00000100;
    b_result = b_result << 1;
    c_result = LATC & 0b00000100;
    d_result = LATD & 0b00000100;
    d_result = d_result >> 1;
    customChars[charNum-1][2] = a_result + b_result + c_result + d_result;


    a_result = LATA & 0b00001000;
    a_result = a_result << 1;
    b_result = LATB & 0b00001000;
    c_result = LATC & 0b00001000;
    c_result = c_result >> 1;
    d_result = LATD & 0b00001000;
    d_result = d_result >> 2;
    customChars[charNum-1][3] = a_result + b_result + c_result + d_result;


    a_result = LATA & 0b00010000;
    b_result = LATB & 0b00010000;
    b_result = b_result >> 1;
    c_result = LATC & 0b00010000;
    c_result = c_result >> 2;
    d_result = LATD & 0b00010000;
    d_result = d_result >> 3;
    customChars[charNum-1][4] = a_result + b_result + c_result + d_result;

    a_result = LATA & 0b00100000;
    a_result = a_result >> 1;
    b_result = LATB & 0b00100000;
    b_result = b_result >> 2;
    c_result = LATC & 0b00100000;
    c_result = c_result >> 3;
    d_result = LATD & 0b00100000;
    d_result = d_result >> 4;
    customChars[charNum-1][5] = a_result + b_result + c_result + d_result;


    a_result = LATA & 0b01000000;
    a_result = a_result >> 2;
    b_result = LATB & 0b01000000;
    b_result = b_result >> 3;
    c_result = LATC & 0b01000000;
    c_result = c_result >> 4;
    d_result = LATD & 0b01000000;
    d_result = d_result >> 5;
    customChars[charNum-1][6] = a_result + b_result + c_result + d_result;

    a_result = LATA & 0b10000000;
    a_result = a_result >> 3;
    b_result = LATB & 0b10000000;
    b_result = b_result >> 4;
    c_result = LATC & 0b10000000;
    c_result = c_result >> 5;
    d_result = LATD & 0b10000000;
    d_result = d_result >> 6;
    customChars[charNum-1][7] = a_result + b_result + c_result + d_result;     
    
    /*
    for(int i=0;i<7;i++){
        customChars[charNum-1][i]=customChars[charNum-1][i+1];
    }
     * */
    
    PORTBbits.RB2 = 0;
    SendBusContents(0x40+(charNum-1)*8);
    for (int i=0; i<8; i++)
    {
        customChars[charNum-1][i]<<1;
        PORTBbits.RB2 = 1;
        SendBusContents(customChars[charNum-1][i]);
    }
    PORTBbits.RB2 = 0;
    SendBusContents(0x80+result);
    PORTBbits.RB2 = 1;
    SendBusContents(charNum-1);
    custom_sentence[result]=charNum-1;
    PORTBbits.RB2 = 0;
    SendBusContents(0x10);
    return;
}



void init(){
    tmr1_init();
    adc_init();
    InitLCD();
    
	TRISJ = 0x00; // init 7_segment_display
    
    TMR1H = (65530 >> 8) & 0xff; // TMR1 preload
    TMR1L = 65530 & 0xff;
    
    TRISE=0xff; // Buttons : PORTE
    LATE=0x00; 
    
    TRISA=0x00; // LEDS : PORT[A-D]
    LATA=0x00;
    TRISB=0x00;
    LATB=0x00;
    TRISC=0x00;
    LATC=0x00;
    TRISD=0x00;
    LATD=0x00;

}


void TEM(){
    int charSelect = 0; // 0 : PREDEFINED , 1 : CUSTOM
    int pressed=0;
    while(1){
        LATD=0x00;
        GODONE = 1;  // for potenitometer
        
        if(PORTEbits.RE4 && charNum!=8){
            while(PORTEbits.RE4);
            CDM(); // goto CDM state
        }
        
        if(PORTEbits.RE5){
            while(PORTEbits.RE5);
            TSM(); // goto TSM state
        }
        
        if(PORTEbits.RE2){
            // forward scroll
            while(PORTEbits.RE2);
            pressed=1;
            charSelect = 0;
            currentIndex++;
            if(currentIndex==37){
                currentIndex=0;
            }
        }
        if(PORTEbits.RE1){
            // backward scroll
            while(PORTEbits.RE1);
            pressed=1;
            charSelect = 0;
            currentIndex--;
            if(currentIndex==-1){
                currentIndex=36;
            }
        }
        if(PORTEbits.RE0){
            // forward scroll custom
            while(PORTEbits.RE0);
            pressed=1;
            charSelect = 1;
            customIndex++;
            if(customIndex > charNum-1) customIndex=-1;
        }
        if(PORTEbits.RE3){
            // backward scroll custom
            while(PORTEbits.RE3);
            pressed=1;
            charSelect = 1;
            customIndex--;
            if(customIndex < -1) customIndex=charNum-1;

        }
        
        if(!charSelect && pressed){
            // write the char and shift curser left by one block to stay in the same area
            pressed=0;
            PORTBbits.RB2 = 1;
            SendBusContents(PREDEFINED[currentIndex]);
            sentence[result]=PREDEFINED[currentIndex];
            PORTBbits.RB2 = 0;
            SendBusContents(0x10);
            __delay_us(30);
        }
        if(charSelect && pressed){
            // write the custom char and shift curser left by one block to stay in the same area
            pressed=0;
            PORTBbits.RB2 = 1;
            if(customIndex==-1)  SendBusContents(' ');
            else  SendBusContents(customIndex);
            custom_sentence[result]=customIndex;
            PORTBbits.RB2 = 0;
            SendBusContents(0x10);
            __delay_us(30);
        }
        
        
        
    }
}



void CDM(){
    
    TRISD=0x00;
    LATA=0x00;
    LATB=0x00;
    LATC=0x00;
    LATD=0x00;
    
    while(1){
        GODONE = 1;  // for potenitometer
        if(flag%2==0){  // RD6 gets on while moving cursor therefore we used a flag.
            LATD&=0xBF;
        }
        else{
            LATD|=0x40;
        }
        if(PORTEbits.RE5){
            while(PORTEbits.RE5);
            row=1;
            col=0;
            charNum++;
            LAT_to_Arr();
            LATA=0;
            LATB=0;
            LATC=0;
            LATD=0;
            flag=0;
            return;
        }
        if(PORTEbits.RE4){
            while(PORTEbits.RE4);
            // toggle
            // Since row is power of two it shows the bit that needs to be toggled.
            if(col==0){
                if(LATA & row){
                    LATA &= ~row; 
                }
                else{
                    LATA|=row;
                }
            }
            if(col==1){
                if(LATB & row){
                    LATB &= ~row; 
                }
                else{
                    LATB|=row;
                }
            }
            if(col==2){
                if(LATC & row){
                    LATC &= ~row; 
                }
                else{
                    LATC|=row;
                }
            }
            if(col==3){
                
                if(row==64) flag++;
                
                if(LATD & row){
                    LATD &= ~row; 
                }
                else{
                    LATD|=row;
                }
            }
            
        }
        if(PORTEbits.RE2){
            while(PORTEbits.RE2);
            // up
            if(row>1) row/=2;
            
        }
        if(PORTEbits.RE1){
            while(PORTEbits.RE1);
            // down
            if(row<128) row*=2;
        }
        if(PORTEbits.RE3){
            while(PORTEbits.RE3);
            // left
            if(col>0) col--;
        }    
        if(PORTEbits.RE0){
            while(PORTEbits.RE0);
            // right
            if(col<3) col++;
        }
    }
}

void TSM(){
    
    LATA=0;
    LATB=0;
    LATC=0;
    LATD=0;
    
    PIE1bits.ADIE = 1;
    PORTBbits.RB2 = 0;
    SendBusContents(0x0C); // Display on, cursor off, blink off.
    
    __delay_ms(10);
    
    PORTBbits.RB2=0;
    SendBusContents(0x01); // clear display
    
    __delay_ms(10);
    
    PORTBbits.RB2=0;
    SendBusContents(0x80 + 3); // set cursor to first line
        
    PORTBbits.RB2=1;
    SendBusContents('f');
    SendBusContents('i');
    SendBusContents('n');
    SendBusContents('i');
    SendBusContents('s');
    SendBusContents('h');
    SendBusContents('e');
    SendBusContents('d');
        

    while(1){
        PORTBbits.RB2=0;
        SendBusContents(0xC0);  // set cursor to second line
        
        // display sentence
        PORTBbits.RB2=1;
        for(int i=0; i<16; i++){
            if(sentence[i]!=' '){
                SendBusContents(sentence[i]);
                continue;
            }
            if(custom_sentence[i]!=-1){
                SendBusContents(custom_sentence[i]);
                continue;
            }
            SendBusContents(' ');
        }
        
        // shift sentence
        char tmp=sentence[0];
        int tmp2=custom_sentence[0];
        for(int i=0;i<15;i++){
            sentence[i]=sentence[i+1];
            custom_sentence[i]=custom_sentence[i+1];
        }
        sentence[15]=tmp;
        custom_sentence[15]=tmp2;
        
        __delay_ms(250);
        
    }
}


/*
 * Program first initializes TRIS values according to the hw PDF and clears LATs.
 * Interrupts, A/D converter and timer 1 gets initialized.
 * First goes to TEM state and polls for PORTE inputs. Switches to desired states or cycles on characters. Cursor can be moved freely.
 * In CDM we update row/col values by PORTE inputs and toggle leds accordingly.
 * Lastly TSM user defined text gets scrolled every 250ms.
 */
void main(void) {
    init();
    
    for(int i=0;i<16;i++){
        sentence[i]=' ';
        custom_sentence[i]=-1;
    }
    
    PIR1bits.ADIF = 0; // ADC Interrupt
    PIE1bits.ADIE = 1;
    
    INTCONbits.PEIE = 1;
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    IPR1bits.ADIP = 1; // A/D -> high priority
	
    IPR1bits.TMR1IP = 0;            // Timer 1 -> Low priority interrupt group
	RCONbits.IPEN = 1;              // Enable interrupt system priority feature
	INTCONbits.GIEH = 1;            // Enable high priority interrupts
	INTCONbits.GIEL = 1;            // Enable low priority interrupts
	PIE1bits.TMR1IE = 1;            // Enable Timer1 interrupt 
    

    while(1){

        TEM(); // goto TEM state

    }
    return;
}