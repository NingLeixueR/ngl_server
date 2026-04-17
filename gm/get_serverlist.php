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
	
	$QueryStr = "select * from db_server;";
	$QueryResult = mysql_query($QueryStr);
	$arrs = array();
	while ($Row = mysql_fetch_array($QueryResult, MYSQL_ASSOC))
	{
		$arrs[$Row['id']] = $Row;
	}
	
	$jsonstr = json_encode($arrs);
	echo $jsonstr;
	$file = './serverlist.txt';
	if (file_exists($file)) 
	{
		exec("rm $file");
	} 
	else 
	{
		echo '文件不存在';
	}
	file_put_contents($file, $jsonstr, FILE_APPEND);
?>