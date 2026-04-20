<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1201);
gm_log('查询协议', '');
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_GM',
    'operator' => 'all_protocol',
    'servertype' => $_POST['servertype'],
    'data' => array()
);
$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(count($_POST['servertype']));
?>
<html>
<head><meta charset="UTF-8"><title>全部协议</title>
<link rel="stylesheet" href="../style.css">
</head>
<body>
<h2>全部协议 <a href="get_allprotocol_html.php">返回</a></h2>
<?php if (empty($results)): ?>
<p>未收到任何服务器响应</p>
<?php endif; ?>
<?php foreach ($results as $resp): ?>
<div class="card">
<h3><?php echo htmlspecialchars(isset($resp['server_name']) ? $resp['server_name'] : '未知'); ?></h3>
<?php if (isset($resp['data']['m_data']) && is_array($resp['data']['m_data'])): ?>
<table>
<tr><th>协议号</th><th>协议名称</th></tr>
<?php foreach ($resp['data']['m_data'] as $pid => $pname): ?>
<tr>
<td><?php echo htmlspecialchars($pid); ?></td>
<td><?php echo htmlspecialchars($pname); ?></td>
</tr>
<?php endforeach; ?>
</table>
<?php else: ?>
<pre><?php echo htmlspecialchars(json_encode($resp['data'])); ?></pre>
<?php endif; ?>
</div>
<?php endforeach; ?>
</body>
</html>
