<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(802);
gm_log('查询邮件', 'roleid=' . $_POST['m_roleid']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_MAIL',
    'operator' => 'get_mails',
    'data' => $_POST['m_roleid']
);
$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(1);

include '../render.php';
render_response($results, '邮件列表', 'get_mail_html.php');
?>
