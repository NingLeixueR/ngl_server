<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1601);
gm_log('查询时间', '');
include '../socketbyte.php';

date_default_timezone_set('Asia/Shanghai');

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_GM',
    'operator' => 'get_time',
    'servertype' => $_POST['servertype'],
    'data' => array()
);
$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(count($_POST['servertype']));
?>
<html>
<head><meta charset="UTF-8"><title>服务器时间</title>
<link rel="stylesheet" href="../style.css">
</head>
<body>
<h2>服务器时间 <a href="get_time_html.php">返回</a></h2>
<?php if (empty($results)): ?>
<p>未收到任何服务器响应</p>
<?php else: ?>
<table>
<tr><th>服务器</th><th>时间</th></tr>
<?php foreach ($results as $resp): ?>
<tr>
<td><?php echo htmlspecialchars(isset($resp['server_name']) ? $resp['server_name'] : '未知'); ?></td>
<td><?php echo htmlspecialchars(isset($resp['data']) ? $resp['data'] : '-'); ?></td>
</tr>
<?php endforeach; ?>
</table>
<?php endif; ?>
</body>
</html>
