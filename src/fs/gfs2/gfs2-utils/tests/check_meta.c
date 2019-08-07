#include <check.h>
#include "libgfs2.h"

// TODO: Remove this when the extern is removed from libgfs2
void print_it(const char *label, const char *fmt, const char *fmt2, ...) {}

START_TEST(test_lgfs2_meta)
{
	fail_unless(lgfs2_selfcheck() == 0);
}
END_TEST

static Suite * libgfs2_suite(void)
{
	Suite *s = suite_create("libgfs2");

	TCase *tc_meta = tcase_create("Meta");
	tcase_add_test(tc_meta, test_lgfs2_meta);
	suite_add_tcase(s, tc_meta);

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
