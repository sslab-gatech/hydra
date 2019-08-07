%define reiserfsprogsversion 3.6.23

Vendor:       Hans Reiser
Distribution: Hans Reiser
Name:         reiserfsprogs
Release:      1
Copyright:    2004 Hans Reiser
Group:        Unsorted

Packager:     anthon@mnt.org

Version:      %{reiserfsprogsversion}
Summary:      utilities belonging to the Reiser filesystem
Source:       reiserfsprogs-%{reiserfsprogsversion}.tar.gz
BuildRoot:    %{_tmppath}/rpmbuildroot-%{name}
%description

The reiserfsprogs package contains programs for creating (mkreiserfs),
checking and correcting any inconsistencies (reiserfsck) and resizing
(resize_reiserfs) of a reiserfs filesystem.

Authors:
--------
Hans Reiser <reiser@namesys.com>
Vitaly Fertman <vitaly@namesys.com>
Alexander Zarochentcev <zam@namesys.com>
Vladimir Saveliev <monstr@namesys.com>

%prep
# Jeff Johnson states that RPM doesn't delete obviouse crap like / if 
# buildroot is set differently since RPM 3.0.4 (rpm mailing list 2001-04-10)
    rm -rf $RPM_BUILD_ROOT
%setup -q -n reiserfsprogs-%{reiserfsprogsversion}
# %patch
%build
  MANDIR=$(dirname $(dirname $(man -w fsck | cut -d ' ' -f 1)))
  ./configure --prefix="" --mandir=$MANDIR
  %{__make} all
%install
  mkdir -p $RPM_BUILD_ROOT/sbin
  %{__make} DESTDIR=$RPM_BUILD_ROOT install
# do we need this?
  cd $RPM_BUILD_ROOT/sbin
  %{__ln_s} -f reiserfsck fsck.reiserfs
  %{__ln_s} -f mkreiserfs mkfs.reiserfs

# __os_install_post is normally executed after \%install disable it
%define ___build_post %{nil} 
# explicitly call it now, so manpages get compressed, exec's stripped etc.
%{?__os_install_post}
%define __os_install_post %{nil}
# now we have all the files execpt for docs, but their owner is unimportant
cd $RPM_BUILD_ROOT

%{__rm} -f rpm-filelist
# we do not have special directories to make
#find . -type d \
# | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' >> rpm-filelist
find . -type f \
 | sed 's,^\.,\%attr(-\,root\,root) ,' | fgrep -v rpm-filelist >> rpm-filelist
find . -type l \
 | sed 's,^\.,\%attr(-\,root\,root) ,' >> rpm-filelist

%post
CONFIG=/usr/src/linux/.config

if [ -f $CONFIG ] ; then
  source $CONFIG
fi

if [ -z $CONFIG_REISERFS_FS ] ; then
  echo -e "\nIn $CONFIG , you probably have to set:"
  if [ "$CONFIG_EXPERIMENTAL" != "y" ] ; then
    echo -e 'CONFIG_EXPERIMENTAL=y'
  fi
  echo -e 'CONFIG_REISERFS_FS=y\n  or'
  echo -e 'CONFIG_REISERFS_FS=m'
  echo -e 'and recompile and reboot your kernel if you cannot use the\nreiserfsprogs utilities'
fi

%clean
  %{__rm} -rf $RPM_BUILD_ROOT
%files -f %{buildroot}/rpm-filelist
%defattr(-,root,root)
%doc README
