<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(701);
gm_log('获取GUID', 'actor=' . $_POST['m_actor_name'] . ' area=' . $_POST['m_area'] . ' dataid=' . $_POST['m_dataid']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_GM',
    'operator' => 'guid',
    'servertype' => array("7"),
    'data' => array(
        'actor_name' => $_POST['m_actor_name'],
        'area' => $_POST['m_area'],
        'dataid' => $_POST['m_dataid'],
    )
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '获取GUID', 'get_guid_html.php');
?>
