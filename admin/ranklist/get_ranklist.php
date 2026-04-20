<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1301);
gm_log('查询排行', 'type=' . $_POST['type'] . ' page=' . $_POST['page']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_RANKLIST',
    'operator' => 'ranklist',
    'data' => array(
        'type' => $_POST['type'],
        'page' => $_POST['page'],
    )
);
$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '排行榜', 'get_ranklist_html.php');
?>
