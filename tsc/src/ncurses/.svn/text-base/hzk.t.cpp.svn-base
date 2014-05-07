/* @(#)hzk.t.cpp
 */
#include <iostream>
#include <cstring>
#include <cassert>
#include "hzk.hpp"


int main(int argc, char* argv[])
{
	CHzk m_hzk;
	char chinese[] = "≤Ω∫≈‘À–– £”‡";
	char ascii[] = "0123456789";

	printf("sizeof(%s) = %d\n",chinese,sizeof(chinese));
	printf("sizeof(%s) = %d\n",ascii,sizeof(ascii));
	for(int i=0; i<strlen(chinese); i+=2)
		m_hzk.addChar(chinese[i],chinese[i+1]);
	for(int i=0; i<strlen(ascii); i++)
		m_hzk.addChar(ascii[i]);

	m_hzk.loadChineseFont("/usr/local/lib/zhcon/font/gb-48-song.bpsf");
	m_hzk.loadAsciiFont("/usr/local/lib/zhcon/font/asc48.bpsf");


	char c,c1,c2;

	for(int i=0; i<10; i++){
		m_hzk.get_char_code(i,c);
		assert(c == ascii[i]);
	}
	return 0;
}




