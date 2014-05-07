/**
 *  log.hpp
 *  logger for debugging purpose
 *  author: ljx
 *  date: 2014-4-26
 */


#include <iostream>
#include <stdio.h>
#include <errno.h>
#include "Log.hpp"

static const char* LOG_FILE="/dev/tty3";
void Log::d(const char* tag, const char* msg)
{	
	FILE* fp = fopen(LOG_FILE,"a+");
	if(fp == NULL){
		std::cerr<<"open log file failed."<<std::endl;
		perror(tag);
		return;
	}
	fprintf(fp,"%s: %s (system error: %s)",tag,msg,strerror(errno));
	fclose(fp);
}
