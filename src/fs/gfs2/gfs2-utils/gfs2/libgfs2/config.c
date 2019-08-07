#include "libgfs2.h"
#include "config.h"

int cfg_debug = 0;

void lgfs2_set_debug(int enable)
{
	cfg_debug = enable;
}
