<?php
	require_once '../config.php';
	
	class payapi{
	
		function write_log($data)
		{
				file_put_contents("test.txt", "\n".date('Y-m-d H-i-s') . "   " . $data."\n", FILE_APPEND);
		}

		function ret_value($err, $msg)
		{
			$resultData = array();
			$resultData['ret'] = -2;
			$resultData['msg'] = "sign fail";
			$resultDataJson = json_encode($resultData);
			echo $resultDataJson;
			$this->write_log($resultDataJson);
		}
		
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
			//echo "[{$orderid}]###[area:{$area}][id:{$id}][rechargeid:{$rechargeid}][createtime:{$createtime}]<br/>";
			
			$con = mysql_connect(DB_IP . ":" . DB_PORT, DB_USER, DB_PASS);
			if(!$con)
			{
				return -1;
			}
			
			mysql_select_db(GMSYS, $con);
			mysql_query("set names 'utf8'");
			
			$QueryInster = "insert into db_recharge(orderid,roleid,rechargeid,createtime,gm,stat) values ('{$orderid}', {$roleid}, {$rechargeid}, now(), {$gm}, {$stat})";
			$QueryInster = $QueryInster." on duplicate key update gm=values(gm),stat=values(stat);";
			if(mysql_query($QueryInster))
			{
				return 1;
			}
			else
			{
				return -2;
			}
		}
	}
	
	
?>