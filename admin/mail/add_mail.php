<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(801);
gm_log('发送邮件', 'roleid=' . $_POST['m_roleid'] . ' items=' . $_POST['m_items']);
include '../socketbyte.php';

$array = explode("\r\n", $_POST['m_items']);
$arrayitem = array();
foreach($array as $id => $item)
{
    $items = explode("*", $item);
    $arrayitem[] = array(
        "m_itemtid" => $items[0],
        "m_itemtcount" => $items[1],
    );
}

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_MAIL',
    'operator' => 'add_mail',
    'data' => array(
        'm_roleid' => $_POST['m_roleid'],
        'm_content' => $_POST['m_content'],
        'm_items' => $arrayitem,
    )
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '发送邮件', 'add_mail_html.php');
?>
