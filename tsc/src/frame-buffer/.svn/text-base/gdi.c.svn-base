#include <malloc.h>
#include <memory.h>
#include <math.h>
#include "gdi.h"
#include "zk.h"
#include "fb_util.h"

//#define BMP565 1
Canvas globalCanvas;
/*#ifdef USE_WINDOW
HDC hWndDC;

void SetGDIDC(HDC hdc)
{
	hWndDC = hdc;
}
#endif*/
BITMAP_T* GetCanvas()
{
	return &globalCanvas.m_Bitmap;
}
char Convert(char c,bool dec)
{
	char temp=0;
	if(!dec)
	{
		int a=c;
		//TRACE("a=%d ",a);
		a=a*255.0/32.0;
		//TRACE("ac=%d \n",a);
		temp=a;
		
	}
	else
	{
		int a=0;
		a=c;
		//TRACE("dec a=%d \n",a);
		a=(a*32.0)/255.0;
		//TRACE("dec ac=%d \n",a);
		temp = a;
		
	}
	return temp;
}
int Encode565(CanvasColor clr,short *s)
{
	*s=0;
    
	*s|=Convert(clr.m_R,true) <<11;
	*s|=Convert(clr.m_G,true) <<5;
	*s|=Convert(clr.m_B,true) ;
	return 0;
}
CanvasColor Decode565(short *clr)
{
	CanvasColor a;
	a.m_R =Convert((*clr)&0xf800>>11,false);
	a.m_G =Convert((*clr)&0x07e0>>5,false);
	a.m_B = Convert((*clr)&0x001f,false);
	return a;
}
int InitBmpIndirect(BITMAP_T *bmp,int w, int h ,void *buf,int clrs)
{
	
	bmp->m_Width =w ;
	bmp->m_Height =h ;
	bmp->m_Color =clrs;
	bmp->m_pData = (unsigned char*)buf;
	memset(bmp->m_pData,0x00,bmp->m_Width *bmp->m_Height* sizeof(char)*bmp->m_Color);
	return 0;
}

int InitBmp(BITMAP_T *bmp, int w,int h,int clrs)
{
	bmp->m_Width =w ;
	bmp->m_Height =h ;
	bmp->m_Color =clrs;
	bmp->m_pData = (unsigned char*)malloc( bmp->m_Width *bmp->m_Height* sizeof(char)*bmp->m_Color);
	memset(bmp->m_pData,0xff,bmp->m_Width *bmp->m_Height* sizeof(char)*bmp->m_Color);
	return 0;
}

int InitGDI(unsigned char *buf)
{
	globalCanvas.m_Color=MakeColor(255,255,255);
	globalCanvas.m_CurX =0;
	globalCanvas.m_CurY =0;
	if (buf)
		InitBmpIndirect(&globalCanvas.m_Bitmap,640,480,buf,2);
	else
#ifndef BMP565
	InitBmp(&globalCanvas.m_Bitmap,640,480,3);
#else
	InitBmp(&globalCanvas.m_Bitmap,640,480,2);
#endif
	return 0;
}
CanvasColor MakeColor ( char r, char g, char b )
{
	CanvasColor clr;
	clr.m_R = r;
	clr.m_B = b;
	clr.m_G = g;
	return clr;
}

int SetColor(CanvasColor clr)
{
	globalCanvas.m_Color =clr;
	return 0;
}
int Valid(int x ,int y )
{
	if ( x >= globalCanvas.m_Bitmap.m_Width ||
		y >=globalCanvas.m_Bitmap.m_Height)
		return -1;
	if (x <0 || y<0)
		return -1;
	return 0;
}

