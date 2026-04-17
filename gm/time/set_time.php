<?php
 include '../SocketByte.php';
 
 date_default_timezone_set('Asia/Shanghai');
 
 $so = new SocketByte();
 if($so->connectServer($_POST['server']) == false)
 {
	 echo "connect err!!!";
	 return;
 }
 
 $year = $_POST['year'];
 $mon = $_POST['mon'];
 $day = $_POST['day'];
 $hour = $_POST['hour'];
 $min = $_POST['min'];
 $sec = $_POST['sec'];
 echo "{$year}-{$mon}-{$day} {$hour}:{$min}:{$sec}<br/>";
 $totime = mktime($hour, $min, $sec, $mon, $day, $year);
 echo "utc:{$totime}<br/>";

 /*
	需要发送给服务器的字段包括
	actor_name  指定actor的类型  单例必须
	actor_id	操作制定actor    非单例必须
	operator	制定操作类型（自定义字符串）
	data        可选数据
 */
 
 
 $arr = array(
	'actor_name' => 'ACTOR_GM',
	'operator' => 'set_time',
	'data' => array(
		'servertype' => $_POST['servertype'],
		'time' => "{$totime}"
	)
 );
 
 $json = json_encode($arr); 
 $so->send($json);
 
 $count = count($_POST['servertype']);
 while(--$count >= 0)
 {
	$response = $so->wait_response();	 
 }

?>