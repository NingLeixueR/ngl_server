#!/bin/bash
path="$(cd $(dirname $0); pwd)"
pids=`ps -elf|grep node|grep -v grep|awk '{print $4}'`
function stop_pid() {
    for pid in ${pids}
        do
        proname=`ls -l /proc/${pid}|grep exe|awk '{print $11}'`
        if [[ ${proname} == ${path}/node ]];then
            kill -9 ${pid}
            echo "kill -9 ${pid}";

        fi
        done
    echo "Stop Server PID is Success "
}

stop_pid
