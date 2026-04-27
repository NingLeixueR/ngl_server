<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1502);
gm_log('Socket缓冲池状态', 'server=' . $_POST['server']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_GM',
    'operator' => 'socket_pool_stats',
	'servertype' => $_POST['servertype'],
    'data' => array()
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(count($_POST['servertype']));

include '../render.php';
render_response($results, 'Socket缓冲池状态', 'socket_pool_stats_html.php');
?>