/**
 * rs485_sample.c
 * 
 * rs485 sample from bsky
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(林景贤)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-08-10 15:57:05 中国标准时间
 * 
 * $log$
 *
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/io.h>

#define TX 0
#define RX 0
#define DLL 0
#define DLH 1
#define FCR 2
#define LCR 3
#define LSR 5
#define MCR 4
#define MSR 6

void outportb(int addr, unsigned char ch)
{
	outb(ch,addr);
}
unsigned char inportb(int addr)
{
	return inb(addr);
}


void delay(int sec)
{
	sleep(sec);
}
void Init(unsigned short com_address, unsigned short bound_rate)
{
    unsigned char LSB, MSB;

    switch (bound_rate)
    {
         case 2400: LSB = 0x30;
                    break;
         case 3600: LSB = 0x20;
                    break;
         case 4800: LSB = 0x18;
                    break;
         case 9600: LSB = 0x0c;
                    break;
         default:   break;
    }
    MSB = 0x00;
    outportb (com_address + LCR, 0x80);
    outportb (com_address + DLH, MSB);
    outportb (com_address + DLL, LSB);
    outportb (com_address + LCR, 0x03);
    outportb (com_address + MCR, 0x00);
    outportb (com_address + FCR, 0x07);
    outportb (com_address + FCR, 0x00);

}

void send(unsigned short com_address, unsigned char ch)
{
          outportb (com_address + TX, ch);
}

unsigned char recieve(unsigned short com_address)
{
       unsigned char temp;

       temp = inportb (com_address + RX);
       printf ("%c ", temp);

       return temp;
}
unsigned char get_status(unsigned short com_address)
{
     unsigned char status;

     status = inportb (com_address + LSR);

     return status;
}

int main()
{
    unsigned char com_status, tmp;
    unsigned short com1, com2;
    long i;

    if(iopl(3) != 0){
	    perror("iopl");
	    return -1;
    };
    com1 = 0x3E8;
    com2 = 0x2E8;

    Init (com1, 9600);
    Init (com2, 9600);

    while (1) {
         if (get_status(com1) & 0x60) {
                outportb (com1 + MCR, 0x03);
                delay (1);
                send (com1, '1');
         }

        if(get_status(com2) & 0x01) {
                 outportb (com2 + MCR, 0x03);
                 delay (1);
                 recieve (com2);
                 delay (1);
                 outportb (com2 + MCR, 0x00);
                 delay (1);

          }
      }
    return 0;
}

