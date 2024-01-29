cd $(dirname $0)
declare -A map
filename="config.txt" # 指定要读取的文件名
while IFS= read -r line; do
	array=($line)
	map["${array[0]}"]="${array[2]}"
done < "$filename"

echo "#######################"

for key in "${!map[@]}"; do
  echo "Key: $key, Value: ${map[$key]}"
done

`pwd`/node robot ${map["area"]} 1 ${map["robotname"]} ${map["robotbeg"]} ${map["robotend"]}
