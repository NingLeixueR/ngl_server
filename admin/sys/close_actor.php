<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1501);
gm_log('关闭Actor', 'actor=' . $_POST['actor_name'] . ' area=' . $_POST['area'] . ' dataid=' . $_POST['dataid']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_GM',
    'operator' => 'close_actor',
    'servertype' => array(7),
    'data' => array(
        'actor_name' => $_POST['actor_name'],
        'area' => $_POST['area'],
        'dataid' => $_POST['dataid'],
    )
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '关闭Actor', 'close_actor_html.php');
?>
