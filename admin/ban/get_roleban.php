<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(302);
gm_log('查询封号', '');
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'actor_role_manage',
    'operator' => 'get_roleban',
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '封号列表', 'get_roleban_html.php');
?>
