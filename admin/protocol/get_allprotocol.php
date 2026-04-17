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
<style>
body { font-family: Arial, sans-serif; background: #f0f2f5; padding: 20px; }
.card { background: #fff; border-radius: 6px; padding: 16px; margin-bottom: 16px; box-shadow: 0 1px 4px rgba(0,0,0,0.08); }
.card h3 { margin: 0 0 12px; font-size: 15px; color: #333; border-bottom: 1px solid #f0f0f0; padding-bottom: 8px; }
table { border-collapse: collapse; width: 100%; }
th, td { border: 1px solid #e8e8e8; padding: 4px 12px; text-align: left; font-size: 13px; }
th { background: #fafafa; }
a { color: #1890ff; text-decoration: none; }
</style>
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
