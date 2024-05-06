<?php
	require_once 'payapi.php';
	
	$roleid = $_GET['roleid'];
	$orderid = $_GET['orderid'];
	$gm = $_GET['gm'];
	$stat = $_GET['stat'];
	
	
	$controller = new payapi();
	$controller->update_recharge($orderid, $roleid, $gm, $stat);
?>