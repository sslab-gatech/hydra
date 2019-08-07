#ifndef _PROGBAR_H_
#define _PROGBAR_H_

#include <time.h>
#include <stdio.h>

enum {
	E2F_FLAG_PROG_SUPPRESS = 1,
	E2F_FLAG_PROG_BAR = 2,
};

struct progbar {
	char units[16];
	int progress_pos;
	int progress_last_percent;
	time_t progress_last_time;
	int flags;
	FILE *file;
};

struct spinner {
	int count;
	FILE *file;
};

void progbar_init(struct progbar *ctx, const char *units, FILE * fp);
void progbar_clear(struct progbar *ctx);
int progbar_update(struct progbar *ctx, const char *label, int curr, int max,
		   unsigned int dpynum);

void spinner_init(struct spinner *spinner, FILE * fp);
void spinner_touch(struct spinner *spinner);
void spinner_clear(struct spinner *spinner);

#endif /* _PROGBAR_H_ */
