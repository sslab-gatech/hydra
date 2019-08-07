#ifndef __JOURNAL_DOT_H__
#define __JOURNAL_DOT_H__

extern void dump_journal(const char *journal, int tblk);
extern uint64_t find_journal_block(const char *journal, uint64_t *j_size);

#endif
