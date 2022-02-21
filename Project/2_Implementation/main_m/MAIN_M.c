/*
===================================================================================== =================================
Port Connections:
System Active = PB2 (Also connect the ext Green LED)
Alarm Active= PB3 (Also connect the ext Red LED)
Shutdown=  PB4
Power On RLY= PB5
Supply on RLY= PB6
Emergency Supply Off= PD2
===================================================================================== ==================================
The main module controls all the other devices' supply as well as the ATX power supply.
As soon as the switch on the main module is pressed, the ATX unit is sitched n sitching the Power On relay thus holding
the supply On the system checks its status and then switches on the other modules through the Supply On Relay.
On pressing the Emergency Supply Off switch the Supply On Relay is switched off while the system remains in normal
Working state. To get the Supplies back on Use command #CONTROLLER&SOK* OR Press the Emergency Supply Switch.
===================================================================================== ==================================
INCOMING FUNCTION CALLS
0= Alarm mode (0=OFF, 1=ONN)
1= Set Time and Date
2= Reset Alarm
3= Shuts Down System
4= Switch On a Module
5= Switch off a Module
6= Test a module
===================================================================================== ==================================
OUTGOING FUNCTION CALLS
0= Switch off a module
1= Switch onn a module
2= Test a module
3= Sets alarm mode
4= Resets Alarm mode
5= Date and time
===================================================================================== ==================================
*/
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include"UARTI.h"
//=================================================================================== ====================================
#define SACT PORTB2
#define AACT PORTB3
#define SSDN PORTB4
#define PRLY PORTB5
#define SRLY PORTB6
//=================================================================================== ====================================
uint8_t year = 0;
uint8_t month = 0;
uint8_t day = 0;
uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;
uint8_t ret;
//=================================================================================== =========================================
void devswit( uint8_t did,uint8_t onoff) //first argument is device id and second argument is on or off command
{
	if(onoff==1)
	{
		PORTB|=(1<<did);
	}
	else if(onoff==0)
	{
		PORTB&=~(1<<did);
	}
}
//=================================================================================== =========================================
void shut_down()
{ uint8_t c;
	c=UART_rx();
	devswit(SSDN,1);
	devswit(SACT,0);
	_delay_ms(5000);
	_delay_ms(5000);
	_delay_ms(5000);
	_delay_ms(5000);
	devswit(SRLY,0);
	devswit(PRLY,0);
}
void device_on()
{
	devswit(PRLY,1);
	_delay_ms(5000);
	devswit(SSDN,0);
	devswit(SACT,1);
	devswit(AACT,0);
	devswit(SRLY,1);
}
//=================================================================================== =========================================
void alm_mode()
{
	uint8_t r,c;
	r=UART_rx();
	c=UART_rx();
	r=UART_rx();
	if(c=='1')
	{
		devswit(AACT,1);
		UART_txs("#Z&3*");
	}
	else if(c=='0')
	{
		devswit(AACT,0);
		UART_txs("#Z&4*");
	}
}
//=================================================================================== =========================================
void testm()
{
	uint8_t r,c;
	r=UART_rx();
	c=UART_rx();
	r=UART_rx();
	UART_tx('#');
	UART_tx(c);
	UART_txs("&2*");
}
//=================================================================================== =========================================
void conn() // Switch onn a module
{
	uint8_t r,c;
	r=UART_rx();
	c=UART_rx();
	r=UART_rx();
	UART_tx('#');
	UART_tx(c);
	UART_txs("&1*");
}
void coff() // Switch off a module
{
	uint8_t r,c;
	r=UART_rx();
	c=UART_rx();
	r=UART_rx();
	UART_tx('#');
	UART_tx(c);
	UART_txs("&0*");
}
//=================================================================================== =========================================
void time_set() // Set rtc time & date
{
	uint8_t hr[2],mi[2],da[2],mo[2],yr[4],c=0;
	c=UART_rx();
	c=0;
	while(c<2)
	{
		hr[c]=UART_rx();
		c++;
	}
	c=UART_rx();
	c=0;
	while(c<2)
	{
		mi[c]=UART_rx();
		c++;
	}
	c=UART_rx();
	c=0;
	while(c<2)
	{
		da[c]=UART_rx();
		c++;
	}
	c=UART_rx();
	c=0;
	while(c<2)
	{
		mo[c]=UART_rx();
		c++;
	}
	c=UART_rx();
	c=UART_rx();
	c=UART_rx();
	c=0;
	while(c<2)
	{
		yr[c]=UART_rx();
		c++;
	}
	c=UART_rx();
}
//=================================================================================== =========================================
void rsal()
{

	UART_txs("#Z&4*");
}

//=================================================================================== =========================================
void d_fnc( uint8_t l)
{
	switch(l)
	{
		case '0':alm_mode();
		break;
		case '1':time_set();
		break;
		case '2':rsal();
		break;
		case '3':shut_down();
		break;
		case '4':conn();
		break;
		case '5':coff();
		break;
		case '6':testm();
		break;
		case '7':ret=UART_rx();
		UART_tx("#Z&2*");
		break;
		default:break;
	}
}
//=================================================================================== =========================================
void readm()   //MODULE NAME VERIFICATION
{
	uint8_t c=0,t=0,r;
	r=UART_rx();
	if(r=='0')
	{
		r=UART_rx();
		t=UART_rx();
		d_fnc(t);
	}
}
//===================================================================================
void main()
{
	uint8_t a=0;
	UART_init();
	sei();
	DDRB=0xff;
	device_on();
	while(1)
	{
		a=UART_rx();
		if(a=='#')
		{
			devswit(AACT,1);
			readm();
			_delay_ms(1000);
			devswit(AACT,0);
		}
	}
}