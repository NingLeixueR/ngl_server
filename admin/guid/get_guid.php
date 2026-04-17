<?php
 include '../SocketByte.php';
 
 $so = new SocketByte();
 if($so->connectServer($_POST['server']) == false)
 {
	 echo "connect err!!!";
	 return;
 }
 
 /*
	需要发送给服务器的字段包括
	actor_name  指定actor的类型  单例必须
	actor_id	操作制定actor    非单例必须
	servertype  指定服务器类型
	enum NODE_TYPE
	{
		DB = 1,	// Databaseserver
		ACTORSERVER = 2,	// Actor server
		GAME = 3,	// Server
		GATEWAY = 4,	// Gatewayserver
		LOGIN = 5,	// Loginserver
		ROBOT = 6,	// Test server
		WORLD = 7,	// Server
		LOG = 8,	// Logserver
		RELOADCSV = 9,	// Csvserver
		CROSS = 10,	// Cross-serverserver
		CROSSDB = 11,	// Cross-serverdatabaseserver
		PUSHSERVERCONFIG = 12,	// Serverconfigon lbgmsys
		NODE_TYPE_COUNT,	
		FAIL = -1,	// Servertype
	};
	operator	制定操作类型（自定义字符串）
	data        可选数据	
 */
 
 $arr = array(
	'actor_name' => 'ACTOR_GM',
	'operator' => 'guid',
	'data' => array(
		'servertype' => array("7"),
		'actor_name' => $_POST['m_actor_name'],
		'area' => $_POST['m_area'],
		'dataid' => $_POST['m_dataid'],
	)
 );
 
 $json = json_encode($arr); 
 $so->send($json);
 $response = $so->wait_response(); 

?>