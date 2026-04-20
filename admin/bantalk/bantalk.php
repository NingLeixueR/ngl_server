<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(401);
gm_log('禁言', 'roleid=' . $_POST['m_roleid'] . ' duration=' . $_POST['m_duration']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_id' => $_POST['m_roleid'],
    'operator' => 'bantalk',
    'data' => $_POST['m_duration']
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '禁言操作', '../bantalk/bantalk_html.php');
?>
