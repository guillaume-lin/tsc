#include <stdlib.h>
#include "zk.h"
CharLib globalCharLib;
short CharToShort(char c1,char c2)
{
	CharShort cs;
	cs.m_s=0;
	cs.m_ch1 = c1;
	cs.m_ch2 = c2;
	return cs.m_s;
}
int InitLogicCharLib(CharLib *lib,int num,int w, int h, FILE *file)
{
	lib->m_Num_Char =num;
	lib->m_Height=h;
	lib->m_Width=w;
	lib->m_pIndexes=(CharIndex*)malloc(w*h/8*num);
	return 0;
}
int InitCharLib( const char *lib)
{
	
	globalCharLib.m_pFile = fopen(lib,"rb+");
	fread(&globalCharLib.m_Num_Char,sizeof(int),1,globalCharLib.m_pFile);
	fread(&globalCharLib.m_Width,sizeof(int),1,globalCharLib.m_pFile);
	fread(&globalCharLib.m_Height,sizeof(int),1,globalCharLib.m_pFile);
	globalCharLib.m_pIndexes = (CharIndex*)malloc(sizeof(CharIndex)*globalCharLib.m_Num_Char);
	fread(globalCharLib.m_pIndexes,sizeof(CharIndex),globalCharLib.m_Num_Char,globalCharLib.m_pFile);
	return 0;	
}
int WriteCharLib( const char *lib,CharLib *data)
{
	data->m_pFile = fopen(lib,"wb+");
	fwrite(&data->m_Num_Char,sizeof(int),1,data->m_pFile);
	fwrite(&data->m_Width,sizeof(int),1,data->m_pFile);
	fwrite(&data->m_Height,sizeof(int),1,data->m_pFile);
	//data->m_pIndexes = (CharIndex*)malloc(sizeof(CharIndex)*data->m_Num_Char);
	fwrite(data->m_pIndexes,sizeof(CharIndex),data->m_Num_Char,data->m_pFile);
	return 0;	
}
#define TRACE(x1,x2,x3) 
long FindCharOffset(short ch)
{
	int i;
	for (i =0;i<globalCharLib.m_Num_Char;i++)
	{
		TRACE("char = %04x, code =%04x\n",ch&0x0000ffff,globalCharLib.m_pIndexes[i].code);
		if ( globalCharLib.m_pIndexes[i].code == (ch&0x0000ffff) )
			return globalCharLib.m_pIndexes[i].m_Offset;
	}
	return -1;

}
int ReadMask(long off, BITMAP_T *bmp)
{
	int Offset= sizeof(int)*3+sizeof(CharIndex)*globalCharLib.m_Num_Char+off;
	int len =globalCharLib.m_Width/8*globalCharLib.m_Height;
	fseek(globalCharLib.m_pFile,Offset,SEEK_SET);
	fread(bmp->m_pData,len,1,globalCharLib.m_pFile);
	return 0;
}

int IsHZ(char ch)
{
	return ch & 0x80;
}