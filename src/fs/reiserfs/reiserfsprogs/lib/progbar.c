#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

#include "progbar.h"

static char bar[128], spaces[128];
static const char spinner[] = "\\|/-";

void progbar_init(struct progbar *ctx, const char *units, FILE * fp)
{
	memset(ctx, 0, sizeof(*ctx));
	if (!bar[0])
		memset(bar, '=', sizeof(bar) - 1);
	if (!spaces[0])
		memset(spaces, ' ', sizeof(spaces) - 1);
	strncpy(ctx->units, units, sizeof(ctx->units));
	ctx->file = fp;
}

void progbar_clear(struct progbar *ctx)
{
	if (!(ctx->flags & E2F_FLAG_PROG_BAR))
		return;

	fprintf(ctx->file, "\r%*s\r", 80, " ");
	fflush(ctx->file);
	ctx->flags &= ~E2F_FLAG_PROG_BAR;
}

int progbar_update(struct progbar *ctx, const char *label, int curr, int max,
		   unsigned int dpynum)
{
	int i;
	unsigned int tick;
	struct timeval tv;
	int dpywidth;
	int fixed_percent;
	assert (curr >= 0);
	assert (max > 0);
	float percent = ((float)curr) / ((float)max) * 100;

	if (ctx->flags & E2F_FLAG_PROG_SUPPRESS)
		return 0;

	/*
	 * Calculate the new progress position.  If the
	 * percentage hasn't changed, then we skip out right
	 * away.
	 */
	fixed_percent = (int)((10 * percent) + 0.5);
	if (ctx->progress_last_percent == fixed_percent)
		return 0;
	ctx->progress_last_percent = fixed_percent;

	/*
	 * If we've already updated the spinner once within
	 * the last 1/8th of a second, no point doing it
	 * again.
	 */
	gettimeofday(&tv, NULL);
	tick = (tv.tv_sec << 3) + (tv.tv_usec / (1000000 / 8));
	if ((tick == ctx->progress_last_time) &&
	    (fixed_percent != 0) && (fixed_percent != 1000))
		return 0;
	ctx->progress_last_time = tick;

	/*
	 * Advance the spinner, and note that the progress bar
	 * will be on the screen
	 */
	ctx->progress_pos = (ctx->progress_pos + 1) & 3;
	ctx->flags |= E2F_FLAG_PROG_BAR;

	dpywidth = 66 - strlen(label);
	dpywidth = 8 * (dpywidth / 8);
	if (dpynum)
		dpywidth -= 8;

	i = ((percent * dpywidth) + 50) / 100;
	fprintf(ctx->file, "\r%s: |%s%s", label,
		bar + (sizeof(bar) - (i + 1)),
		spaces + (sizeof(spaces) - (dpywidth - i + 1)));
	if (fixed_percent == 1000)
		fputc('|', ctx->file);
	else
		fputc(spinner[ctx->progress_pos & 3], ctx->file);
	fprintf(ctx->file, " %4.1f%%  ", percent);
	if (dpynum)
		fprintf(ctx->file, "%u%s\r", dpynum, ctx->units);
	else
		fputs(" \r", ctx->file);

	if (fixed_percent == 1000)
		progbar_clear(ctx);
	fflush(ctx->file);

	return 0;
}

void spinner_init(struct spinner *ctx, FILE * fp)
{
	memset(ctx, 0, sizeof(*ctx));
	ctx->file = fp;
}

void spinner_touch(struct spinner *ctx)
{
	fprintf(ctx->file, "%c", spinner[ctx->count++ % 4]);
}

void spinner_clear(struct spinner *ctx)
{
	fputs("", ctx->file);
}
