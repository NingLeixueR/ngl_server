#!/bin/sh
rm -rf ngl*
rm -rf sql*
mkdir ngl -p
mkdir ngl/tools -p
mkdir ngl/config -p
mkdir ngl/csv -p
mkdir ngl/lib64 -p
mkdir ngl/log -p
mkdir sql -p
cp start.sh ngl/start
cp close.sh ngl/close
chmod 777 ngl/start
chmod 777 ngl/close
cp config.template.txt ngl/config.template.txt
cp /usr/local/lib64/libstdc++.so.6.0.31 ngl/lib64/libstdc++.so.6
cp /usr/lib64/mysql/libmysqlclient.so.21 ngl/lib64
cp -r ./config/* ngl/config
cp -r ./csv/* ngl/csv
cp node ngl
cp *.sql ./sql
rm -rf *.gz
tar -zcvf ngl_server.tar.gz ngl
tar -zcvf sql.tar.gz sql
md5sum sql.tar.gz >ngl.md5
md5sum ngl_server.tar.gz >>ngl.md5

current_time=$(date +"%Y%m%d_%H%M%S")
echo $current_time
tar -czvf ngl_server_$current_time.tar.gz sql.tar.gz ngl_server.tar.gz ngl.md5