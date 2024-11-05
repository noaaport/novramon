#!/bin/sh
#
# $Id: 0be340f7cc9e3a92501fa20c03faa7bb86b467ad $
#
if [ -f /etc/fedora-release ]
then
   flavor=fedoracore
elif [ -f /etc/SuSE-release ]
then
   flavor=opensuse
elif [ -f /etc/redhat-release ]
then
    flavor=redhat
elif [ -f /etc/debian_version ] 
then
   flavor=debian
else
   flavor=unknown
fi

echo $flavor
