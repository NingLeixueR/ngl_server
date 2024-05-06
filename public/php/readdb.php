<?php
	require_once 'config.php';

	$con = mysql_connect(DB_IP . ":" . DB_PORT, DB_USER, DB_PASS);
	if(!$con)
	{
		echo 'Can not connect: ' . mysql_error();
		return false;
	}
		
	$name = $_GET['tab'];
	$dbname = $_GET['dbname'];
	$everynum = intval($_GET['everynum']);
	$page = intval($_GET['page']);
		
	mysql_select_db($dbname, $con);
	mysql_query("set names 'utf8'");
	
	$page -= 1;
	if($page < 0)
		$page = 0;
	if($everynum < 0)
		$everynum = 10;
	$beg = $page*$everynum;
	
	$Query = "SELECT * FROM {$name} LIMIT {$beg},{$everynum};";
	if($Result = mysql_query($Query))
	{
		echo "<table Border=1>";
		echo "<tr><th>id</th><th>data</th></tr>";
		while ($Row = mysql_fetch_array($Result, MYSQL_ASSOC))
		{
			echo "<tr><th>{$Row['id']}</th><th>{$Row['data']}</th></tr>";
		}
		echo "</table>";
		if($page > 0)
			echo "<a href=">上一页"
		return true;
	}
	else
	{
		echo $Query." fail ".mysql_error();
		return false;
	}
?>