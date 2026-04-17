<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1401);
gm_log('查看服务器状态', '');
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_GM',
    'operator' => 'server_stat',
    'servertype' => $_POST['servertype'],
    'data' => array()
);
$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(count($_POST['servertype']));

$type_names = array(
    1 => 'DB', 2 => 'ACTOR', 3 => 'GAME', 4 => 'GATEWAY',
    5 => 'LOGIN', 6 => 'ROBOT', 7 => 'WORLD', 8 => 'LOG',
    9 => 'RELOADCSV', 10 => 'CROSS', 11 => 'CROSSDB', 12 => 'PUSHCONFIG'
);
?>
<html>
<head><meta charset="UTF-8"><title>服务器状态</title>
<style>
body { font-family: Arial, sans-serif; background: #f0f2f5; padding: 20px; }
.card { background: #fff; border-radius: 6px; padding: 16px; margin-bottom: 16px; box-shadow: 0 1px 4px rgba(0,0,0,0.08); }
.card h3 { margin: 0 0 12px; font-size: 15px; color: #333; border-bottom: 1px solid #f0f0f0; padding-bottom: 8px; }
table { border-collapse: collapse; width: 100%; }
th, td { border: 1px solid #e8e8e8; padding: 6px 12px; text-align: left; font-size: 13px; }
th { background: #fafafa; }
a { color: #1890ff; text-decoration: none; }
</style>
</head>
<body>
<h2>服务器状态 <a href="server_stat_html.php">返回</a></h2>
<?php if (empty($results)): ?>
<p>未收到任何服务器响应</p>
<?php endif; ?>
<?php foreach ($results as $resp): ?>
<div class="card">
<h3><?php echo htmlspecialchars(isset($resp['server_name']) ? $resp['server_name'] : '未知'); ?></h3>
<?php if (isset($resp['data']['m_vec']) && is_array($resp['data']['m_vec'])): ?>
<table>
<tr><th>Actor名称</th><th>区服/实例</th></tr>
<?php foreach ($resp['data']['m_vec'] as $item): ?>
<tr>
<td><?php echo htmlspecialchars($item['m_actor_name']); ?></td>
<td><?php
if (isset($item['m_actor']) && is_array($item['m_actor']))
{
    $parts = array();
    foreach ($item['m_actor'] as $area => $ids)
    {
        $parts[] = 'area=' . $area . ': [' . implode(',', $ids) . ']';
    }
    echo htmlspecialchars(implode('; ', $parts));
}
?></td>
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
