#!/bin/sh

project=novramon
#
masterhost="http://svn.1-loop.net"
masterrepo="novramonrepo"
#
mastersite=${masterhost}/${masterrepo}

svn co ${mastersite}/${project}/trunk ${project}
