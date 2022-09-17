/*
 * GROUP 65 *
 Berke Sina Ahlatc? 2468502
 At?nç Utku Alparslan 2380061
 Murat Akkoyun 2380020
 Mustafa Mert Köse 2381705
 */
#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 40000000
#pragma config OSC = HSPLL
void tmr_isr();
void flick();
void check();
void __interrupt(high_priority) highPriorityISR(void) {
	if (INTCONbits.TMR0IF) tmr_isr();
}

void __interrupt(low_priority) lowPriorityISR(void) {
	if (PIR1bits.TMR1IF) flick();
}

unsigned int flag = 0, level = 1, pin, tmr1_val, cnt = 0, ready = 0, fail = 0, success = 0, lvl3_pass = 0, lvl2_pass = 0, checkFlag=1, pres=0;
int health = 9;
unsigned int pres1=0, pres0=0, pres2=0, pres3=0, pres4=0;
unsigned char seg[] = {
	 0x3F, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c , 0x07, 0x7f, 0x6f
};

unsigned int rotr(unsigned int x, unsigned n) {
  return ((x >> n) | (x << (16-n))) & 0xffff;
}

void init_LATs() {
	TRISA = 0x00;
	TRISB = 0x00;
	TRISC = 0x00;
	TRISD = 0x00;
	TRISE = 0x00;
	TRISF = 0x00;
	TRISG = 0x0F;
	LATA = 0x00;
	LATB = 0x00;
	LATC = 0x00;
	LATD = 0x00;
	LATE = 0x00;
	LATF = 0x00;
	LATG=0x00;
}

void flick() {
	PIR1bits.TMR1IF = 0;
	if (fail) {
		LATH = 0x01;
		LATJ = 0x38;
		__delay_ms(1);
		LATH = 0x02;
		LATJ = seg[0];
		__delay_ms(1);
		LATH = 0x04;
		LATJ = 0x6D;
		__delay_ms(1);
		LATH = 0x08;
		LATJ = 0x79;
		__delay_ms(1);
	}
	if (success) {
		LATH = 0x01;
		LATJ = 0x79;
		__delay_ms(1);
		LATH = 0x02;
		LATJ = 0x54;
		__delay_ms(1);
		LATH = 0x04;
		LATJ = 0x5E;
		__delay_ms(1);
	}
	if (!success && !fail) {
		LATH = 0x08;
		LATJ = seg[level];
		__delay_ms(1);
		LATH = 0x01;
		LATJ = seg[health];
		__delay_ms(1);
	}

}

void init_irq() {
	INTCONbits.TMR0IE = 1;
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
}


void init_seg() {
	TRISH = 0x00;
	TRISJ = 0x00;
}

#define TIMER0_PRELOAD 60
// Initialize Timer 0 with 1:256 prescale and set preload value
// to 60 in order to get high priority interrupts every 5ms.

void tmr_preload() {
	TMR0L = TIMER0_PRELOAD & 0xff;
}

void missRG() {
	health -= 1;
	if (health == 0) {
		fail = 1;
		return;
	}
}



void shiftLights() {
	if (LATF != 0x00) {
		missRG();
		if (fail == 1) {
			return;
		}
	}
	LATF = LATE;
	LATE = LATD;
	LATD = LATC;
	LATC = LATB;
	LATB = LATA;
	LATA = 0x00;
	if (level == 3 && !LATF && !LATE && !LATD && !LATC && !LATB && !LATA) {
		success = 1;
	}
	if (level == 2 && !LATF && !LATE && !LATD && !LATC && !LATB && !LATA) {
		lvl3_pass = 1;
	}
	if (level == 1 && !LATF && !LATE && !LATD && !LATC && !LATB && !LATA) {
		lvl2_pass = 1;
	}
}
void tmr_isr() {
	INTCONbits.TMR0IF = 0;
	cnt++;
    check();
	tmr_preload();
	if (level == 1) {
		if (cnt == 100) {
			cnt = 0;
			ready = 1;
			shiftLights();
		}
	}
	if (level == 2) {
		if (cnt == 80) {
			cnt = 0;
			ready = 1;
			shiftLights();
		}
	}
	if (level == 3) {
		if (cnt == 60) {
			cnt = 0;
			ready = 1;
			shiftLights();
		}
	}
}


void tmr0_init() {
	T0CON = 0x57; // internal clock with 1:256 prescaler 
	tmr_preload();
}

void tmr1_init() {
	T1CON = 0xC9;
}

