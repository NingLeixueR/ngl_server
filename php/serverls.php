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
	
	echo '<select name="server">';
	while ($Row = mysql_fetch_array($QueryResult, MYSQL_ASSOC))
	{
		if($Row['type'] == 7)
			echo "<option value=\"".$Row['id']."\">".$Row['name']."-".$Row['area']."-".$Row['ip']."</option>";
	}
	echo "</select><br/>";
	
?>