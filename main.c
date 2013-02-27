#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/interrupt.h>
#include "util.h"

uint16_t linearize_poti(uint16_t adc)
{
    static float p2 = 8.70075617e-06;
    static float p1 = 4.56448472e-01;

    float v = (p2*adc*adc + p1*adc);
    v = fminf(v, (float)0xffff);
    v = fmaxf(v, (float)0x0000);

    return (uint16_t)v;
}

const uint16_t dR[256] = {
         0,   1,   3,   4,   6,   8,  10,  12,  14,  16,  18,  20,  22,
        24,  27,  29,  31,  34,  36,  38,  41,  43,  46,  48,  51,  53,
        56,  58,  61,  63,  66,  69,  71,  74,  77,  79,  82,  85,  87,
        90,  93,  96,  98, 101, 104, 107, 110, 113, 115, 118, 121, 124,
       127, 130, 133, 136, 139, 142, 145, 148, 151, 154, 157, 160, 163,
       166, 169, 172, 175, 178, 181, 184, 187, 190, 194, 197, 200, 203,
       206, 209, 212, 216, 219, 222, 225, 228, 232, 235, 238, 241, 245,
       248, 251, 254, 258, 261, 264, 268, 271, 274, 277, 281, 284, 287,
       291, 294, 297, 301, 304, 308, 311, 314, 318, 321, 325, 328, 331,
       335, 338, 342, 345, 349, 352, 355, 359, 362, 366, 369, 373, 376,
       380, 383, 387, 390, 394, 397, 401, 404, 408, 411, 415, 419, 422,
       426, 429, 433, 436, 440, 444, 447, 451, 454, 458, 462, 465, 469,
       472, 476, 480, 483, 487, 491, 494, 498, 502, 505, 509, 513, 516,
       520, 524, 527, 531, 535, 538, 542, 546, 550, 553, 557, 561, 564,
       568, 572, 576, 579, 583, 587, 591, 595, 598, 602, 606, 610, 613,
       617, 621, 625, 629, 632, 636, 640, 644, 648, 651, 655, 659, 663,
       667, 671, 674, 678, 682, 686, 690, 694, 698, 702, 705, 709, 713,
       717, 721, 725, 729, 733, 737, 740, 744, 748, 752, 756, 760, 764,
       768, 772, 776, 780, 784, 788, 792, 795, 799, 803, 807, 811, 815,
       819, 823, 827, 831, 835, 839, 843, 847
};


void timer1_on()
{
    TCCR1A = 0x00;    // CTC
    TCCR1B = 0x09;    // CTC; clk/1

    TCNT1 = 0;        // set counter
    OCR1A = 0x1fff;   // compare A

    TIMSK |= (1 << OCIE1A);
}

// Curret PWM slot
uint8_t phase = 0;

// Color to be displayed
volatile uint8_t red;      // XXX volatile?? XXX
volatile uint8_t green;
volatile uint8_t blue;


#define PIN_RED    0
#define PIN_GREEN  1
#define PIN_BLUE   2
#define PIN_DEBUG1 3
#define PIN_DEBUG2 4

#define PIN_ON(var, pin) ((var) &= ~(1 << (pin)))
#define PIN_TOGGLE(var, pin) ((var) ^= (1 << (pin)))

#define PIN_DEFAULT ((1 << PIN_RED) | (1 << PIN_GREEN) | (1 << PIN_BLUE))

