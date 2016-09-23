

#include <avr/io.h>
#include <util/delay.h>

#include "notes.h"

/// Default volume, value assigned to OCR1B
#define DEFAULT_VOLUME 100


#define y1 PA1
#define x2 PA2
#define y2 PA3
#define x1 PA4


void setADC()
{
	ADMUX |= (1<<REFS0);
	ADCSRA = (1<<ADEN)|(7<<ADPS0);
}

uint16_t readADC(uint8_t ch)
{
	
    // setting channel (3 for x , 4 for y)
	ch &= 0b00000111;
	ADMUX &= 0b11100000;
	ADMUX |= ch;
	ADCSRA |= (1<<ADSC);
	while(!(ADCSRA & (1<<ADIF))); // wait for interrupt flag
	ADCSRA |= (1<<ADIF); // invert interrupt flag
	return(ADC);
}
uint16_t getX(){
	DDRA=((1<<x1)|(1<<x2)); // setting x1 and x2 output
	PORTA=((0<<x2)|(1<<x1)); // setting default 1 to XH
	_delay_ms(10); // delay for touch

	return readADC(3); // reading x from adc
}
uint16_t getY(){
	uint16_t y;
	
	DDRA=((1<<y1)|(1<<y2));
	PORTA=((1<<y1)|(0<<y2));
	_delay_ms(10);
	return readADC(4);
	
}




void InitMusic()
{
	// Configure OC1B pin as output

	DDRD |= 1 << (DDD4); //OC1B as output

	// timer1 configuration (for PWM)
	TCCR1A |= 1 << (COM1B1);  // Clear OC1A/OC1B on compare match

	
	TCCR1B |= 1<< (WGM13) 	//mode 8, PWM, Phase and Frequency Correct (TOP value is ICR1)
	| 1 << (CS11); 		//prescaler(8)
}


/**
Plays music.
*/

void PlayMusic(  int* pMusicNotes )
{
	int duration;
	int note;
	int i;
	uint16_t delay = 20 * 1000;

	while( *pMusicNotes )
	{
		note = *pMusicNotes;
		pMusicNotes++;

		duration = *pMusicNotes;
		pMusicNotes++;

		if( p == note )
		{
			//pause, do not generate any sound
			OCR1B = 0;
		}
		else
		{
			//not a pause, generate tone
			OCR1B = DEFAULT_VOLUME;

			//set frequency
			ICR1H = (note >> 8);
			ICR1L = note;
		}

		//wait duration
		for(i=0;i<32-duration;i++)
		{
			_delay_loop_2( delay );
		}

	}
	

	//turn off any sound
	OCR1B = 0;
}


const int map[6][7] = {
	{g2,f2,e2,d2,c2,b2,a2},
	{g3,f3,e3,d3,c3,b3,a3},
	{g4,f4,e4,d4,c4,b4,a4},
	{g5,f5,e5,d5,c5,b5,a5},
	{g6,f6,e6,d6,c6,b6,a6},
	{g7,f7,e7,d7,c7,b7,a7}
};


main()
{
	InitMusic();
	setADC();
	while(1)
	{

		uint16_t x = getY();
		uint16_t y = getX();
		
		if(x == 0 || y == 0 )
		continue;
		
		int indx = (x/256.0) * 8;
		int indy = (y/256.0) * 6;
		
		
		if(indy == 6 || indx == 8)
		continue;
		
		if(indx < 7){
			int temp[3] = { map[indy][indx] , 8 , MUSIC_END};
			PlayMusic(temp );
		}
		
	}
	
	

}
