<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(301);
gm_log('封号', 'roleid=' . $_POST['m_roleid'] . ' stat=' . $_POST['m_stat']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'actor_role_manage',
    'operator' => 'roleban',
    'data' => array(
        'm_roleid' => $_POST['m_roleid'],
        'm_stat' => $_POST['m_stat'],
    )
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '封号操作', 'roleban_html.php');
?>
