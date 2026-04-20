<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(803);
gm_log('删除邮件', 'roleid=' . $_POST['m_roleid'] . ' mailid=' . $_POST['m_mailid']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_MAIL',
    'operator' => 'del_mail',
    'data' => array(
        "m_roleid" => $_POST['m_roleid'],
        "m_mailid" => $_POST['m_mailid']
    )
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '删除邮件', 'del_mail_html.php');
?>
