<?php require_once dirname(__FILE__) . '/../auth.php'; check_action(1701);

$con = mysql_connect(DB_IP . ':' . DB_PORT, DB_USER, DB_PASS);
mysql_select_db(GMSYS, $con);
mysql_query("set names 'utf8'");

$where = '1=1';
$q_admin = isset($_GET['admin_name']) ? trim($_GET['admin_name']) : '';
$q_action = isset($_GET['action_name']) ? trim($_GET['action_name']) : '';
$q_date_from = isset($_GET['date_from']) ? trim($_GET['date_from']) : '';
$q_date_to = isset($_GET['date_to']) ? trim($_GET['date_to']) : '';

if ($q_admin !== '')
{
    $safe = mysql_real_escape_string($q_admin);
    $where .= " AND admin_name='{$safe}'";
}
if ($q_action !== '')
{
    $safe = mysql_real_escape_string($q_action);
    $where .= " AND action_name LIKE '%{$safe}%'";
}
if ($q_date_from !== '')
{
    $safe = mysql_real_escape_string($q_date_from);
    $where .= " AND created_at >= '{$safe} 00:00:00'";
}
if ($q_date_to !== '')
{
    $safe = mysql_real_escape_string($q_date_to);
    $where .= " AND created_at <= '{$safe} 23:59:59'";
}

$page = isset($_GET['p']) ? max(1, intval($_GET['p'])) : 1;
$pagesize = 50;
$offset = ($page - 1) * $pagesize;

$count_result = mysql_query("SELECT COUNT(*) as cnt FROM db_gmlog WHERE {$where}");
$total = 0;
if ($count_result)
{
    $cr = mysql_fetch_assoc($count_result);
    $total = intval($cr['cnt']);
}
$total_pages = max(1, ceil($total / $pagesize));

$result = mysql_query("SELECT * FROM db_gmlog WHERE {$where} ORDER BY id DESC LIMIT {$offset},{$pagesize}");
$logs = array();
if ($result)
{
    while ($row = mysql_fetch_assoc($result)) $logs[] = $row;
}

function build_query_string($overrides = array())
{
    $params = array('admin_name', 'action_name', 'date_from', 'date_to', 'p');
    $qs = array();
    foreach ($params as $k)
    {
        $v = isset($overrides[$k]) ? $overrides[$k] : (isset($_GET[$k]) ? $_GET[$k] : '');
        if ($v !== '') $qs[] = $k . '=' . urlencode($v);
    }
    return '?' . implode('&', $qs);
}
?>
<html>
<head><meta charset="UTF-8"><title>操作日志</title>
<link rel="stylesheet" href="../style.css">
</head>
<body>
<h2>操作日志 <a href="../index.php" style="font-size:14px;color:#1890ff;text-decoration:none;">返回首页</a></h2>
<div class="filter">
<form method="get">
    <label>操作人:</label><input type="text" name="admin_name" value="<?php echo htmlspecialchars($q_admin); ?>"/>
    <label>操作:</label><input type="text" name="action_name" value="<?php echo htmlspecialchars($q_action); ?>"/>
    <label>从:</label><input type="date" name="date_from" value="<?php echo htmlspecialchars($q_date_from); ?>"/>
    <label>到:</label><input type="date" name="date_to" value="<?php echo htmlspecialchars($q_date_to); ?>"/>
    <input type="submit" value="查询"/>
</form>
</div>

<p>共 <?php echo $total; ?> 条记录，第 <?php echo $page; ?>/<?php echo $total_pages; ?> 页</p>
<table>
<tr><th>ID</th><th>操作人</th><th>操作</th><th>详情</th><th>IP</th><th>时间</th></tr>
<?php foreach ($logs as $log): ?>
<tr>
    <td><?php echo $log['id']; ?></td>
    <td><?php echo htmlspecialchars($log['admin_name']); ?></td>
    <td><?php echo htmlspecialchars($log['action_name']); ?></td>
    <td class="detail"><?php echo htmlspecialchars($log['detail']); ?></td>
    <td><?php echo htmlspecialchars($log['ip']); ?></td>
    <td><?php echo $log['created_at']; ?></td>
</tr>
<?php endforeach; ?>
</table>

<div class="pager">
<?php for ($i = 1; $i <= $total_pages; $i++): ?>
<a href="<?php echo htmlspecialchars(build_query_string(array('p' => $i))); ?>" class="<?php echo $i === $page ? 'cur' : ''; ?>"><?php echo $i; ?></a>
<?php endfor; ?>
</div>
</body>
</html>
