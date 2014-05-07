#ifndef INCLUDED_ISR_SMBUS_H
#define INCLUDED_ISR_SMBUS_H

#include "mcu.h"
#define	SMB_BUS_ERROR  	0x00					// (all modes) BUS ERROR
#define	SMB_START      	0x08					// (MT & MR) START transmitted
#define	SMB_RP_START   	0x10					// (MT & MR) repeated START
#define	SMB_MTADDACK   	0x18					// (MT) Slave address + W transmitted,ACK received;
#define	SMB_MTADDNACK  	0x20					// (MT) Slave address + W transmitted,NACK received;
#define	SMB_MTDBACK    	0x28					// (MT) data byte transmitted; ACK rec'vd
#define	SMB_MTDBNACK   	0x30					// (MT) data byte transmitted; NACK rec'vd
#define	SMB_MTARBLOST  	0x38					// (MT) arbitration lost
#define	SMB_MRADDACK   	0x40					// (MR) Slave address + R transmitted,ACK received;
#define	SMB_MRADDNACK  	0x48					// (MR) Slave address + R transmitted,NACK received;
#define	SMB_MRDBACK    	0x50					// (MR) data byte rec'vd; ACK transmitted
#define	SMB_MRDBNACK   	0x58					// (MR) data byte rec'vd; NACK transmitted

extern bit f_rd_eeprom_now,f_eeprom_rd,f_step_all;
extern unsigned int rdt_num,rdt_num_bak;
extern unsigned char xdata *start_addr_ram;
#endif