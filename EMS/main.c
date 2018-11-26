// ********************************************************************************
// Includes
// ********************************************************************************
#include <avr/io.h>
#include <avr/interrupt.h>
// ********************************************************************************
// Interrupt Routines
// ********************************************************************************
ISR(TIM0_COMPA_vect) {
    PORTA ^= ((1<<PA7) | (1<<PA3));
 }

void init_ports(){
    // Configure PORTA as output
    DDRA = 0xFF;
    PORTA = ((1<<PA7)) ;
    DDRB = 0xF;
    PORTB = 0x0; //(1<<PB2);
}

ISR(TIM1_COMPA_vect) {
    TCCR0B ^= ((1<<CS00));
    if (TCCR0B == 0) {
        PORTA = 0x0;
    } else {
        init_ports();
    }
}

void init_cpu(){
    CLKPR = (1<<CLKPCE);
    //CLKPR = 0;
    //CLKPR = ((1<<CLKPS2) | (1<<CLKPS1) | (1<<CLKPS0)); //CPS scaler 128
    CLKPR = ((1<<CLKPS1) | (1<<CLKPS0)); //CPS scaler 8
    //CLKPR = ((1<<CLKPS0)); //CPS scaler 2
}
void init_timers(){
    //set CTC mode
    TCCR0A |= ((1<<WGM01) | (1<<COM0A1) ); //((1<<WGM01) | (1<<COM0A0));   //1<<COM0A0 -- Toggle OC0A, do not need interrupt; 1<<WGM01 -- CTC mode
    //set prescaler
    TCCR0B |= ((1<<CS00) | (1<<CS02));  //1024
    //TCCR0B |= ((1<<CS02));  //256
    //TCCR0B |= ((1<<CS00) | (1<<CS01));  //64
    //TCCR0B |= ((1<<CS01));  //8
    //TCCR0B |= ((1<<CS00));  //1
    //output register
    OCR0A |= 58;
    //OCR0B |= 255; not used
    TIMSK0 |= (1<<OCIE0A);
}
void init_timer16() {
    TCCR1A |= ((1<<COM1A0) | (1<<COM1B0)); //CTC
    TCCR1B |= ((1<<CS10) | (1<<CS12) | (1<<WGM12));  //1024
    TCNT1 = 0; // Initialize counter
    //OCR1AH |= 0;
    OCR1A |= 58;
    //OCR1BH |= 0;
    OCR1B |= 58;
    TIMSK1 |= (1 << OCIE1A);
}

// ********************************************************************************
// Main
// ********************************************************************************
int main( void ) {
    init_cpu();
    init_ports();
    init_timers();
    //init_timer16();
    // enable timer overflow interrupt for both Timer0 and Timer1
    sei();
    while(1) {
    }
}
