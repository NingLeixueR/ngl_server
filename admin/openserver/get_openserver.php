<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1002);
gm_log('查询开服', '');
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'actor_keyvalue',
    'operator' => 'get_openserver',
    'data' => array(),
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '开服信息', 'get_openserver_html.php');
?>