int PutPixel(int x, int y, CanvasColor clr)
{
#if 1
	int which;
#ifndef USE_WINDOW
	if (Valid(x,y))
		return -1;
#ifdef BMP565
	which = (x*globalCanvas.m_Bitmap.m_Height  +y)*2;
	//TRACE("x=%d,y=%d;locate=%d\n",x,y,which);
	Encode565(clr,(short*)(globalCanvas.m_Bitmap.m_pData +which));
#else
	which = (x*globalCanvas.m_Bitmap.m_Width  +y)*3;
	*(globalCanvas.m_Bitmap.m_pData+which) = clr.m_R;
	*(globalCanvas.m_Bitmap.m_pData+which+1) = clr.m_G;
	*(globalCanvas.m_Bitmap.m_pData+which+2) = clr.m_B;
#endif
#else
	//COLORREF clr1;
	//clr1=RGB(clr.m_R ,clr.m_G,clr.m_B);
	//TRACE("p(%d,%d)\n",x,y);
	//::SetPixel (hWndDC,x,y,clr1);
#endif
#endif
	return 0;
}
int GetPixel(int x, int y , BITMAP_T *bmp, char *R , char *G ,char *B)
{
#ifndef  BMP565 
	int which = x*bmp->m_Height +y;
	*R = *(bmp->m_pData+ which);
	*G = *(bmp->m_pData+ which+1);
	*B = *(bmp->m_pData+ which+2);
#else
	int which = (x*bmp->m_Height+y)*2;
	//if(which>=640*480*2)
	//	TRACE("(%d,%d) which =%d,start=%x,locate=%x,out of range!\n",x,y, which ,bmp->m_pData,bmp->m_pData+which);
	CanvasColor c = Decode565((short*)(bmp->m_pData+which));
	*R=c.m_R;
	*G=c.m_G;
	*B=c.m_B;
#endif
	return 0;
}
int GetCharPixel(int x ,int y ,BITMAP_T *bmp,int chW )
{
	//TRACE("x=%d,y=%d\n",x,y);
	int theByte= (x * chW +y)/8;
	//TRACE("byte =%d,char=0x%02x,bit =%d,mask=0x%02x\n",theByte,bmp->m_pData[theByte],7-(x*chW+y)%8,(1<<(7-(x*chW+y)%8)));

	if(bmp->m_pData[theByte]&(1<<(7-(x*chW+y)%8)))
	{
		//TRACE("Result=1\n");
		return 1;
	}
	//TRACE("Result=0\n");
	return 0;
}
int Canvas_MoveTo( int x, int y )
{
	if(Valid(x,y))
		return -1;
	globalCanvas.m_CurX =x ;
	globalCanvas.m_CurY =y ;
//#ifdef USE_WINDOW
//	POINT pt;
//	::MoveToEx(hWndDC,globalCanvas.m_CurX,globalCanvas.m_CurY,&pt);
//#endif
	return 0;
}
int Canvas_LineTo( int xe,int ye )
{
	
                /* Assumes -1 <= m <= 1, x0 < x1 */
   int     x;       /* x runs from x0 to x1 in unit increments. */
   float   dy, dx, y, m;
   int x0 = globalCanvas.m_CurX;
   int y0 = globalCanvas.m_CurY;
   int x1 = xe;
   int y1 =ye;

   if ( Valid(xe,ye))
	   return -1;
   dy = y1 - y0;
   dx = x1 - x0;

   if(dx!=0)
   {
		m = dy / dx;
		y = y0;
		if (dy==0)
		{
			if (x0>xe)
			{
				y=y0;
				for( x=x0;x<xe;x++)
					PutPixel(x,y, globalCanvas.m_Color );
			}
			else{
				y=y0;
				for ( x=xe;x<x0	;x++)
					PutPixel(x,y, globalCanvas.m_Color );
			}
	
		}
		else
		{
			for ( x = x0; x <= x1; x++ ) 
			{
				PutPixel ( x, (int) floor ( y + 0.5 ), globalCanvas.m_Color ); 
				/* Set pixel to value */
				y += m;                         /* Step y by slope m */
			}
		}
		
   }
   else
   {
	   x=x0;
	   if (ye>=y0){
		   for (y=y0;y<=ye;y++) {
			   PutPixel(x,y,globalCanvas.m_Color);
		   }
	   }
	   else {
		   for (y=y0;y>=ye;y--)	{
			   PutPixel(x,y,globalCanvas.m_Color);
		   }
	   }
   }
   globalCanvas.m_CurX = xe;
   globalCanvas.m_CurY = ye;
//#ifdef USE_WINDOWS
//	POINT pt;
//	::MoveToEx(hWndDC,globalCanvas.m_CurX,globalCanvas.m_CurY,&pt);
//#endif
	
   return 0;

}
int Canvas_TextOut(int x,int y ,const char *str/*,int n*/)
{
	short code;
	char *str1 =(char*)str;
	Canvas_MoveTo(x,y);
	//int i=0;
	while(*str1 /*&& i<n*/)
	{
		//i++;
		if (IsHZ(*str1))
		{
			//TRACE("%02x %02x hz\n",*str,*(str+1));
			code =0x0000;
			code = CharToShort(*str1,*(str1+1));
			//TRACE("%04x\n",code);
			Canvas_PutChar(code);
			str1 +=2;
		}
		else
		{
			code =0x0000;
			code = CharToShort(0,*str1);
			Canvas_PutChar(code);
			str1 ++;
		}

		
	}
	return 0;
}
int Canvas_Circle ( int xCenter, int yCenter , int radius)
{
        int x, y, r2;
#if 0
        r2 = radius * radius;
        PutPixel( xCenter, yCenter + radius,globalCanvas.m_Color);
        PutPixel( xCenter, yCenter - radius,globalCanvas.m_Color);
        for (x = 1; x <= radius; x++) 
		{
            y = (int) (sqrt((double)(r2 - x*x)) + 0.5);
            PutPixel( xCenter + x, yCenter + y,globalCanvas.m_Color);
            PutPixel( xCenter + x, yCenter - y,globalCanvas.m_Color);
            PutPixel( xCenter - x, yCenter + y,globalCanvas.m_Color);
            PutPixel( xCenter - x, yCenter - y,globalCanvas.m_Color);
        }
#endif
   	return 0;
}
int Canvas_PutChar(short ch)
{
	return Canvas_WriteChar(globalCanvas.m_CurX ,globalCanvas.m_CurY,ch);
}
int Canvas_Rectangle(int x, int y , int w, int h)
{
	Canvas_MoveTo(x,y);
	Canvas_LineTo(x+w,y);
	Canvas_LineTo(x+w,y+h);
	Canvas_LineTo(x,y+h);
	Canvas_LineTo(x,y);
	return 0;
}
int Canvas_WriteChar(int x ,int y , short ch)
{
	BITMAP_T bmp;
	int i,j;
	InitBmp(&bmp,6,48,1);
	if(FindChar(ch,&bmp))
	{
		FreeBmp(&bmp);
		globalCanvas.m_CurY += 48 +1;
		return -1;
	}
	//char data[6*48]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x0F,0xC0,0x00,0x00,0x00,0x00,0x0F,0x80,0x00,0x00,0x00,0x00,0x0F,0x80,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xFF,0xF8,0x07,0xFF,0xFF,0xFF,0xFF,0xF8,0x07,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,0x00,0x7C,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0xF0,0x0F,0x00,0x00,0x00,0x01,0xE0,0x0F,0x00,0x00,0x00,0x03,0xE0,0x0F,0x00,0x00,0x00,0x03,0xC0,0x0F,0x00,0x00,0x00,0x3F,0xC0,0x0F,0x00,0x00,0x00,0x3F,0x80,0x0F,0x00,0x00,0x00,0x3F,0x00,0x0F,0x00,0x00,0x00,0x3F,0x00,0x0F,0x00,0x00,0x00,0x7E,0x3F,0xFF,0xFF,0xE0,0x00,0xFC,0x3F,0xFF,0xFF,0xE0,0x01,0xFC,0x3F,0xFF,0xFF,0xE0,0x03,0xFC,0x3F,0xFF,0xFF,0xE0,0x07,0xFC,0x00,0x0F,0x00,0x00,0x1F,0xFC,0x00,0x0F,0x00,0x00,0x3F,0xBC,0x00,0x0F,0x00,0x00,0x1F,0x3C,0x00,0x0F,0x00,0x00,0x0E,0x3C,0x00,0x0F,0x00,0x00,0x04,0x3C,0x00,0x0F,0x00,0x00,0x00,0x3C,0x00,0x0F,0x00,0x00,0x00,0x3C,0x00,0x0F,0x00,0x00,0x00,0x3C,0x00,0x0F,0x00,0x00,0x00,0x3C,0x00,0x0F,0x00,0x00,0x00,0x3C,0x00,0x0F,0x00,0x00,0x00,0x3C,0xFF,0xFF,0xFF,0xF8,0x00,0x3C,0xFF,0xFF,0xFF,0xFC,0x00,0x3C,0xFF,0xFF,0xFF,0xFC,0x00,0x3C,0xFF,0xFF,0xFF,0xFC,0x00,0x3E,0x00,0x00,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	//bmp.m_Height=6;
	//bmp.m_Width =48;]
	//InitBmp(&bmp,6,48,1);
	//memcpy(bmp.m_pData ,data,48*6);
	//int which = x*globalCanvas.m_Bitmap.m_Width *3 +y;
	for ( i =0;i < 48; i++)
		for ( j =0;j<48;j++)
		{
			if(GetCharPixel(i,j,&bmp, 48))
			{
				PutPixel(x+j,y+i,globalCanvas.m_Color);
			}
		}
	globalCanvas.m_CurX += 48 +1;
	FreeBmp(&bmp);
	return 0;
}

int FindChar(short code, BITMAP_T *bmp )
{
	long len = FindCharOffset(code);
	if (len == -1)
		return -1;
	return ReadMask(len, bmp);
}
void FreeBmp(BITMAP_T *bmp)
{
	free(bmp->m_pData );
}
