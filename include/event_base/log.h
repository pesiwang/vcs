#ifndef _VCS_LOG_H
#define _VCS_LOG_H

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#define _log_printf(ls, fn, ln, ... ) do { \
	time_t time_now; time_now = time(&time_now); \
	struct tm *tm_now = localtime((const time_t*)&time_now); \
	fprintf(stderr, "[%04d-%02d-%02d %02d:%02d:%02d][%s][%s:%d]", \
			1900 + tm_now->tm_year, \
			1 + tm_now->tm_mon, \
			tm_now->tm_mday, \
			tm_now->tm_hour, \
			tm_now->tm_min, \
			tm_now->tm_sec , \
			ls, fn, ln); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
}while(0)

#define log_error( ... ) _log_printf("ERROR", __FILE__, __LINE__, __VA_ARGS__ )
#define log_warn( ... ) _log_printf("WARN", __FILE__, __LINE__, __VA_ARGS__ )

#ifdef DEBUG
	#define log_debug( ... ) _log_printf("DEBUG", __FILE__, __LINE__, __VA_ARGS__ )
	#define log_info( ... ) _log_printf("INFO", __FILE__, __LINE__, __VA_ARGS__ )

	#define CALC_TIME_DECLARE struct timeval _tv_begin_;struct timeval _tv_end_;
	#define CALC_TIME_BEGIN gettimeofday(&_tv_begin_, NULL);
	#define CALC_TIME_END(what) gettimeofday(&_tv_end_, NULL); \
		unsigned long long _tv_lag_ = (_tv_end_.tv_sec-_tv_begin_.tv_sec)*1000 + (_tv_end_.tv_usec-_tv_begin_.tv_usec)/1000; \
		log_debug("%s cost %llu milliseconds", what, _tv_lag_);

#elif defined INFO
	#define log_debug( ... )
	#define log_info( ... ) _log_printf("INFO", __FILE__, __LINE__, __VA_ARGS__ )
	#define CALC_TIME_DECLARE
	#define CALC_TIME_BEGIN 
	#define CALC_TIME_END(what) 

#else
	#define log_debug( ... )
	#define log_info( ... )
	#define CALC_TIME_DECLARE
	#define CALC_TIME_BEGIN 
	#define CALC_TIME_END(what) 
	
#endif




#endif

