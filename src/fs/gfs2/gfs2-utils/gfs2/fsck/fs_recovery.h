#ifndef __FS_RECOVERY_H__
#define __FS_RECOVERY_H__

#include "libgfs2.h"

extern int replay_journals(struct gfs2_sbd *sdp, int preen, int force_check,
			   int *clean_journals);
extern int preen_is_safe(struct gfs2_sbd *sdp, int preen, int force_check);

extern int ji_update(struct gfs2_sbd *sdp);
extern int init_jindex(struct gfs2_sbd *sdp, int allow_ji_rebuild);
#endif /* __FS_RECOVERY_H__ */

