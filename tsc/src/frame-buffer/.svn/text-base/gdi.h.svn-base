#ifndef INCLUDED_GDI_H
#define INCLUDED_GDI_H

#ifdef __cplusplus
extern "C" {
#else
#define bool int
#define true 1
#define false 0
#endif

//#define USE_WINDOW 1
typedef struct BITMAP_T 
{
	int m_Width;
	int m_Height;
	int m_Color;
	unsigned char *m_pData;
} BITMAP_T;

typedef struct 
{
	char m_R;
	char m_G;
	char m_B;
} CanvasColor;

typedef struct 
{
	BITMAP_T m_Bitmap;
	int m_CurX;
	int m_CurY;
	CanvasColor m_Color;
}Canvas;

void FreeBmp(BITMAP_T *bmp);
#ifdef USE_WINDOW
//void SetGDIDC(HDC hdc);
#endif
BITMAP_T* GetCanvas();
int InitGDI(unsigned char *buf);
int InitBmpIndirect(BITMAP_T *bmp,int w, int h ,void *buf,int clrs);
int InitBmp(BITMAP_T *bmp, int w,int h,int clrs);
int FindChar(short code, BITMAP_T *bmp );
int Encode565(CanvasColor clr,short *s);
CanvasColor Decode565(short *clr);

CanvasColor MakeColor ( char r, char g, char b );
int SetColor(CanvasColor clr);
int PutPixel(int x, int y, CanvasColor clr);
int GetPixel(int x, int y , BITMAP_T *bmp, char *R , char *G ,char *B);


int Canvas_PutChar(short ch);
int Canvas_MoveTo( int x, int y );
int Canvas_LineTo( int x,int y );
int Canvas_TextOut(int x,int y ,const char *str/*,int n*/);
int Canvas_Circle ( int x, int y , int r);
int Canvas_WriteChar(int x ,int y , short ch);
int Canvas_Rectangle(int x, int y , int w, int h);

#ifdef __cplusplus
};
#endif

#endif
