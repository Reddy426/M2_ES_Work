#ifndef UARTI_H_
#define UARTI_H_


/************************************************************************* 
For initializing UART for Tx-Rx @8Mhz, 9600bps, 8 bit transmission,1 stop bit
*************************************************************************/

void UART_init()
{
UBRRH=0;
UBRRL=51;
UCSRC=(1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
}

/************************************************************************* 
For Transmitting one byte of data via PD0
Use: UART_tx(data_byte);
*************************************************************************/

void UART_tx(uint8_t tx)
{
UDR=tx;
while(!(UCSRA&(1<<UDRE)));
}

/************************************************************************* 
For Receiving one byte of data via PD1
Use: unsigned char var=UART_rx();
*************************************************************************/

unsigned char UART_rx()
{
while((UCSRA&(1<<RXC))==0);
return(UDR);
}

/************************************************************************* 
For Transmitting string via PD1
Use: UART_txs(string[]);
*************************************************************************/

void UART_txs(uint8_t s[])
{
unsigned int a=0;
_delay_ms(10);
while(s[a]!='\0')
{
UART_tx(s[a]);
_delay_ms(100);
a++;
}
}

void UART_txsl(uint8_t s[],uint8_t b)
{
unsigned int a=0;
_delay_ms(10);
while(a<b)
{
UART_tx(s[a]);
_delay_ms(100);
a++;
}
}

#endif
