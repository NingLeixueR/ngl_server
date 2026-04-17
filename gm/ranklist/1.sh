#/bin/bash

#此脚本用来更新合服测试服

#更新一个服务器
function updateServer()
{
        if [ "${1}" == "" ]; then
                echo "没有输入服务器后缀"
                return
        fi

        #检查是否有服务器包以及数量
        cd /data
        packnum=$(ls -l | grep qmzg2_*.tar.gz | wc -l)
        if [ ${packnum} -eq 0 ]; then
                echo "没有找到服务器包"
                return
        elif [ ${packnum} -gt 1 ]; then
                echo "有多个服务器包，请删除旧包"
                return
        fi

        #检查目标服务器目录是否存在
        serverdir="/data/merge_"${1}
        if [ ! -e ${serverdir} ] || [ ! -d ${serverdir} ];then
                echo ${serverdir}"文件夹不存在"
                return
        fi

        #关服
        if [ -e ${serverdir}/qmzg2/close ] && [ -f ${serverdir}/qmzg2/close ];then
                cd ${serverdir}/qmzg2/
                ./close
        fi

        #关中心服
        if [ -e ${serverdir}/qmzg2/closeCenterServer ] && [ -f ${serverdir}/qmzg2/closeCenterServer ];then
                cd ${serverdir}/qmzg2/
                ./closeCenterServer
        fi

        #解压服务器包到服务器目录
        cd /data
        tar -zxvf qmzg2.tar.gz -C ${serverdir}

        #修改权限
        chmod -R 777 ${serverdir}

        echo ${serverdir} is update done
}