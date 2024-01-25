#!/bin/sh
rm -rf ngl*
mkdir ngl -p
mkdir ngl/tools -p
mkdir ngl/config -p
mkdir ngl/csv -p
mkdir ngl/lib64 -p
mkdir ngl/log	-p
cp linux_start.sh ngl/start
cp linux_start_cross.sh ngl/cross
cp linux_start_initdb.sh ngl/initdb
cp linux_start_robot.sh ngl/robot
cp linux_close.sh ngl/close
chmod 777 ngl/start
chmod 777 ngl/cross
chmod 777 ngl/initdb
chmod 777 ngl/robot
chmod 777 ngl/close
cp config.template.txt ngl/config.template.txt
cp /usr/local/lib64/libstdc++.so.6.0.31 ngl/lib64/libstdc++.so.6
cp /usr/lib64/mysql/libmysqlclient.so.21 ngl/lib64
cp -r ./config/* ngl/config
cp -r ./csv/* ngl/csv
cp node ngl
rm -rf *.gz
current_time=$(date +"%Y%m%d_%H%M%S")
echo $current_time
tar -zcvf ngl_server_$current_time.tar.gz ngl
