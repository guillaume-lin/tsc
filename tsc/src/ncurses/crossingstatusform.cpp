/* @(#)crossingstatusform.cpp
 */
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/vt.h>
#include "key_translate.h"
#include "crossingstatusform.hpp"
#include "messagebox.hpp"
#include "label.hpp"

static int get_fg_tty()
{
	return 0;
}
static int switch_to_tty(int tty)
{
	int fd;
	int ret;
	fd = open("/dev/console",O_RDWR);
	ioctl(fd,VT_ACTIVATE,tty);
	close(fd);
	return ret;
}
static int hide_cursor()
{
	curs_set(0);
	return 0;
}
static int show_cursor()
{
	curs_set(1);
	return 0;
}
CCrossingStatusForm::CCrossingStatusForm()
	:CBaseForm(8,24,0,0)
{

	F = new NCursesFormField*[2];
	F[0] = new CLabel("初始化屏幕...",0,0);
	//	F[0]->options_off(O_VISIBLE);
	F[1] = new NCursesFormField();
	InitForm(F,TRUE,TRUE);



	if(fb_bitmap_init(&m_fb,&m_bitmap) != 0){
		F[0]->set_value("帧缓冲初始化失败!");
		postKey(CTRL('X'));
		return;
	}
	if(crossing_bitmap_init(&m_crossing,&m_bitmap,"/bin/crossing.txt") != 0){
		F[0]->set_value("初始化路口配置失败！");
		postKey(CTRL('X'));
		return;
	}


	if(vm_get_lamp_group_setting(&g_vm,&m_lamp_group_setting) != 0){
		CMessageBox box("读取灯组设置失败");
		box();
		postKey(CTRL('X'));
		return;
	}
	halfdelay(3);//300ms
	hide_cursor();
	color_t bg = make_color(255,255,255);
	color_t fg = make_color(0,0,0);
	m_hzbmp.m_bg = bg;
	m_hzbmp.m_fg = fg;
	m_hzbmp.get_width = mem_bitmap_get_width;
	m_hzbmp.get_height = mem_bitmap_get_height;
	m_hzbmp.get_pixel  = mem_bitmap_get_pixel;
	m_hzbmp.put_pixel = mem_bitmap_put_pixel;
	initHzk();
	switch_to_tty(8);
	initBackground();
	fb_switch_to_screen(&m_fb.m_dev,2);
	fb_show_screen(&m_fb.m_dev,2);
}
static int is_bg_init_done = 0;
int CCrossingStatusForm::initBackground()
{

	if(!is_bg_init_done){
		is_bg_init_done = 1;
		fb_switch_to_screen(&m_fb.m_dev,2);
		bitmap_load_from_file(&m_bitmap,"/bin/crossing.bmp");
		color_t pixel = make_color(255,255,255);
		for(int y = 0; y < 480; y++)
			for(int x=480; x<640; x++){
				bitmap_put_pixel(&m_bitmap,x,y,pixel);
			}
	}
}
int CCrossingStatusForm::initHzk()
{
	char chinese[] = "模式相位配时步号运行剩余";
	char ascii[] = " 0123456789";
	for(int i=0; i<strlen(chinese); i+=2)
		m_hzk.addChar(chinese[i],chinese[i+1]);
	for(int i=0; i<strlen(ascii); i++)
		m_hzk.addChar(ascii[i]);

	m_hzk.loadChineseFont("/usr/local/lib/zhcon/font/gb-48-song.bpsf");
	m_hzk.loadAsciiFont("/usr/local/lib/zhcon/font/asc48.bpsf");

	return 0;
}
CCrossingStatusForm::~CCrossingStatusForm()
{
	fb_switch_to_screen(&m_fb.m_dev,0);
	fb_show_screen(&m_fb.m_dev,0);
	switch_to_tty(1);
	show_cursor();
	clearok(TRUE);
	fb_bitmap_fini(&m_fb);
	crossing_bitmap_fini(&m_crossing);
}
static int isHz(char c)
{
	return c & 0x80;
}
int CCrossingStatusForm::TextOut(int x, int y, char* text)
{
	y = y - 48;
	while(*text){
		if(isHz(*text)){
			mem_bitmap_t* mb = m_hzk.getBitmap(*text,*(text+1));
			if(mb){
				m_hzbmp.m_private_data = mb;
				bitmap_paint_bitmap(&m_bitmap,&m_hzbmp,x,y);
#if 0
				char s[10];
				sprintf(s,"xx.bmp");
				s[0] = *text;
				s[1] = *(text+1);
				bitmap_save_to_file(&m_hzbmp,s);
#endif
			}
			x += 48;
			text += 2;
		}else{
			mem_bitmap_t* mb = m_hzk.getBitmap(*text);
			if(mb){
				m_hzbmp.m_private_data = mb;
				bitmap_paint_bitmap(&m_bitmap,&m_hzbmp,x,y);
#if 0
				char s[10];
				sprintf(s,"x.bmp");
				s[0] = *text;
				bitmap_save_to_file(&m_hzbmp,s);
#endif
			}
			x += 24;
			text++;
		}
	}
	return 0;
}
int CCrossingStatusForm::virtualize(int c)
{
	// refresh crossing status
	machine_status_t m_data;

	if(c != CTRL('X') && (vm_get_machine_status(&g_vm,&m_data) == 0)){
		char buf[20];
		phase_t* phase = &m_data.m_current_lamp_status;
		// update lamp status
		for(int i=0; i<crossing_bitmap_get_lamp_count(&m_crossing);
		    i++){
			int st;
			int term = lamp_group_setting_get_terminal
				( &m_lamp_group_setting,i+1);
			if(term <= 0)
				continue;
			st = phase_get_output(phase,term);
			crossing_bitmap_set_lamp_status(&m_crossing,i+1,st);
		}

		// update detector status
		detector_status_t* detector = &m_data.m_current_detector_status;
		for(int i=0; i<crossing_bitmap_get_detector_count(&m_crossing);
		    i++){
			int st;
			st = detector_status_get_status(detector,i+1);
			crossing_bitmap_set_detector_status(&m_crossing,i+1,st);
		}
		// draw m_step, m_elapse_time, m_remain_time

		sprintf(buf,"模式%02d ",m_data.m_actual_control_mode);
		TextOut(480,50,buf);

		sprintf(buf,"相位%02d ",m_data.m_phase);
		TextOut(480,100,buf);

		sprintf(buf,"配时%02d ",m_data.m_timming);
		TextOut(480,150,buf);

		sprintf(buf,"步号%02d",m_data.m_step);
		TextOut(480,200,buf);

		sprintf(buf,"运行%02d ",m_data.m_elapse_time);
		TextOut(480,250,buf);

		sprintf(buf,"剩余%02d ",m_data.m_remain_time);
		TextOut(480,300,buf);

	}
	if(c > '0'  && c <= '9')
		switch_to_tty(c-'0');
	if(c != CTRL('X'))
	   crossing_bitmap_update(&m_crossing);
	return CBaseForm::virtualize(c);
}
