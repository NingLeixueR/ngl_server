<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(901);
gm_log('添加公告', 'notice=' . $_POST['m_notice']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_NOTICE',
    'operator' => 'add_notice',
    'data' => array(
        'id' => -1,
        'notice' => $_POST['m_notice'],
        'starttime' => intval($_POST['m_starttime']),
        'finishtime' => intval($_POST['m_finishtime']),
    )
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '添加公告', 'add_notice_html.php');
?>
