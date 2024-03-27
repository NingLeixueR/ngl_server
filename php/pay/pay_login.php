<?php
	require_once '../config.php';
	
	$roleid = $_GET['roleid'];
	
	$con = mysql_connect(DB_IP . ":" . DB_PORT, DB_USER, DB_PASS);
	if(!$con)
	{
		$Result['ret'] = 1;
		$Result['msg'] = 'Can not connect: ' . mysql_error();
		echo json_encode($Result);
		return;
	}
		
	mysql_select_db(GMSYS, $con);
	mysql_query("set names 'utf8'");
		
	$QuerySelect = "select * from db_recharge where roleid = {$roleid} AND stat!=1;";
	$Result = mysql_query($QuerySelect);
	if ($Row = mysql_fetch_array($Result, MYSQL_ASSOC))
	{
		echo json_encode($Row);
		return;
	}
?>