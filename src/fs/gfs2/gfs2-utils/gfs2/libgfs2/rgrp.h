#ifndef __RGRP_DOT_H__
#define __RGRP_DOT_H__

#include "libgfs2.h"

struct rgplan {
	uint32_t num;
	uint32_t len;
};

/**
 * This structure is defined in libgfs2.h as an opaque type. It stores the
 * constants and context required for creating resource groups from any point
 * in an application.
 */
struct _lgfs2_rgrps {
	struct osi_root root;
	struct rgplan plan[2];
	struct gfs2_sbd *sdp;
	unsigned long align;
	unsigned long align_off;
};

struct lgfs2_rbm {
	lgfs2_rgrp_t rgd;
	uint32_t offset;    /* The offset is bitmap relative */
	unsigned bii;       /* Bitmap index */
};

static inline struct gfs2_bitmap *rbm_bi(const struct lgfs2_rbm *rbm)
{
	return rbm->rgd->bits + rbm->bii;
}

static inline uint64_t lgfs2_rbm_to_block(const struct lgfs2_rbm *rbm)
{
	return rbm->rgd->ri.ri_data0 + (rbm_bi(rbm)->bi_start * GFS2_NBBY) +
	        rbm->offset;
}

static inline int lgfs2_rbm_eq(const struct lgfs2_rbm *rbm1, const struct lgfs2_rbm *rbm2)
{
	return (rbm1->rgd == rbm2->rgd) && (rbm1->bii == rbm2->bii) &&
	        (rbm1->offset == rbm2->offset);
}

extern int lgfs2_rbm_from_block(struct lgfs2_rbm *rbm, uint64_t block);
extern int lgfs2_rbm_find(struct lgfs2_rbm *rbm, uint8_t state, uint32_t *minext);
extern unsigned lgfs2_alloc_extent(const struct lgfs2_rbm *rbm, int state, const unsigned elen);

#endif /* __RGRP_DOT_H__ */
