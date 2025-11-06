cd $(dirname $0)
declare -A map
filename="config.txt" # 指定要读取的文件名
while IFS= read -r line; do
	array=($line)
	map["${array[0]}"]="${array[2]}"
done < "$filename"

nohup `pwd`/node actor ${map["area"]} 1 > /dev/null 2>&1 &
nohup `pwd`/node game ${map["area"]} 1 > /dev/null 2>&1 &  
nohup `pwd`/node db ${map["area"]} 1 > /dev/null 2>&1 &  
nohup `pwd`/node login ${map["area"]} 1 > /dev/null 2>&1 &  
nohup `pwd`/node gateway ${map["area"]} 1 > /dev/null 2>&1 &  
nohup `pwd`/node world ${map["area"]} 1 > /dev/null 2>&1 &
sleep 3s
ps -ef | grep `pwd`/node
echo 'Done!'