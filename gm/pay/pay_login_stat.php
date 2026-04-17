<?php
	require_once '../config.php';
	
	$roleid = $_GET['roleid'];
	$orderid = $_GET['orderid'];
	$rechargeid = $_GET['rechargeid'];
	
	$con = mysql_connect(DB_IP . ":" . DB_PORT, DB_USER, DB_PASS);
	if(!$con)
	{
		return;
	}
		
	mysql_select_db(GMSYS, $con);
	mysql_query("set names 'utf8'");
	
	$ret = array(
		'stat' => 1,
		'orderid' => $orderid,
		'rechargeid' => $rechargeid,
	);
	$QuerySelect = "select * from db_recharge where roleid = {$roleid} AND stat!=1 AND orderid = {$orderid} AND rechargeid = {$rechargeid};";
	$Result = mysql_query($QuerySelect);
	
	if ($Row = mysql_fetch_array($Result, MYSQL_ASSOC))
	{
		$QuerySelect = "UPDATE db_recharge SET stat = 1 WHERE roleid = {$roleid} AND stat!=1 AND orderid = {$orderid} AND rechargeid = {$rechargeid};";
		mysql_query($QuerySelect);
		$ret['stat'] = 0;
		echo json_encode($ret);
		return;
	}
	echo json_encode($ret);
?>