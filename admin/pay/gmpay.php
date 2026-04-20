<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1101);
gm_log('GM充值', 'roleid=' . $_POST['roleid'] . ' rechangeid=' . $_POST['rechangeid'] . ' area=' . $_POST['area']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_id' => $_POST['roleid'],
    'operator' => 'gmrechange',
    'data' => array(
        'rechangeid' => $_POST['rechangeid'],
        'area' => $_POST['area'],
        'dataid' => $_POST['dataid'],
    )
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, 'GM充值', 'gmpay_html.php');
?>
