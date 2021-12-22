/**
 *******************************************************************
 * "PWM"
 *
 * Pulse-Width Modulation (PWM) is a scheme that provides power to a load by switching
 * quickly between fully on and fully off states. The PWM signal resembles a square wave
 * where the high portion of the signal is considered the on state and the low portion of
 * the signal is considered the off state. The high portion, also known as the pulse width,
 * can vary in time and is defined in steps. A longer, high on time will illuminate the LED
 * brighter. The frequency or period of the PWM does not change. A larger number of
 * steps applied, which lengthens the pulse width, also supplies more power to the load.
 * Lowering the number of steps applied, which shortens the pulse width, supplies less
 * power. The PWM period is defined as the duration of one complete cycle or the total
 * amount of on and off time combined.
 *
 * Rotating the POT will adjust the brightness of a single LED, DS4
 *
 * *******************************************************************
 * See Low Pin Count Demo Board User's Guide for Lesson Information*
 * ******************************************************************
 */


#include <stdio.h>
#include <stdlib.h>
#include <htc.h>  

#define _XTAL_FREQ 500000                          

//config bits that are part-specific for the PIC16F1829
#pragma config FOSC = INTOSC
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF

    /* -------------------LATC-----------------
     * Bit#:  -7---6---5---4---3---2---1---0---
     * LED:   ---------------|DS4|DS3|DS2|DS1|-
     *-----------------------------------------
     */

void main(void) {
    OSCCON = 0b00111000;                            //500KHz clock speed
    TRISC = 0;                                      //all LED pins are outputs
    LATC = 0;

                                                    //setup ADC
    TRISAbits.TRISA4 = 1;                           //Potentiamtor is connected to RA4...set as input
    ANSELAbits.ANSA4 = 1;                           //analog
    ADCON0 = 0b00001101;                            //select RA4 as source of ADC and enable the module (AN3)
    ADCON1 = 0b00010000;                            //left justified - FOSC/8 speed - Vref is Vdd

                                                    //setup the PWM
    CCP2CON = 0b00001100;                           //PWM mode single output
    PR2 = 255;                                      //Frequency at 486Hz. Anything over ~60Hz will get rid of any flicker
                                                    //PWM Period = [PR2 + 1]*4*Tosc*T2CKPS = [255 + 1] * 4 * (1 / 500KHz) * 1
    CCPTMRSbits.C2TSEL = 0b00;                      //select timer2 as PWM source
    T2CONbits.T2CKPS = 0b00;                        //1:1 prescaler
    T2CONbits.TMR2ON = 1;                           //start the PWM

    while (1) {
        __delay_us(5);                              //wait for ADC charging cap to settle
        GO = 1;                                     //start the ADC conversion
        while (GO) continue;                        //wait for conversion to be finished
        CCPR2L = ADRESH;                            //put the top 8 MSbs into CCPR2L
        CCP2CONbits.DC2B = (ADRESL>>6);             //put the 2 LSbs into DC2B register to complete the 10bit resolution
    }
}
