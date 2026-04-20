<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(503);
gm_log('修改数据', 'db=' . $_POST['db'] . ' data=' . $_POST['m_data']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_DB',
    'operator' => 'change',
    'db' => intval($_POST['db']),
    'data' => "{$_POST['m_data']}",
);
$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '修改数据', 'changedb_html.php');
?>
