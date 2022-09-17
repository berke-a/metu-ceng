PROCESSOR 18F8722
    
#include <xc.inc>

; configurations
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

; global variable declarations
GLOBAL var1
GLOBAL _t1, _t2, _t3, bVal, cVal, ledBool, ledVal

; allocating memory for variables
PSECT udata_acs
    var1:
	DS     1    ; allocates 1 byte
    _t1:
        DS 1    ; allocate 1 byte
    _t2:
        DS 1    ; allocate 1 byte
    _t3:
        DS 1    ; allocate 1 byte
    bVal:
	DS 1
    cVal:
	DS 1
    ledBool:
	DS 1
    ledVal:
	DS 1

PSECT resetVec,class=CODE,reloc=2
resetVec:
    goto    main

; DO NOT DELETE OR MODIFY
; 500ms pass check for test scripts
ms500_passed:
    nop
    return

; DO NOT DELETE OR MODIFY
; 1sec pass check for test scripts
ms1000_passed:
    nop
    return

		
wait1000ms:
    movlw 0x84   
    movwf _t3     
    l3:
        movlw 0xAF      
        movwf _t2       
        l2:
            movlw 0x8F
	    movwf _t1
            l1:
                decfsz _t1, 1   
                goto l1    
                decfsz _t2, 1  
                goto l2    
                decfsz _t3, 1 
                goto l3   
                return 
				
wait500ms:
    movlw 0x42      
    movwf _t3     
    _l3:
        movlw 0xAF      
        movwf _t2       
        _l2:
            movlw 0x8F
	    movwf _t1
            _l1:
                decfsz _t1, 1   
                goto _l1    
                decfsz _t2, 1  
                goto _l2    
                decfsz _t3, 1 
                goto _l3   
                return 
		
init:
    movlw 0b00010000
    movwf TRISA
    clrf LATA
    
    movwf TRISE
    clrf LATE
    
    movlw 0b11110000
    movwf TRISB
    
    movlw 0b11111100
    movwf TRISC
    
    movlw 0b00000000
    movwf TRISD
    
    movlw 0xFF
    movwf LATB
    movwf LATC
    movwf LATD
    return
    
B_NotSoBusyLoop:
    movlw 0x16    
    movwf _t3     
    loop3:
        movlw 0xAF      
        movwf _t2       
        loop2:
            movlw 0x8F
	    movwf _t1
            loop1:
		btfsc PORTE,4
		goto C_re4_loop1
		btfsc PORTA,4
		goto B_ra4_loop1
                decfsz _t1, 1   
		goto loop1    
                decfsz _t2, 1  
                goto loop2    
                decfsz _t3, 1 
                goto loop3   
		call ms500_passed
                call leds 
		goto B_NotSoBusyLoop
		
B_ra4_loop:
    movlw 0x21  
    movwf _t3     
    B_ra4_loop3:
        movlw 0xAF      
        movwf _t2       
        B_ra4_loop2:
            movlw 0x8F
	    movwf _t1
            B_ra4_loop1:
		btfss PORTA,4
		goto controlB
                decfsz _t1, 1   
		goto B_ra4_loop1    
                decfsz _t2, 1  
                goto B_ra4_loop2    
                decfsz _t3, 1 
                goto B_ra4_loop3   
		call ms500_passed
		call leds
                goto B_ra4_loop 
		
controlB:
    btfsc bVal,3
    goto PortB1
    btfsc bVal,2
    goto PortB4
    btfsc bVal,1
    goto PortB3
    goto PortB2
    
leds:
    btfsc ledBool,0
    goto B_CloseLeds
    goto B_OpenLeds
    
B_CloseLeds:
    clrf LATB
    clrf ledBool
    return
    
B_OpenLeds:
    movf ledVal,0
    movwf LATB
    movlw 0b00000001
    movwf ledBool
    return
    

PortB1:
    movlw 0b00000001
    movwf ledVal
    movlw 0b00000001
    movwf bVal
    btfsc ledBool,0
    call B_OpenLeds
    goto loop1
    
