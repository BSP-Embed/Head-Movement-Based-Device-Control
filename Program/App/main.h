#ifndef MAIN_H
#define MAIN_H

#include "includes.h"

//DEFINE PORTS 
#define INTDDR				DDRD
#define INTPORT				PORTD
#define INT0_PIN			PD2
#define INT1_PIN			PD3

#define RFDDR			DDRA
#define RFPORT			PORTA
#define RF_EN_PIN		PA3

#define AUDIO_DDR		DDRD
#define AUDIO_PORT		PORTD

struct  {
	volatile int8u Meas:1;
	volatile int8u Mchg:1;
} Flag;

enum {VOICE = 0, CTRL = 1};

//DEFINE CONSTANT
#define THRESHOLD		50

//DEFINE MACROS
#define StartTmr()		TCCR0  	|= _BV(CS01)
#define StopTmr()		TCCR0  	&= ~_BV(CS01)


//FUNCTION PROTOTYPES
static void		 init		(void);
static void 	 disptitl 	(void);
static void 	tmr1init	(void);
static void 	inittest	(void);
static void		GPIOinit	(void);
static void 	txdata		(int8u rfdata);
static void		sensinit	(void);
static void		EXT0init	(void);
#endif
