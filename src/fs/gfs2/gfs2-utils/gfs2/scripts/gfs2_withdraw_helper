#!/bin/sh
#
# Do not run this script manually. This script is called by udev on a gfs2
# withdraw uevent and is used to complete the withdraw action and notify the
# kernel.
#

# Sanity checks
if [ "$SUBSYSTEM" != "gfs2" ] || [ "$LOCKPROTO" != "lock_dlm" ] ||
       [ -z "$DEVPATH" ] || [ "$ACTION" != "offline" ]
then
    exit 1 # Nothing to do here
fi

# Try and suspend the device
SYSFS_TOPDIR="/sys"$DEVPATH
DM_NAME=$(cat "$SYSFS_TOPDIR/device/dm/name")
DM_DEV="/dev/mapper/"$DM_NAME

if [ -z "$DM_DEV" ]
then
    /usr/bin/dmsetup suspend $DM_DEV
fi

# Signal completion of withdraw
WD_ACK="$SYSFS_TOPDIR/lock_module/withdraw"
if [ -f "$WD_ACK" ]
then
    echo "1" > $WD_ACK
fi