SIGNAL(SIG_OUTPUT_COMPARE1A)
{
    PIN_TOGGLE(PORTB, PIN_DEBUG1);

    if (phase < 16) {
        PIN_TOGGLE(PORTB, PIN_DEBUG2);
        PIN_TOGGLE(PORTB, PIN_DEBUG2);

        /* Anti volatile
        uint8_t red_   = red;
        uint8_t green_ = green;
        uint8_t blue_  = blue;
        */
    
        // Precalc first phases
        uint8_t i;
        uint8_t port_cyc[16];   
        uint8_t port_default = PORTB;

        for(i=0; i<16; i++) {
            port_cyc[i] = port_default;
            if (red   > i) PIN_ON(port_cyc[i], PIN_RED);
            if (green > i) PIN_ON(port_cyc[i], PIN_GREEN);
            if (blue  > i) PIN_ON(port_cyc[i], PIN_BLUE);
        }

        uint8_t port_cyc0 = port_cyc[0];
        uint8_t port_cyc1 = port_cyc[1];

        // phase == 0
        PORTB = port_cyc0;
    
        // phase == 1
        PORTB = port_cyc1;

        // phase == 2
        PORTB = port_cyc[2];

        // phase == 3
        __builtin_avr_delay_cycles(dR[2]-1);
        PORTB = port_cyc[3];

        // phase == 4
        __builtin_avr_delay_cycles(dR[3]-1);
        PORTB = port_cyc[4];

        // phase == 5
        __builtin_avr_delay_cycles(dR[4]-1);
        PORTB = port_cyc[5];

        // phase == 6
        __builtin_avr_delay_cycles(dR[5]-1);
        PORTB = port_cyc[6];

        // phase == 7
        __builtin_avr_delay_cycles(dR[6]-1);
        PORTB = port_cyc[7];

        // phase == 8
        __builtin_avr_delay_cycles(dR[7]-1);
        PORTB = port_cyc[8];

        // phase == 9
        __builtin_avr_delay_cycles(dR[8]-1);
        PORTB = port_cyc[9];

        // phase == 10
        __builtin_avr_delay_cycles(dR[9]-1);
        PORTB = port_cyc[10];

        // phase == 11
        __builtin_avr_delay_cycles(dR[10]-1);
        PORTB = port_cyc[11];

        // phase == 12
        __builtin_avr_delay_cycles(dR[11]-1);
        PORTB = port_cyc[12];

        // phase == 13
        __builtin_avr_delay_cycles(dR[12]-1);
        PORTB = port_cyc[13];

        // phase == 14
        __builtin_avr_delay_cycles(dR[13]-1);
        PORTB = port_cyc[14];

        // phase == 15
        __builtin_avr_delay_cycles(dR[14]-1);
        PORTB = port_cyc[15];

        phase = 15;

        TCNT1 = 4;
        OCR1A = dR[phase];

        phase++;
    } else {
        uint8_t port = PORTB;
        if (red   > phase) PIN_ON(port, PIN_RED);
        if (green > phase) PIN_ON(port, PIN_GREEN);
        if (blue  > phase) PIN_ON(port, PIN_BLUE);
        PORTB = port;

        OCR1A = dR[phase];
        phase++;
        if (phase == 255)
            phase = 0;
    }
    PIN_TOGGLE(PORTB, PIN_DEBUG1);
}

void hsv_to_rgb(float hue, float sat, float value, 
                uint8_t *red, uint8_t *green, uint8_t *blue)
{
    hue = fminf(hue, 360.);
    hue = fmaxf(hue, 0.);
    
    int hi = hue / 60.;

    float f = (hue / 60.) - hi; 
    uint8_t p = 255 * value * (1. - sat);
    uint8_t q = 255 * value * (1. - sat * f);
    uint8_t t = 255 * value * (1. - sat * (1. - f));
    uint8_t v = 255 * value;

    switch (hi) {
        case 0:
        case 6:
            *red   = v;
            *green = t;
            *blue  = p;
            return;
        case 1:
            *red   = q;
            *green = v;
            *blue  = p;
            return;
        case 2:
            *red   = p;
            *green = v;
            *blue  = t;
            return;
        case 3:
            *red   = p;
            *green = q;
            *blue  = v;
            return;
        case 4:
            *red   = t;
            *green = p;
            *blue  = v;
            return;
        case 5:
            *red   = v;
            *green = p;
            *blue  = q;
            return;
        default:
            *red   = 255;
            *green = 0;
            *blue  = 0;
            return;
    }
}

/**
 * Main
 */
int main()
{
    // Digital I/O
    PORTB = PIN_DEFAULT;
	DDRB = 0xff;
    
    // Setup ADC
    /* ADMUX Register:
     *   REFS1 REFS0 ADLAR MUX4-MUX0
     * 
     *  REFS:
     *  0 0     Aref Pin
     *  0 1     AVcc
         *  1 0     Reserved
     *  1 1     Internal 2.56V Reference
     * 
     * MUX = 0-7: Single ended Input on ADC0-ADC7
     */
    ADMUX = 0x20;
  
    /* ADCSRA Register:
     *   ADEN ADSC ADATE ADIF ADIE ADPS2-ADPS0
     */
    ADCSRA = (1<<ADEN) | 7; //ADC aktivieren, Takt/128

    // Initialize color channels
    red   = 0;
    green = 0;
    blue  = 0;

    // Activate timer
    timer1_on();

	sei();

    wait(500);
    ADCSRA |= (1<<ADIF);//Interrupt Flag zurücksetzen
    ADCSRA |= (1<<ADSC);//Einen Konvertierungsvorgang starten
    wait(500);

    float hue = 0.0;
    float sat = 1.0;
    float val = 1.0;

	while(1){
        /*
        if ( (ADCSRA & (1<<ADIF)) ) {
            val = 1.0*linearize_poti(ADC) / 0xffff;

            ADCSRA |= (1<<ADIF);//Interrupt Flag zurücksetzen
            ADCSRA |= (1<<ADSC);//Einen Konvertierungsvorgang starten
        }
        */

		wait(50);
        hue += 1;

        if (hue > 360.) {
            hue = 0.0;
        }

        hsv_to_rgb(hue, sat, val, &red, &green, &blue);
	}
}
