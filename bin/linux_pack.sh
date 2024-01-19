#!/bin/sh
rm -rf ngl*
mkdir ngl -p
mkdir ngl/tools -p
mkdir ngl/config -p
mkdir ngl/csv -p
mkdir ngl/lib64 -p
mkdir ngl/log
cp /usr/local/lib64/libstdc++.so ngl/lib64
cp -r Debug/config/* ngl/config
cp -r Debug/csv/* ngl/csv
cp node ngl
rm -rf *.gz
current_time=$(date +"%Y%m%d_%H%M%S")
echo $current_time
tar -zcvf ngl_server_$current_time.tar.gz