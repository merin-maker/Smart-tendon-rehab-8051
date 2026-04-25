// Initial logic generated with assistance from ChatGPT; hardware integration and debugging by Merin Roy.
#include <reg51.h> 
sbit servo = P1^0; 
sbit led = P1^1; 
sbit buzzer = P1^2; 
sbit SDA = P2^0; 
sbit SCL = P2^1; 
#define MPU_ADDR 0xD0 
#define LOW_LIMIT 105 #define HIGH_LIMIT 145 
void delay_us(unsigned int t)
{ 
  while(t--); 
} 
void delay_ms(unsigned int ms)
{
  unsigned int i,j; 
  for(i=0;i<ms;i++) 
    for(j=0;j<1275;j++); 
} 
void servo_position(unsigned int pulse) 
{ 
  int i;
  for(i=0;i<25;i++)
    {
      servo = 1; 
      delay_us(pulse); 
      servo = 0;
      delay_ms(20); 
    }
} 
void I2C_Start() 
{ 
  SDA = 1;
  SCL = 1; SDA = 0;
  SCL = 0; 
} 
void I2C_Stop() 
{
  SDA = 0; 
  SCL = 1;
  SDA = 1;
} 
void I2C_Write(unsigned char dat)
{
  unsigned char i; 
  for(i=0;i<8;i++)
    {
      SDA = dat & 0x80;
      SCL = 1;
      SCL = 0;
      dat <<= 1;
    }
  SDA = 1; 
  SCL = 1;
  SCL = 0;
} 
unsigned char I2C_Read() 
{
  unsigned char i,
  dat=0;
  SDA = 1; 
  for(i=0;i<8;i++)
    {
      SCL = 1;
      dat = (dat<<1) | SDA; SCL = 0;
    }
  return dat;
} 
void MPU_Write(unsigned char reg, unsigned char val)
{
  I2C_Start();
  I2C_Write(MPU_ADDR); 
  I2C_Write(reg);
  I2C_Write(val);
  I2C_Stop();
} 
unsigned char MPU_Read(unsigned char reg) 
{
  unsigned char val;
  I2C_Start();
  I2C_Write(MPU_ADDR);
  I2C_Write(reg);
  I2C_Start();
  I2C_Write(MPU_ADDR | 1);
  val = I2C_Read();
  I2C_Stop(); 
  return val;
} 
void MPU_Init() 
{
MPU_Write(0x6B,0); 
} 
void main() 
{
unsigned char accel;
bit limit_state = 0;
MPU_Init();
servo_position(1500);
while(1) 
{
accel = MPU_Read(0x3B);
if(accel >= LOW_LIMIT && accel <= HIGH_LIMIT)
{
if(limit_state == 1)
{
servo_position(1500);
limit_state = 0;
}
  led = 0;
  buzzer = 0;
} 
else
{
if(limit_state == 0)
{
servo_position(1750);
limit_state = 1;
}
  buzzer = 1;
  led = 1;
  delay_ms(250);
  led = 0;
  delay_ms(250);
}
}
}
