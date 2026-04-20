<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(602);
gm_log('家族改名', 'familyid=' . $_POST['m_familyid'] . ' name=' . $_POST['m_familyname']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_FAMILY',
    'operator' => 'change_familyname',
    'data' => array(
        "familid" => $_POST['m_familyid'],
        "familname" => $_POST['m_familyname']
    )
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '家族改名', 'changname_family_html.php');
?>
