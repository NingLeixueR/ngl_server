<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(601);
gm_log('查询家族', 'familyid=' . $_POST['m_familyid']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_FAMILY',
    'operator' => 'get_family',
    'data' => $_POST['m_familyid']
);
$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '家族信息', 'get_family_html.php');
?>
