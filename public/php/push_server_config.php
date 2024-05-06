<?php
	require_once 'config.php';

	$con = mysql_connect(DB_IP . ":" . DB_PORT, DB_USER, DB_PASS);
	if(!$con)
	{
		echo 'Can not connect: ' . mysql_error();
		return false;
	}
		
	mysql_select_db(GMSYS, $con);
	mysql_query("set names 'utf8'");
	
	$id = $_GET['id'];
	$name = $_GET['name'];
	$area = $_GET['area'];
	$ip = $_GET['ip'];
	$nip = $_GET['nip'];
	$type = $_GET['type'];
	$port = $_GET['port'];
	
	$QueryInster = "INSERT INTO db_server(id,area,name,ip,nip,port,type) VALUES ({$id}, {$area}, '{$name}', '{$ip}', '{$nip}', {$port}, {$type})";
	$QueryInster = $QueryInster." ON DUPLICATE KEY UPDATE area=values(area),name=values(name),ip=values(ip),nip=values(nip),port=values(port),type=values(type);";
	echo $QueryInster."<br/>";
	if(mysql_query($QueryInster))
	{
		echo $QueryInster." success";
		return true;
	}
	else
	{
		echo $QueryInster." fail";
		return false;
	}
?>