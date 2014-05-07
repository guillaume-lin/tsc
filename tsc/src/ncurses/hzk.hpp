/* 
 *@(#)hzk.hpp
 */

#ifndef _HZK_HPP
#define _HZK_HPP 1

#include <stddef.h>
#include <string>
#include "../frame-buffer/canvas.h"

#define MAX_CHINESE_CHARACTER 12
#define MAX_ASCII_CHARACTER   11
class CHzk {
public:
	CHzk();
	~CHzk();

	int loadChineseFont(char* fontfile);
	int loadAsciiFont(char* fontfile);

	int addChar(char c1, char c2);
	int addChar(char c);

	mem_bitmap_t* getBitmap(char c1, char c2);
	mem_bitmap_t* getBitmap(char c);

	int get_char_code(int i, char& c1, char& c2);
	int get_char_code(int i, char& c);

private:

	int m_ascii_index[MAX_ASCII_CHARACTER];
	mem_bitmap_t m_ascii[MAX_ASCII_CHARACTER];
	int m_ascii_counter;
	int m_chinese_index[MAX_CHINESE_CHARACTER];
	mem_bitmap_t m_chinese[MAX_CHINESE_CHARACTER];
	int m_chinese_counter;

	std::string m_ascii_font;
	std::string m_chinese_font;
};
#endif /* _HZK_HPP */
