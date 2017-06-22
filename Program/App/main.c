#include "main.h"

extern lcdptr;

const char *dirstr[] = {"","RIGHT","LEFT","FRONT","BACK"};

int main(void)
{
	volatile int16u xAxis, yAxis;
	volatile int16u xAxisRef, yAxisRef;
	volatile int8u mode = CTRL;
	int8u RFData = 0x00;
	
	int8u direct = 0;

	init();
	
	xAxisRef = adcget(0);
	yAxisRef = adcget(1);
	
	while (TRUE) {
		
		if (Flag.Meas) {
			
			xAxis = adcget(0);
			yAxis = adcget(1);

			if (xAxis <= xAxisRef - THRESHOLD) direct = 3;
			else if (xAxis >= xAxisRef + THRESHOLD) direct = 4; 
			else if (yAxis <= (yAxisRef - THRESHOLD)) direct = 2;
			else if (yAxis >= (yAxisRef + THRESHOLD)) direct = 1;
			else direct = 0;
			
			if (direct) {
				lcdclr();
				lcdws ("Movement Occured");
				lcdr2();
				lcdws("Direction:");
				lcdws(dirstr[direct]);
				beep(1,ALRT_DLY);
				dlyms(DISP_DLY);
				
				switch (mode) {
					
					case VOICE: lcdclrr(1);
								lcdws(" Voice Control");
								APRVoice(direct-1);
								break;
					case CTRL:	lcdclrr(1);
								lcdws(" Device Control");
								switch(direct) {
									case 1: RFData |= 0x01; break;
									case 2: RFData &= 0xfe; break;
									case 3: RFData |= 0x02; break;
									case 4: RFData &= 0xfd; break;
								}
								txdata(RFData);
								dlyms(DISP_DLY);
								break;
				}
				disptitl();
				
				if (mode == VOICE) 
					lcdws("VOICE CTRL ");
				else 
					lcdws("DEVICE CTRL");
				
			} 
			Flag.Meas = FALSE; 
		}
		
		if (Flag.Mchg) {
			Flag.Mchg = FALSE;
			lcdptr = 0xc5;
			if (mode == CTRL) {
				lcdws("VOICE CTRL ");
				mode = VOICE;
			} else {
				mode = CTRL;
				lcdws("DEVICE CTRL");
			
			}
			
			beep(1,IND_DLY);
		}
			
		sleep();
			
	}
	return 0;
}
static void init(void)
{
	buzinit();
	beep(2,100);
	GPIOinit();
	APRinit();
	adcinit();
	ledinit();
	lcdinit();
	tmr1init();
	disptitl();
	EXT0init();
	Flag.Mchg = TRUE;
	sei();

}
static void GPIOinit(void)
{

	RFDDR  |= 0xF8;
	RFPORT |= 0x08;

}

static void disptitl(void)
{
	lcdclr();
	lcdws("HEAD MOVMENT BAS");
	lcdclrr(1);
	lcdws("VOICE EN WRL DEV");
	dlyms(DISP_DLY);
	
	lcdclr();
	lcdws("VOICE EN WRL DEV");
	lcdclrr(1);
	lcdws("Mode:");
	
}
static void EXT0init(void)
{
	INTDDR 	&= ~_BV(INT0_PIN);
	INTPORT |= _BV(INT0_PIN);


	GICR |= _BV(INT0);			//ENABLE EXTERNAL INTERRUPT
	MCUCR |= _BV(ISC01);		//FALLING EDGE INTERRUPT

}
static void tmr1init(void)
{
	TCNT1H   = 0xD3;
	TCNT1L   = 0x00;
	TIMSK   |= _BV(TOIE1);			//ENABLE OVERFLOW INTERRUPT
	TCCR1A   = 0x00;					
	TCCR1B  |= _BV(CS10) | _BV(CS11); /* PRESCALAR BY 16 */
}

/* overflows at every 100msec */
ISR(TIMER1_OVF_vect) 
{ 
	static int8u i,j;

	TCNT1H = 0xD3;
	TCNT1L = 0x00;
	
	if (++i >= 50) i = 0;
	switch(i) {
		case 0: case 2: ledon(); break;
		case 1: case 3: ledoff(); break;
	} 
	
	if (++j >= 5) {
		j = 0;
		Flag.Meas = TRUE;
	}
}

static void txdata(int8u rfdata)
{
	RFPORT = (RFPORT & 0x0f) | (rfdata << 4);
	RFPORT &= ~_BV(RF_EN_PIN);
	dlyms(100);
	RFPORT |= _BV(RF_EN_PIN);
}
	
ISR(INT0_vect)
{
	Flag.Mchg = TRUE;
	GICR |= _BV(INT0);
}