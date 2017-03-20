#!/bin/bash
pid=`ps aux | grep group_server.exe | grep -v grep | awk '{print $2}'`
if [ -z $pid ] ; then
	echo "group_server not existed";
	exit;
else 
	echo -n "killing group_server  $pid..."
	kill -INT $pid
	echo "done"
fi

