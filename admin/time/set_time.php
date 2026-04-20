<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1602);
gm_log('设置时间', $_POST['year'] . '-' . $_POST['mon'] . '-' . $_POST['day'] . ' ' . $_POST['hour'] . ':' . $_POST['min'] . ':' . $_POST['sec']);
include '../socketbyte.php';

date_default_timezone_set('Asia/Shanghai');

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$year = $_POST['year'];
$mon = $_POST['mon'];
$day = $_POST['day'];
$hour = $_POST['hour'];
$min = $_POST['min'];
$sec = $_POST['sec'];
$time_str = "{$year}-{$mon}-{$day} {$hour}:{$min}:{$sec}";
$totime = mktime($hour, $min, $sec, $mon, $day, $year);

$arr = array(
    'actor_name' => 'ACTOR_GM',
    'operator' => 'set_time',
    'servertype' => $_POST['servertype'],
    'data' => array(
        'time' => "{$totime}"
    )
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(count($_POST['servertype']));
?>
<html>
<head><meta charset="UTF-8"><title>设置时间结果</title>
<link rel="stylesheet" href="../style.css">
</head>
<body>
<h2>设置时间结果 <a href="set_time_html.php">返回</a></h2>
<p>目标时间: <?php echo htmlspecialchars($time_str); ?> (UTC: <?php echo $totime; ?>)</p>
<table>
<tr><th>服务器</th><th>结果</th></tr>
<?php foreach ($results as $resp): ?>
<tr>
<td><?php echo htmlspecialchars(isset($resp['server_name']) ? $resp['server_name'] : '未知'); ?></td>
<td><?php if (isset($resp['data']) && $resp['data']): ?>
<span class="ok">成功</span>
<?php else: ?>
<span class="fail">失败</span>
<?php endif; ?></td>
</tr>
<?php endforeach; ?>
</table>
</body>
</html>
