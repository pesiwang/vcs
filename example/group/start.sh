#!/bin/sh

nohup ./build/group_server.exe ./etc/config.json >> ./log/group_server.log 2>&1 &

