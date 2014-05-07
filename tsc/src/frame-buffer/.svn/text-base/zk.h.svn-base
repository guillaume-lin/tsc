#ifndef INCLUDED_ZK_H
#define INCLUDED_ZK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "gdi.h"

typedef struct 
{
	unsigned short code;
	long m_Offset;  // This offset is start from the end of index in the char library.
}CharIndex;

typedef union 
{
	struct
	{
	char m_ch1;
	char m_ch2;
	} m_ch;
	short m_s;
}CharShort;
#define m_ch1 m_ch.m_ch1
#define m_ch2 m_ch.m_ch2

typedef struct 
{
	int m_Num_Char ;
	int m_Width;
	int m_Height;
	CharIndex *m_pIndexes;
	FILE * m_pFile;
}CharLib;

extern CharLib globalCharLib;
short CharToShort(char c1,char c2);
int InitLogicCharLib(CharLib *lib,int num,int w, int h, FILE *file);
int InitCharLib( const char *lib);
int WriteCharLib( const char *lib,CharLib *data);
long FindCharOffset(short ch);
int ReadMask(long off, BITMAP_T *bmp);
int IsHZ(char ch);

#ifdef __cplusplus
};
#endif

#endif
