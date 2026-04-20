<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(903);
gm_log('删除公告', 'id=' . $_POST['id']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_NOTICE',
    'operator' => 'del_notice',
    'data' => intval($_POST['id'])
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '删除公告', 'get_notice_html.php');
?>
