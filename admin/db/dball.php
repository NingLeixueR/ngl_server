<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(502);
gm_log('分页查询', 'db=' . $_POST['db'] . ' page=' . $_POST['m_page']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_DB',
    'operator' => 'queryall',
    'db' => intval($_POST['db']),
    'data' => array(
        'm_everypagecount' => intval($_POST['m_everypagecount']),
        'm_page' => intval($_POST['m_page']),
    )
);
$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, 'DB查询', 'dball_html.php');
?>
