#!/bin/sh
dev=$1
i=0
gfs2_edit -p rg 0 $dev | grep rg_data0 > /dev/null 2>&1
# New fields not present in /usr/include/linux/gfs2_ondisk.h
test $? = 0 || exit 0
gfs2_edit -p rindex $dev | while read field rival unused
do
	test $field = ri_data0 -o $field = ri_data -o $field = ri_bitbytes || continue
	rgfield=$(echo $field | sed 's/ri/rg/')
	rgval=$(gfs2_edit -p rg $i $dev | grep " $rgfield " | awk '{print $2}')

	if test "$rival" != "$rgval"
	then
		echo "Bad $rgfield in rg $i: $rgval (expected: $rival)" >&2
		exit 1
	fi

	test $field = ri_bitbytes && i=$((i+1))
done
