<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(902);
gm_log('查询公告', '');
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_NOTICE',
    'operator' => 'get_notice',
    'm_data' => ''
);
$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '公告列表', '../notice/get_notice_html.php');
?>
