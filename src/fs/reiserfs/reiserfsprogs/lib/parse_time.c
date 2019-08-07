#define _XOPEN_SOURCE
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "parse_time.h"
#include "misc.h"
#include "reiserfs_lib.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

time_t parse_time(char *str)
{
	struct tm ts;

	if (strcmp(str, "now") == 0) {
		return (time(NULL));
	}
	memset(&ts, 0, sizeof(ts));
#ifdef HAVE_STRPTIME
	strptime(str, "%Y%m%d%H%M%S", &ts);
#else
	sscanf(str, "%4d%2d%2d%2d%2d%2d", &ts.tm_year, &ts.tm_mon,
	       &ts.tm_mday, &ts.tm_hour, &ts.tm_min, &ts.tm_sec);
	ts.tm_year -= 1900;
	ts.tm_mon -= 1;
	if (ts.tm_year < 0 || ts.tm_mon < 0 || ts.tm_mon > 11 ||
	    ts.tm_mday < 0 || ts.tm_mday > 31 || ts.tm_hour > 23 ||
	    ts.tm_min > 59 || ts.tm_sec > 61)
		ts.tm_mday = 0;
#endif
	if (ts.tm_mday == 0)
		reiserfs_warning(stderr, "Couldn't parse date/time "
				 "specifier: %s", str);
	return (mktime(&ts));
}