PortB2:
    movlw 0b00000011
    movwf ledVal
    movlw 0b00000010
    movwf bVal
    btfsc ledBool,0
    call B_OpenLeds
    goto loop1
    
PortB3:
    movlw 0b00000111
    movwf ledVal
    movlw 0b00000100
    movwf bVal
    btfsc ledBool,0
    call B_OpenLeds
    goto loop1
    
PortB4:
    movlw 0b00001111
    movwf ledVal
    movlw 0b00001000
    movwf bVal
    btfsc ledBool,0
    call B_OpenLeds
    goto loop1
    
		
Cloop_re4:
    movlw 0x16    
    movwf _t3     
    C_re4_loop3:
        movlw 0xAF      
        movwf _t2       
        C_re4_loop2:
            movlw 0x8F
	    movwf _t1
            C_re4_loop1:
		btfss PORTE,4
		goto ledValIni
                decfsz _t1, 1   
		goto C_re4_loop1    
                decfsz _t2, 1  
                goto C_re4_loop2    
                decfsz _t3, 1 
                goto C_re4_loop3   
		call ms500_passed
		call leds
		goto Cloop_re4
    
ledValIni:
    call B_OpenLeds
    movlw 0x1
    movwf ledVal
    movwf ledBool
    goto C_loop1

C_NotSoBusyLoop:
    movlw 0x16    
    movwf _t3     
    C_loop3:
        movlw 0xAF      
        movwf _t2       
        C_loop2:
            movlw 0x8F
	    movwf _t1
            C_loop1:
		btfsc PORTE,4
		goto D_loop1
		btfsc PORTA,4
		goto C_ra4_loop1
                decfsz _t1, 1   
		goto C_loop1    
                decfsz _t2, 1  
                goto C_loop2    
                decfsz _t3, 1 
                goto C_loop3   
		call ms500_passed
                call C_leds
		goto C_NotSoBusyLoop
		
C_ra4_loop:
    movlw 0x21  
    movwf _t3     
    C_ra4_loop3:
        movlw 0xAF      
        movwf _t2       
        C_ra4_loop2:
            movlw 0x8F
	    movwf _t1
            C_ra4_loop1:
		btfss PORTA,4
		goto controlC
                decfsz _t1, 1   
		goto C_ra4_loop1    
                decfsz _t2, 1  
                goto C_ra4_loop2    
                decfsz _t3, 1 
                goto C_ra4_loop3   
		call ms500_passed
                call C_leds
		goto C_ra4_loop
    
controlC:
    btfsc cVal, 0
    goto PORTC2
    goto PORTC1
    
C_leds:
    btfsc ledBool,0
    goto C_CloseLeds
    goto C_OpenLeds
    
C_CloseLeds:
    clrf LATC
    clrf ledBool
    return
    
C_OpenLeds:
    movf ledVal,0
    movwf LATC
    movlw 0b00000001
    movwf ledBool
    return
       
    
PORTC2:
    movlw 0b00000010
    movwf ledVal
    movlw 0b00000010	; if cVal 10 it is DEFEND
    movwf cVal
    btfsc ledBool,0
    call C_OpenLeds
    goto C_loop1
    
PORTC1:
    movlw 0b00000001
    movwf ledVal
    movlw 0b00000001	; if cVal 01 it is ATTACK
    movwf cVal
    btfsc ledBool,0
    call C_OpenLeds
    goto C_loop1
    		
Dloop_re4:
    movlw 0x16    
    movwf _t3     
    D_loop3:
        movlw 0xAF      
        movwf _t2       
        D_loop2:
            movlw 0x8F
	    movwf _t1
            D_loop1:
		btfss PORTE,4
		goto D_re4_press
                decfsz _t1, 1   
		goto D_loop1    
                decfsz _t2, 1  
                goto D_loop2    
                decfsz _t3, 1 
                goto D_loop3   
		call ms500_passed
		call C_leds
		goto Dloop_re4

    
