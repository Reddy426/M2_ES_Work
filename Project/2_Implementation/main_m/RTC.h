#ifndef RTC_H_
#define RTC_H_

uint8_t HH[2],MM[2],SS[2],dd[2],mm[2],yr[2];

void init_i2c()
{
TWSR = 0X00;
TWBR = 0X47;
TWCR = (1<<TWEN);
}

unsigned char read_i2c()
{
TWCR = (1<<TWINT)|(1<<TWEN);
while(!(TWCR & (1<<TWINT)));
return TWDR;
}

void write_i2c(unsigned char ch)
{
TWDR = ch;
TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
while(!(TWCR&(1<<TWINT)));
}

void start()
{
TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
while((TWCR &(1<<TWINT))==0);
}

void stop()
{
TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
_delay_ms(1);
}

void rtc_write(char dev_addr,char dev_loc,char dev_data)
{
start();
write_i2c(dev_addr);
write_i2c(dev_loc);
write_i2c(dev_data);
stop();
_delay_ms(10);
}

unsigned char rtc_read(char dev_addr,char dev_loc)
{
char ch;

start();

write_i2c(dev_addr);
write_i2c(dev_loc);

start();
write_i2c(dev_addr|0x01);
ch = read_i2c();

stop();
return ch;
}


void get_time_date()
{
char ch;
ch = rtc_read(0xd0 , 0x02);
HH[0]=(ch/16+48);
HH[1]=(ch%16+48);

ch = rtc_read(0xd0 , 0x01);
MM[0]=(ch/16+48);
MM[1]=(ch%16+48);

ch = rtc_read(0xd0 , 0x00);
SS[0]=(ch/16+48);
SS[1]=(ch%16+48);

ch = rtc_read(0xd0 , 0x04);
dd[0]=(ch/16+48);
dd[1]=(ch%16+48);

ch = rtc_read(0xd0 , 0x05);
mm[0]=(ch/16+48);
mm[1]=(ch%16+48);

ch = rtc_read(0xd0 , 0x06);
yr[0]=(ch/16+48);
yr[1]=(ch%16+48);


}

void set_time_date(uint8_t H[],uint8_t M[],uint8_t d[],uint8_t m[],uint8_t y[])
{
char ch;

ch =((H[0]-'0')<<4)+(H[1]-'0');
rtc_write(0xd0,0x02,ch);//HH

ch =((M[0]-'0')<<4)+(M[1]-'0');
rtc_write(0xd0,0x01,ch);//MM

rtc_write(0xd0,0x00,0x00);//SS

ch =((d[0]-'0')<<4)+(d[1]-'0');
rtc_write(0xd0,0x04,ch);//dd

ch =((m[0]-'0')<<4)+(m[1]-'0');
rtc_write(0xd0,0x05,ch);//mm

ch =((y[0]-'0')<<4)+(y[1]-'0');
rtc_write(0xd0,0x06,ch);//yr
}

#endif