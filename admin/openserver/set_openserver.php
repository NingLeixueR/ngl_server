<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1001);
gm_log('设置开服', 'time=' . $_POST['m_time']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'actor_keyvalue',
    'operator' => 'set_openserver',
    'data' => intval($_POST['m_time']),
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '设置开服', 'set_openserver_html.php');
?>
