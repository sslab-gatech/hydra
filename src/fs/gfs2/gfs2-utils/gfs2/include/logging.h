#ifndef __LOGGING_H__
#define __LOGGING_H__

extern int print_level;
#define increase_verbosity() do { print_level++; } while(0)
#define decrease_verbosity() do { print_level--; } while(0)

#define MSG_DEBUG       7
#define MSG_INFO        6
#define MSG_NOTICE      5
#define MSG_WARN        4
#define MSG_ERROR       3
#define MSG_CRITICAL    2
#define MSG_NULL        1

#define log_debug(format...) \
	do { if (print_level >= MSG_DEBUG) { \
		printf("(%s:%d) ", __FUNCTION__, __LINE__); \
		printf(format); } } while(0)

#define log_info(format...) \
	do { if (print_level >= MSG_INFO) printf(format); } while(0)

#define log_notice(format...) \
	do { if (print_level >= MSG_NOTICE) printf(format); } while(0)

#define log_warn(format...) \
	do { if (print_level >= MSG_WARN) printf(format); } while(0)

#define log_err(format...) \
	do { if (print_level >= MSG_ERROR) fprintf(stderr, format); } while(0)

#define log_crit(format...) \
	do { if (print_level >= MSG_CRITICAL) fprintf(stderr, format); } while(0)

#endif /* __LOGGING_H__ */
