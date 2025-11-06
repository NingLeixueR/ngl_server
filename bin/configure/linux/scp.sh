declare -A map
filename="scp.txt" # 指定要读取的文件名
while IFS= read -r line; do
	array=($line)
	map["${array[0]}"]="${array[2]}"
done < "$filename"

sshpass -p${map["password"]} scp -P${map["port"]} ./ngl_server_*.tar.gz ${map["usr"]}@${map["ip"]}:${map["dir"]}
