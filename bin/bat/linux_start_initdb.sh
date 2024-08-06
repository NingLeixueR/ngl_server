declare -A map
filename="config.txt" # 指定要读取的文件名
while IFS= read -r line; do
	array=($line)
	map["${array[0]}"]="${array[2]}"
done < "$filename"

nohup `pwd`/node actor ${map["area"]} 1 > /dev/null 2>&1 &
`pwd`/node db ${map["area"]} 1 init 
sleep 3s
ps -ef | grep `pwd`/node
echo 'Done!'
