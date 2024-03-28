<?php
require_once 'payapi.php';

class pay_recharge {

	function pay()
	{
		$dataPack = $_GET;
		$time 			= $dataPack['time'];		//time	int(11)	发送请求的时间，UNIX时间戳
		$orderid 		= $dataPack['orderid'];		//orderid	string(30)	平台订单号，唯一
		$roleid			= $dataPack['roleid'];		//roleid	
		$ext 			= $dataPack['ext'];			//ext	string(60)	扩展参数，游戏方自定义，原样返回
		
		$tools = new payapi();
		
		$tools->write_log(json_encode($_GET));
		$pay_key = "111111111111111222222222333333"
		$signMsg= md5($orderid.$time.$ext.$pay_key);
		if($signMsg != $sign)
		{
			$tools->ret_value(-2, "sign fail");
			return;
		}
		$con = mysql_connect(DB_IP . ":" . DB_PORT, DB_USER, DB_PASS);
		if(!$con)
		{
			$tools->ret_value(-3, "mysql_connect error");
			return;
		}
		mysql_select_db(GMSYS, $con);
		mysql_query("set names 'utf8'");
		
		$orderid = $ext;
		$QueryStr = "select * from db_recharge where orderid='" . $orderid ."';";
		$QueryResult = mysql_query($QueryStr);
		$QueryNum = mysql_num_rows($QueryResult);
		if($QueryNum > 0)
		{
			if($QueryRow = mysql_fetch_array($QueryResult, MYSQL_ASSOC))
			{
				if($QueryRow['stat'] == 1)
				{
					$tools->ret_value(-4, "order had finish");
					return;
				}
			}	
		}
		else
		{
			$tools->update_recharge($orderid, $roleid, 0, 2, false);
		}
		
		//// #####
		$area = area($orderid);
		$QueryStr = "select * from db_server where area={$area} and type=7;";
		$QueryResult = mysql_query($QueryStr);
		if ($Row = mysql_fetch_array($QueryResult, MYSQL_ASSOC))
		{
			$controller = new SocketByte;
			if ($controller->connect($Row['nip'],$Row['port']))
			{
				/*
					需要发送给服务器的字段包括
					actor_name  指定actor的类型  单例必须
					actor_id	操作制定actor    非单例必须
					operator	制定操作类型（自定义字符串）
					data        可选数据	
				*/	 
				 $arr = array(
					'actor_id' => $roleid,
					'operator' => 'pay',
					'data' => array(
						'orderid' => $orderid,
						'rechargeid' => rechargeid($orderid),
					)
				 );
				 $controller->send($json);
				 
				 $response = $controller->wait_response(); 
				 $tools->update_recharge($orderid, $roleid, $response['data'], $stat);
				
			}
			
			
			return;
		}
	}
}

?>