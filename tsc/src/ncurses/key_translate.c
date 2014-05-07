/**
 * key_translate.c
 * 
 * 
 * 
 * Company: Xiamen Uni-Creative Technology Corporation
 * Author:  Lin Jingxian(�־���)
 * E-Mail:  cckk_lin@yahoo.ie
 * Date:    2005-07-28 10:05:58 �й���׼ʱ��
 * 
 * $log$
 *
 */
#include <curses.h>
#include "key_translate.h"

// ����ת����
//
static int trans_table[] = 
{
	0, // none key

	'1','2',     '3',       KEY_ENTER,
	'4','5',     '6',       KEY_EXIT,
	'7','8',     '9',       KEY_UP,
	'0',KEY_LEFT,KEY_RIGHT,	KEY_DOWN,
	0,0,0,0,
	0,0,0,0,
	0,0,0,KEY_PPAGE,
	0,0,0,KEY_NPAGE
};

// ת������
int translate_key(int key)
{
	if(key > 0 && key < sizeof(trans_table)/sizeof(int)){
		return trans_table[key];
	}
	else{
		return trans_table[0];
	}
}

