#include <check.h>
#include <libgfs2.h>
#include <rgrp.h> /* Private header libgfs2/rgrp.h for convenience */

// TODO: Remove this when the extern is removed from libgfs2
void print_it(const char *label, const char *fmt, const char *fmt2, ...) {}

static lgfs2_rgrps_t mockup_rgrp(void)
{
	struct gfs2_sbd *sdp;
	lgfs2_rgrps_t rgs;
	unsigned i;
	uint64_t addr;
	struct gfs2_rindex ri = {0};
	lgfs2_rgrp_t rg;
	uint32_t rgsize = (1024 << 20) / 4096;

	sdp = calloc(1, sizeof(*sdp));
	fail_unless(sdp != NULL);

	sdp->device.length = rgsize + 20;
	sdp->device_fd = -1;
	sdp->bsize = sdp->sd_sb.sb_bsize = 4096;
	compute_constants(sdp);

	rgs = lgfs2_rgrps_init(sdp, 0, 0);
	fail_unless(rgs != NULL);

	lgfs2_rgrps_plan(rgs, sdp->device.length - 16, rgsize);

	addr = lgfs2_rindex_entry_new(rgs, &ri, 16, rgsize);
	ck_assert(addr != 0);

	rg = lgfs2_rgrps_append(rgs, &ri, 0);
	fail_unless(rg != NULL);

	for (i = 0; i < rg->ri.ri_length; i++) {
		rg->bits[i].bi_bh = bget(sdp, rg->ri.ri_addr + i);
		fail_unless(rg->bits[i].bi_bh != NULL);
	}
	return rgs;
}

START_TEST(test_mockup_rgrp)
{
	lgfs2_rgrps_t rgs = mockup_rgrp();
	fail_unless(rgs != NULL);
}
END_TEST

START_TEST(test_rbm_find_good)
{
	uint32_t minext;
	struct lgfs2_rbm rbm = {0};
	lgfs2_rgrps_t rgs = mockup_rgrp();
	rbm.rgd = lgfs2_rgrp_first(rgs);

	/* Check that extent sizes up to the whole rg can be found */
	for (minext = 1; minext <= rbm.rgd->ri.ri_data; minext++) {
		int err;
		uint64_t addr;

		rbm.offset = rbm.bii = 0;

		err = lgfs2_rbm_find(&rbm, GFS2_BLKST_FREE, &minext);
		ck_assert_int_eq(err, 0);

		addr = lgfs2_rbm_to_block(&rbm);
		fail_unless(addr == rbm.rgd->ri.ri_data0);
	}
}
END_TEST

START_TEST(test_rbm_find_bad)
{
	int err;
	uint32_t minext;
	struct lgfs2_rbm rbm = {0};
	lgfs2_rgrps_t rgs = mockup_rgrp();

	rbm.rgd = lgfs2_rgrp_first(rgs);
	minext = rbm.rgd->ri.ri_data + 1;

	err = lgfs2_rbm_find(&rbm, GFS2_BLKST_FREE, &minext);
	ck_assert_int_eq(err, 1);
}
END_TEST

START_TEST(test_rbm_find_lastblock)
{
	int err;
	unsigned i;
	uint64_t addr;
	uint32_t minext = 1; /* Only looking for one block */
	struct lgfs2_rbm rbm = {0};
	lgfs2_rgrp_t rg;
	lgfs2_rgrps_t rgs = mockup_rgrp();

	rbm.rgd = rg = lgfs2_rgrp_first(rgs);

	/* Flag all blocks as allocated... */
	for (i = 0; i < rg->ri.ri_length; i++)
		memset(rg->bits[i].bi_bh->b_data, 0xff, rgs->sdp->bsize);

	/* ...except the final one */
	err = gfs2_set_bitmap(rg, rg->ri.ri_data0 + rg->ri.ri_data - 1, GFS2_BLKST_FREE);
	ck_assert_int_eq(err, 0);

	err = lgfs2_rbm_find(&rbm, GFS2_BLKST_FREE, &minext);
	ck_assert_int_eq(err, 0);

	addr = lgfs2_rbm_to_block(&rbm);
	fail_unless(addr == (rg->ri.ri_data0 + rg->ri.ri_data - 1));
}
END_TEST

static Suite * libgfs2_suite(void)
{

	Suite *s = suite_create("libgfs2");

	TCase *tc_rgrp = tcase_create("rgrp");

	tcase_add_test(tc_rgrp, test_mockup_rgrp);
	tcase_add_test(tc_rgrp, test_rbm_find_good);
	tcase_add_test(tc_rgrp, test_rbm_find_bad);
	tcase_add_test(tc_rgrp, test_rbm_find_lastblock);
	tcase_set_timeout(tc_rgrp, 0);
	suite_add_tcase(s, tc_rgrp);

	return s;
}

int main(void)
{
	int failures;
	Suite *s = libgfs2_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	failures = srunner_ntests_failed(sr);
	srunner_free(sr);
	return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
