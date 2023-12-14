<?php
	require_once 'payapi.php';
	
	$roleid = $_GET['roleid'];
	$orderid = $_GET['orderid'];
	$gm = $_GET['gm'];
	$stat = $_GET['stat'];
	
	update_recharge($orderid, $roleid, $gm, $stat);
?>