D_re4_press:
    call C_OpenLeds
    clrf LATD
    btfsc bVal, 3 
    goto level4
    btfsc bVal, 2
    goto level3
    btfsc bVal, 1
    goto level2
    btfsc bVal, 0
    goto level1

level1:
    btfsc cVal, 1
    goto Def1
    btfsc cVal, 0
    goto Att1
    
level2:
    btfsc cVal, 1
    goto Def2
    btfsc cVal, 0
    goto Att2
    
level3:
    btfsc cVal, 1
    goto Def3
    btfsc cVal, 0
    goto Att3
    
level4:
    btfsc cVal, 1
    goto Def4
    btfsc cVal, 0
    goto Att4
    
Def4:
    movlw 0b11111111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b01111111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00111111
    movwf LATD
    call wait500ms	
    call ms500_passed
    movlw 0b00011111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00001111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000011
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000001
    movwf LATD
    call wait500ms
    call ms500_passed
    clrf LATD
    goto ret
    
Att4:
    movlw 0b00001111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000011
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000001
    movwf LATD
    call wait500ms
    call ms500_passed
    clrf LATD
    goto ret
    
Def3:
    movlw 0b00111111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00011111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00001111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000011
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000001
    movwf LATD
    call wait500ms	
    call ms500_passed
    clrf LATD
    goto ret
    
Att3:
    movlw 0b00000111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000011
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000001
    movwf LATD
    call wait500ms
    call ms500_passed
    clrf LATD
    goto ret
    
Def2:
    movlw 0b00001111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000111
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000011
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000001
    movwf LATD
    call wait500ms
    call ms500_passed
    clrf LATD
    goto ret
    
Att2:
    movlw 0b00000011
    movwf LATD
    call wait500ms
    call ms500_passed
    movlw 0b00000001
    movwf LATD
    call wait500ms
    call ms500_passed
    clrf LATD
    goto ret
    
Def1:
    movlw 0b00000011
    movwf LATD
    call wait500ms	
    call ms500_passed
    movlw 0b00000001
    movwf LATD
    call wait500ms
    call ms500_passed
    clrf LATD
    goto ret
    
Att1:
    movlw 0b00000001
    movwf LATD
    call wait500ms
    call ms500_passed
    clrf LATD
    goto ret 

defaultConfig:
    movlw 0x01
    movwf bVal
    movwf cVal
    movwf ledVal
    movwf ledBool
    movwf LATB
    movwf LATC
    clrf LATD
    return
    
ret:
    call wait500ms
    call ms500_passed
    call defaultConfig
    goto loop

PSECT CODE
main:
    ; some code to initialize and wait 1000ms here, maybe
    call init		;all leds are on
    call wait1000ms
    call ms1000_passed
    call defaultConfig
    
    ; a loop here, maybe
    loop:
	movlw 0x21    
	movwf _t3     
	mainLoop3:
	    movlw 0xAF      
	    movwf _t2       
	    mainLoop2:
		movlw 0x8F
		movwf _t1
		mainLoop1:
		    btfsc PORTE,4
		    goto nextLoop1  ; go to next loop to release re4
		    decfsz _t1, 1   
		    goto mainLoop1    
		    decfsz _t2, 1  
		    goto mainLoop2    
		    decfsz _t3, 1 
		    goto mainLoop3   
		    call ms500_passed
		    goto loop
    nextLoop:
	movlw 0x21 
	movwf _t3     
	nextLoop3:
	    movlw 0xAF      
	    movwf _t2       
	    nextLoop2:
		movlw 0x8F
		movwf _t1
		nextLoop1:
		    btfss PORTE,4
		    call Btransition	 ; if re4 released goto B
		    decfsz _t1, 1   
		    goto nextLoop1    
		    decfsz _t2, 1  
		    goto nextLoop2    
		    decfsz _t3, 1 
		    goto nextLoop3   
		    call ms500_passed
		    goto nextLoop
		    
Btransition:
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    dcfsnz _t3, 1
    goto Btransition2
    goto loop1
    
Btransition2:
    call ms500_passed
    goto B_NotSoBusyLoop
    
    
end resetVec