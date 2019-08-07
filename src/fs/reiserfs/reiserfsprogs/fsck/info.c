/*
 * Copyright 1996-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#include "fsck.h"

int fsck_user_confirmed(reiserfs_filsys_t fs, char *q, char *a,
			int default_answer)
{
	if (!fsck_interactive(fs))
		return default_answer;

	return user_confirmed(fsck_progress_file(fs), q, a);
}

void stage_report(int pass, reiserfs_filsys_t fs)
{
	switch (pass) {
	case 0:
		{
			struct pass0_stat *stat;

			/* what has been done on pass 0 */
			stat = pass_0_stat(fs);
			fsck_progress
			    ("\tRead blocks (but not data blocks) %lu\n",
			     stat->dealt_with);
			fsck_progress("\t\tLeaves among those %lu\n",
				      stat->leaves);
			if (stat->leaves_corrected)
				fsck_progress("\t\t\t- corrected leaves %lu\n",
					      stat->leaves_corrected);
			if (stat->all_contents_removed)
				fsck_progress
				    ("\t\t\t- leaves all contents of which could not be saved and deleted %lu\n",
				     stat->all_contents_removed);
			if (stat->too_old_leaves)
				fsck_progress
				    ("\t\t\t- too old leaves %lu (skipped)\n",
				     stat->too_old_leaves);
			if (stat->wrong_pointers)
				fsck_progress
				    ("\t\tpointers in indirect items to wrong area %lu (zeroed)\n",
				     stat->wrong_pointers);
			fsck_progress("\t\tObjectids found %lu\n",
				      proper_id_map(fs)->count);
		}
		break;

	case 1:
		{
			struct pass1_stat *stat;

			/* what has been done on pass 1 */
			stat = pass_1_stat(fs);
			fsck_progress("\t%lu leaves read\n", stat->leaves);
			fsck_progress("\t\t%lu inserted\n",
				      stat->inserted_leaves);
			if (stat->pointed_leaves)
				fsck_progress
				    ("\t\t\t- pointers in indirect items pointing to metadata %lu (zeroed)\n",
				     stat->pointed_leaves);
			if (stat->saved_items)
				fsck_progress("\tSaved %lu items\n",
					      stat->saved_items);
			if (stat->uninsertable_leaves)
				fsck_progress("\t\t%lu not inserted\n",
					      stat->uninsertable_leaves);
			if (stat->non_unique_pointers)
				fsck_progress
				    ("\tnon-unique pointers in indirect items (zeroed) %lu\n",
				     stat->non_unique_pointers);
		}
		break;

	case 2:
		{
			struct pass2_stat *stat;

			/* what has been done on pass 2 */
			stat = pass_2_stat(fs);
			if (stat->leaves)
				fsck_progress
				    ("\tLeaves inserted item by item %lu\n",
				     stat->leaves);
			if (stat->safe_non_unique_pointers)
				fsck_progress("\t%lu \n",
					      stat->safe_non_unique_pointers);
			if (stat->shared_objectids)
				fsck_progress("\t%lu shared objectids\n",
					      stat->shared_objectids);
			if (stat->relocated)
				fsck_progress
				    ("\tFiles relocated because of key conflicts with a directory %lu\n",
				     stat->relocated);
			if (stat->rewritten)
				fsck_progress("\tFiles rewritten %lu\n",
					      stat->rewritten);
		}
		break;

	case 3:		/* semantic */
		{
			struct semantic_stat *stat;

			/* what has been done on the semantic pass */
			stat = sem_pass_stat(fs);
			fsck_progress("\tFiles found: %ld\n",
				      stat->regular_files);
			fsck_progress("\tDirectories found: %ld\n",
				      stat->directories);
			if (stat->symlinks)
				fsck_progress("\tSymlinks found: %ld\n",
					      stat->symlinks);
			if (stat->others)
				fsck_progress("\tOthers: %ld\n", stat->others);
			if (stat->broken_files)
				fsck_progress
				    ("\tBroken (of files/symlinks/others): %ld\n",
				     stat->broken_files);
			if (stat->fixed_sizes)
				fsck_progress("\tFiles with fixed size: %ld\n",
					      stat->fixed_sizes);
			if (stat->deleted_entries)
				fsck_progress
				    ("\tNames pointing to nowhere (removed): %ld\n",
				     stat->deleted_entries);
			if (stat->oid_sharing)
				fsck_progress
				    ("\tObjects having used objectids: %lu\n",
				     stat->oid_sharing);
			if (stat->oid_sharing_files_relocated)
				fsck_progress("\t\tfiles fixed %lu\n",
					      stat->
					      oid_sharing_files_relocated);
			if (stat->oid_sharing_dirs_relocated)
				fsck_progress("\t\tdirs fixed %lu\n",
					      stat->oid_sharing_dirs_relocated);
		}
		break;

	case 0x3a:		/* lost+found pass */
		{
			struct lost_found_stat *stat;

			/* what has been done on the semantic pass */
			stat = lost_found_pass_stat(fs);
			if (stat->lost_found)
				fsck_progress("\tObjects without names %lu\n",
					      stat->lost_found);
			if (stat->empty_lost_dirs)
				fsck_progress("\tEmpty lost dirs removed %lu\n",
					      stat->empty_lost_dirs);
			if (stat->lost_found_dirs)
				fsck_progress
				    ("\tDirs linked to /lost+found: %lu\n",
				     stat->lost_found_dirs);
			if (stat->dir_recovered)
				fsck_progress
				    ("\t\tDirs without stat data found %lu\n",
				     stat->dir_recovered);

			if (stat->lost_found_files)
				fsck_progress
				    ("\tFiles linked to /lost+found %lu\n",
				     stat->lost_found_files);
			if (stat->oid_sharing)
				fsck_progress
				    ("\tObjects having used objectids: %lu\n",
				     stat->oid_sharing);
			if (stat->oid_sharing_files_relocated)
				fsck_progress("\t\tfiles fixed %lu\n",
					      stat->
					      oid_sharing_files_relocated);
			if (stat->oid_sharing_dirs_relocated)
				fsck_progress("\t\tdirs fixed %lu\n",
					      stat->oid_sharing_dirs_relocated);
		}
		break;

	case 4:		/* removing of unreachable */
		{
			struct pass_4_stat *stat;

			/* what has been done on the semantic pass */
			stat = pass_4_stat(fs);
			if (stat->deleted_items)
				fsck_progress
				    ("\tDeleted unreachable items %lu\n",
				     stat->deleted_items);
		}
		break;

	case 5:
		fsck_progress("There are on the filesystem:\n"
			      "\tLeaves %lu\n\tInternal nodes %lu\n"
			      "\tDirectories %lu\n\tOther files %lu\n"
			      "\tData block pointers %lu (%lu of them are zero)\n"
			      "\tSafe links %lu\n",
			      fsck_check_stat(fs)->leaves,
			      fsck_check_stat(fs)->internals,
			      fsck_check_stat(fs)->dirs,
			      fsck_check_stat(fs)->files,
			      fsck_check_stat(fs)->unfm_pointers,
			      fsck_check_stat(fs)->zero_unfm_pointers,
			      fsck_check_stat(fs)->safe);

	}			/* switch */

	memset(&fsck_data(fs)->rebuild.pass_u, 0,
	       sizeof(fsck_data(fs)->rebuild.pass_u));

}
