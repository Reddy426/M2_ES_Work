// Filename : PasswordLock.C
// Author : FHN
// Date : 03-03-2018
// MCU : ATTINY2313
// DISPLAY : LCD 16x1
// MISC : 3 buttons  1 relay  1jumper

#define F_CPU 1000000L  

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#define LCD_DP	PORTB
#define LCD_CP	PORTD
#define LCD_EN	0b01000000
#define LCD_RS	0b00100000


#define B_UP	0b00000100 
#define B_RIGHT	0b00001000 
#define B_ENTER	0b00010000 
#define J_ERAS	0b00000001 
#define B_ALL 	0b00011100

#define RELAY	0b00000001 
#define BLKR	0b00000010  

#define OPEN  1
#define CLOSE 0


uint8_t LCD_COL=1;
char S_PWD[]="XXXXXX";
char E_PWD[]="000000";
char H_PWD[]="******  ";

//============================================================

void initPorts ( )
{
	DDRB = 0xFF; 
	DDRA = 0x11; 
	DDRD = 0b01100000;
}
//============================================================
void erPwd()
{
	eeprom_update_block ( (const void *) S_PWD, (void *) 1 , 6 ); 
	_delay_ms ( 50 );
}
//============================================================
int rdPwd ( )
{
	eeprom_read_block ( (void *)S_PWD, (void *)(1), 6); 

	if ( S_PWD[0]<'0' || S_PWD[0]>'9' )
	{
		S_PWD[0] = 'X';
		return -1;
	}

	return 0;
}
/************************************************************/
int wrPwd ( )
{
	eeprom_update_block((const void *) E_PWD, (void *)1 , 6); 
	_delay_ms ( 50 );
	return ( rdPwd ( ) );
}
//=============================================================================
void LCD_EB ( )
{
	

    LCD_CP |= LCD_EN; 
    _delay_us(50);	
   LCD_CP &= (~LCD_EN ); 
   _delay_us(30);		
}
//=============================================================================
void LCD_CM ( unsigned char cmd )
{
	LCD_CP &= (~LCD_RS );
	LCD_DP = cmd;
	LCD_EB ( );
}
//=============================================================================
void LCD_CS ( )
{
	LCD_COL=1;
	LCD_CM ( 0x01 );
	_delay_ms(3);
}
//=============================================================================
void LCD_ST( ) 
{

	unsigned char iv = 0x30; 


	_delay_ms ( 50 ); 
	LCD_CM  ( 0x30 );
	_delay_ms ( 20 ); 
	LCD_CM  ( 0x30 );
	_delay_us ( 200 );
	LCD_CM  ( 0x30 );
	_delay_ms ( 100 );

	iv |= 0b00001000; 
	iv |= 0b00000100; 
	LCD_CM ( iv ); 
	_delay_ms ( 25 );

	LCD_CM ( 0x0F );  
	_delay_ms ( 25 );

	LCD_CM ( 0x06 );
	_delay_ms ( 50 );

}
//=============================================================================
void LCD_CL ( uint8_t pos )
{
	LCD_CM ( 0xC0+pos+0 ); 
	_delay_ms(3);
}
//=============================================================================

void LCD_CH ( unsigned char c )
{
	if ( LCD_COL==9 )
		LCD_CM ( 0xC0 ); 


	LCD_CP |= LCD_RS; 
	LCD_DP = c;
	LCD_EB ( );
	LCD_COL++;
}
//=============================================================================
void LCD_WS ( char s[] )
{
	for (int i=0; s[i]!=0; i++)
		{
		LCD_CH ( s[i] );
		_delay_us ( 20 ); 
		}
}
//============================================================
int edPwd( )
{
	uint8_t loops = 100;
	uint8_t ep = 0;
	uint8_t pst = 0xFF;

	E_PWD[0]='0';
	E_PWD[1]='0';
	E_PWD[2]='0';
	E_PWD[3]='0';
	E_PWD[4]='0';
	E_PWD[5]='0';

	LCD_CS ( );
	if (S_PWD[0]=='X')
		LCD_WS ( "NEW " );
	else
		LCD_WS ( "Open" );
	LCD_WS ( "PWD:" );
	while ( loops > 0 )
	{
		loops--;
		PORTA ^= BLKR;
		LCD_CL ( 0 );
		LCD_WS ( H_PWD );
		LCD_CL ( ep );
		LCD_CH ( E_PWD[ep] );
		LCD_CL ( ep );
		_delay_ms(50);

		if (pst != B_ALL)
		{
			loops = 100;
			_delay_ms(500);
		}

		pst = (PIND & B_ALL);

		if (( pst & B_UP ) == 0 )
		{
			E_PWD[ep]++;
			if ( E_PWD[ep] > '9' )
				E_PWD[ep] = '0';
		}
		else if (( pst & B_RIGHT ) == 0 )
		{
			ep++;
			if ( ep > 5 )
				ep = 0;
		}
		else if (( pst & B_ENTER ) == 0 )
		{
			return 1;  // edited
		}

	}
	return 0;

}
//============================================================
void setLock ( uint8_t stat )
{
	if (stat)
		PORTA |= RELAY;
	else
		PORTA &= (~RELAY);
}
//============================================================
int main ( )
{
	uint8_t retval=0;

	_delay_ms ( 10 );

	initPorts ();
	setLock ( CLOSE );
	_delay_ms ( 3000 );
	
	LCD_ST ( );
	_delay_ms ( 100 );

	LCD_CS();
	LCD_WS("PWD LOCK");
	_delay_ms ( 2000 );

	if ( ( PIND & J_ERAS ) == 0 )
	{
		erPwd();
		LCD_CS();
		LCD_WS("ERASED");
		_delay_ms ( 2000 );
	}

	rdPwd ( );

	LCD_CS();
	_delay_ms ( 50 );
	
	
	while ( 1 )
	{
		
		setLock ( CLOSE );
		retval = edPwd();
		PORTA &= (~BLKR);
		if ( retval > 0 )
		{
			if ( S_PWD[0]=='X' ) 
			{
				if ( E_PWD[0]=='0' && E_PWD[1]=='0' && E_PWD[2]=='0' &&
					 E_PWD[3]=='0' && E_PWD[4]=='0' && E_PWD[5]=='0' )
				{
						LCD_CL ( 0 );
						LCD_WS("INVALID");
				}
				else
				{
					wrPwd();
					LCD_CL ( 0 );
					LCD_WS("SAVED.");
				}
			}
			else
			{
				if ( E_PWD[0]==S_PWD[0] && E_PWD[1]==S_PWD[1] && E_PWD[2]==S_PWD[2] &&
					 E_PWD[3]==S_PWD[3] && E_PWD[4]==S_PWD[4] && E_PWD[5]==S_PWD[5] )
					 {
						setLock ( OPEN );
						PORTA |= BLKR;
						LCD_CS();
						LCD_WS("LOCK OPENED");	
						_delay_ms(6000);
					 }
				else
					{
					 LCD_CL ( 0 );
					 LCD_WS("INVALID");
					 }
			}
			_delay_ms ( 4000 );
		}

		_delay_ms ( 50 );
	
	
	}
}
//============================================================









