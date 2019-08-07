#ifndef PROGRESS_H
#define PROGRESS_H

struct gfs2_progress_bar {
	uint64_t max;
	int max_digits;
	int skip_progress;
};

extern void gfs2_progress_init(struct gfs2_progress_bar *progress, uint64_t max, const char *message, int quiet);
extern void gfs2_progress_update(struct gfs2_progress_bar *progress, uint64_t value);
extern void gfs2_progress_close(struct gfs2_progress_bar *progress, const char *message);

#endif /* PROGRESS_H */
