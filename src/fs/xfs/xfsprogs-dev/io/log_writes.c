/*
 * Copyright (c) 2017 Intel Corporation.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "platform_defs.h"
#include <libdevmapper.h>
#include "command.h"
#include "init.h"

static cmdinfo_t log_writes_cmd;

static int
mark_log(const char *device, const char *mark)
{
	struct dm_task 	*dmt;
	const int 	size = 80;
	char 		message[size];
	int 		len, ret = 0;

	len = snprintf(message, size, "mark %s", mark);
	if (len >= size) {
		printf("mark '%s' is too long\n", mark);
		return ret;
	}

	if (!(dmt = dm_task_create(DM_DEVICE_TARGET_MSG)))
		return ret;

	if (!dm_task_set_name(dmt, device))
		goto out;

	if (!dm_task_set_sector(dmt, 0))
		goto out;

	if (!dm_task_set_message(dmt, message))
		goto out;

	if (dm_task_run(dmt))
		ret = 1;
out:
	dm_task_destroy(dmt);
	return ret;
}

static int
log_writes_f(
	int			argc,
	char			**argv)
{
	const char 	*device = NULL;
	const char 	*mark = NULL;
	int 		c;

	exitcode = 1;
	while ((c = getopt(argc, argv, "d:m:")) != EOF) {
		switch (c) {
		case 'd':
			device = optarg;
			break;
		case 'm':
			mark = optarg;
			break;
		default:
			return command_usage(&log_writes_cmd);
		}
	}

	if (device == NULL || mark == NULL)
		return command_usage(&log_writes_cmd);

	if (mark_log(device, mark))
		exitcode = 0;

	return 0;
}

void
log_writes_init(void)
{
	log_writes_cmd.name = "log_writes";
	log_writes_cmd.altname = "lw";
	log_writes_cmd.cfunc = log_writes_f;
	log_writes_cmd.flags = CMD_NOMAP_OK | CMD_NOFILE_OK | CMD_FOREIGN_OK
				| CMD_FLAG_ONESHOT;
	log_writes_cmd.argmin = 0;
	log_writes_cmd.argmax = -1;
	log_writes_cmd.args = _("-d device -m mark");
	log_writes_cmd.oneline =
		_("create mark <mark> in the dm-log-writes log specified by <device>");

	add_command(&log_writes_cmd);
}
