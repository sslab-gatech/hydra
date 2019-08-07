/**
 * Progress bar to give updates for operations in gfs2-utils.
 * Adapted from the simple progress bar in e2fsprogs progress.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>

#include "progress.h"

static char spaces[44], backspaces[44];
static time_t last_update;

static int number_of_digits(int value)
{
	int digits = 0;

	do {
		value /= 10;
		digits++;
	} while (value != 0);

	return digits;
}

void gfs2_progress_init(struct gfs2_progress_bar *progress, uint64_t max, const char *message, int quiet)
{
	/**
	 * NOTE:
	 *
	 * Default operation is to output the progress indication
	 * in full. Although we will honor the quiet flag in the
	 * application, if this is set we skip progress bar any
	 * update operations and output.
	 *
	 */

	memset(spaces, ' ', sizeof(spaces)-1);
	spaces[sizeof(spaces)-1] = 0;

	memset(backspaces, '\b', sizeof(backspaces)-1);
	backspaces[sizeof(backspaces)-1] = 0;

	memset(progress, 0, sizeof(*progress));

	if (quiet) {
		progress->skip_progress++;
		return;
	}

	progress->max = max;
	progress->max_digits = number_of_digits(max);

	if (message) {
		fputs(message, stdout);
		fflush(stdout);
	}
	last_update = 0;
}

void gfs2_progress_update(struct gfs2_progress_bar *progress, uint64_t value)
{
	time_t current_time;

	if (progress->skip_progress || (!isatty(STDOUT_FILENO)))
		return;

	current_time = time(0);
	if (current_time == last_update)
		return;
	last_update = current_time;

	printf("[%*"PRIu64"/%*"PRIu64"]", progress->max_digits, value,
		progress->max_digits, progress->max);
	fflush(stdout);
	fprintf(stdout, "%.*s", (2 * progress->max_digits) + 3, backspaces);
}

void gfs2_progress_close(struct gfs2_progress_bar *progress, const char *message)
{
	if (progress->skip_progress)
		return;

	if (isatty(STDOUT_FILENO)) {
		fprintf(stdout, "%.*s", (2 * progress->max_digits) + 3, spaces);
		fprintf(stdout, "%.*s", (2 * progress->max_digits) + 3, backspaces);
	}

	if (message)
		fputs(message, stdout);
}
