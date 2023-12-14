<?php
	require_once '../config.php';
	
	//$orderid = $_GET['orderid'];
	//$rechargeid = $_GET['rechargeid'];
	//$gm = $_GET['gm'];
	//$stat = $_GET['stat'];
	
	//$orderid
	//%05d%010d%010d%010d%02d
	//area(), id(), arechargeid, localtime::gettime(), ++billnoindex
	
	// stat 
	// 0  成功
	// 1  玩家不在线发货失败
	
	function area($orderid)
	{
		return intval(substr($orderid,0,5));
	}
	
	function id($orderid)
	{
		return intval(substr($orderid,5,10));
	}
	
	function rechargeid($orderid)
	{
		return intval(substr($orderid,15,10));
	}
	
	function createtime($orderid)
	{
		return intval(substr($orderid,25,10));
	}
	
	function update_recharge($orderid, $roleid, $gm, $stat)
	{
		$rechargeid = rechargeid($orderid);
		$area = area($orderid);
		$id = id($orderid);
		$createtime = createtime($orderid);
		echo "[{$orderid}]###[area:{$area}][id:{$id}][rechargeid:{$rechargeid}][createtime:{$createtime}]<br/>";
		
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
		
		//$QuerySelect = "select * from db_recharge where orderid = {$orderid};";
		//
		//echo "{$QuerySelect}<br/>";
		//$Result = mysql_query($QuerySelect);
		//if ($Row = mysql_fetch_array($Result, MYSQL_ASSOC))
		//{
		//	//$platform = $Row['platform'];
		//	return;
		//}
		
		$QueryInster = "insert into db_recharge(orderid,roleid,rechargeid,createtime,gm,stat) values ('{$orderid}', {$roleid}, {$rechargeid}, now(), {$gm}, {$stat})";
		$QueryInster = $QueryInster." on duplicate key update gm=values(gm),stat=values(stat);";
		mysql_query($QueryInster);
		echo "{$QueryInster}<br/>";
	}
	
	
?>