void check() {
    if(fail || success){
        return;
    }
	if (PORTGbits.RG0) {
        pres0=1;
	}
    if(!PORTGbits.RG0 && pres0){
        pres0=0;
        if(LATF!=0x01 && checkFlag){
            checkFlag=0; 
            missRG();
        }
		if(LATF==0x01 && checkFlag){
            LATF=0x00;
            checkFlag=0;
        }
        return;
    }
    
    if (PORTGbits.RG1) {
        pres1=1;
	}
    if(!PORTGbits.RG1 && pres1){
        pres1=0;
        if(LATF!=0x02 && checkFlag){
            checkFlag=0; 
            missRG();
        }
		if(LATF==0x02 && checkFlag){
            LATF=0x00;
            checkFlag=0;
        }
        return;
    }
    
    if (PORTGbits.RG2) {
        pres2=1;
	}
    if(!PORTGbits.RG2 && pres2){
        pres2=0;
        if(LATF!=0x04 && checkFlag){
            checkFlag=0; 
            missRG();
        }
		if(LATF==0x04 && checkFlag){
            LATF=0x00;
            checkFlag=0;
        }
        return;
    }
    
    if (PORTGbits.RG3) {
        pres3=1;
	}
    if(!PORTGbits.RG3 && pres3){
        pres3=0;
        if(LATF!=0x08 && checkFlag){
            checkFlag=0; 
            missRG();
        }
		if(LATF==0x08 && checkFlag){
            LATF=0x00;
            checkFlag=0;
        }
        return;
    }
    
    if (PORTGbits.RG4) {
        pres4=1;
	}
    if(!PORTGbits.RG4 && pres4){
        pres4=0;
        if(LATF!=0x10 && checkFlag){
            checkFlag=0; 
            missRG();
        }
		if(LATF==0x10 && checkFlag){
            LATF=0x00;
            checkFlag=0;
        }
        return;
    }
    checkFlag=1;
}


void waitt() {
	while (!ready) {
		if (fail) {
			return;
		}

	}
}

void game_task_lvl3() {
	LATH = 0x08;
	LATJ = seg[3];
	unsigned lvl3_val = tmr1_val;
	for (int i = 0; i < 15;i++) {
		waitt();
		if (fail) {
			return;
		}
		ready = 0;
		lvl3_val = rotr(lvl3_val, 5);
		int random_val = (lvl3_val & 7) % 5;
		switch (random_val) {
		case 0:
			LATA = 0x01;
			break;
		case 1:
			LATA = 0x02;
			break;
		case 2:
			LATA = 0x04;
			break;
		case 3:
			LATA = 0x08;
			break;
		case 4:
			LATA = 0x10;
			break;
		}
	}
	while (!success) {
		if (fail) {
			return;
		}
	}
	return;
}

void game_task_lvl2() {
	LATH = 0x08;
	LATJ = seg[2];
	unsigned lvl2_val = tmr1_val;
	for (int i = 0; i < 10;i++) {
		waitt();
		if (fail) {
			return;
		}
		ready = 0;
		lvl2_val = rotr(lvl2_val, 3);
		int random_val = (lvl2_val & 7) % 5;
		switch (random_val) {
		case 0:
			LATA = 0x01;
			break;
		case 1:
			LATA = 0x02;
			break;

		case 2:
			LATA = 0x04;
			break;
		case 3:
			LATA = 0x08;
			break;
		case 4:
			LATA = 0x10;
			break;
		}
	}
	while (!lvl3_pass) {
		if (fail) {
			return;
		}
	}
	level = 3;
	game_task_lvl3();
}


void game_task_lvl1() {
	//RANDOM NOTE GENERATE
	unsigned lvl1_val = tmr1_val;
	for (int i = 0; i < 5;i++) {
		waitt();
		if (fail) {
			return;
		}
		ready = 0;
		lvl1_val = rotr(lvl1_val, 1);
		int random_val = (lvl1_val & 7) % 5;
		switch (random_val) {
		case 0:
			LATA = 0x01;
			break;
		case 1:
			LATA = 0x02;
			break;
		case 2:
			LATA = 0x04;
			break;
		case 3:
			LATA = 0x08;
			break;
		case 4:
			LATA = 0x10;
			break;
		}
	}

	while (!lvl2_pass) {
		if (fail) {
			return;
		}
	}
	level = 2;
	game_task_lvl2();
}


void main(void) {
	tmr0_init();
	tmr1_init();
	init_irq();
	while (1) {
		if (!flag) {
			TRISA = 0x00;
			TRISB = 0x00;
			TRISC = 0xFF;
			TRISD = 0x00;
			TRISE = 0x00;
			TRISF = 0x00;
			TRISG = 0x00;  // Init TRIS and LAT ports.
			LATA = 0x00;
			LATB = 0x00;
			LATC = 0x00;
			LATD = 0x00;
			LATE = 0x00;
			LATF = 0x00;
			flag = 1;
		}
		if (PORTCbits.RC0 == 1) {
			while (PORTCbits.RC0 == 1) {}
			init_LATs();
			T0CON |= 0x80;
            tmr1_val = TMR1L ;
            tmr1_val = tmr1_val | (((int)TMR1H) << 8);
			TMR1H = (65530 >> 8) & 0xff;
			TMR1L = 65530 & 0xff;
            // Initialized Timer 1 with 65530 preload value and disabled prescale 
            // to make 7-segment display look stable.
			init_seg();
			IPR1bits.TMR1IP = 0;            // Timer 1 -> Low priority interrupt group
			RCONbits.IPEN = 1;              // Enable interrupt system priority feature
			INTCONbits.GIEH = 1;            // Enable high priority interrupts
			INTCONbits.GIEL = 1;            // Enable low priority interrupts
			PIE1bits.TMR1IE = 1;            // Enable Timer1 interrupt  
			level = 1;
			flag = 0;
			ready = 1;
			fail = 0;           // Initialize essential values.
			health = 9;
            success=0;
            pres1=0; pres0=0; pres2=0; pres3=0; pres4=0;
			game_task_lvl1();
			T0CON &= 0x7F;
			LATB = 0x00;
		}
	}
}
