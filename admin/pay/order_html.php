<?php require_once dirname(__FILE__) . '/../auth.php'; check_action(1102);

$con = mysql_connect(DB_IP . ':' . DB_PORT, DB_USER, DB_PASS);
mysql_select_db(GMSYS, $con);
mysql_query("set names 'utf8'");

$where = '1=1';
$q_orderid = isset($_GET['orderid']) ? trim($_GET['orderid']) : '';
$q_roleid = isset($_GET['roleid']) ? trim($_GET['roleid']) : '';
$q_stat = isset($_GET['stat']) ? $_GET['stat'] : '';
$q_date_from = isset($_GET['date_from']) ? trim($_GET['date_from']) : '';
$q_date_to = isset($_GET['date_to']) ? trim($_GET['date_to']) : '';

if ($q_orderid !== '')
{
    $safe = mysql_real_escape_string($q_orderid);
    $where .= " AND orderid='{$safe}'";
}
if ($q_roleid !== '')
{
    $safe = mysql_real_escape_string($q_roleid);
    $where .= " AND roleid='{$safe}'";
}
if ($q_stat !== '')
{
    $where .= " AND stat=" . intval($q_stat);
}
if ($q_date_from !== '')
{
    $safe = mysql_real_escape_string($q_date_from);
    $where .= " AND createtime >= '{$safe} 00:00:00'";
}
if ($q_date_to !== '')
{
    $safe = mysql_real_escape_string($q_date_to);
    $where .= " AND createtime <= '{$safe} 23:59:59'";
}

$page = isset($_GET['p']) ? max(1, intval($_GET['p'])) : 1;
$pagesize = 50;
$offset = ($page - 1) * $pagesize;

$count_result = mysql_query("SELECT COUNT(*) as cnt FROM db_recharge WHERE {$where}");
$total = 0;
if ($count_result)
{
    $cr = mysql_fetch_assoc($count_result);
    $total = intval($cr['cnt']);
}
$total_pages = max(1, ceil($total / $pagesize));
$result = mysql_query("SELECT * FROM db_recharge WHERE {$where} ORDER BY createtime DESC LIMIT {$offset},{$pagesize}");
$orders = array();
if ($result)
{
    while ($row = mysql_fetch_assoc($result)) $orders[] = $row;
}

if ($q_orderid !== '' || $q_roleid !== '')
{
    gm_log('订单查询', 'orderid=' . $q_orderid . ' roleid=' . $q_roleid);
}

$stat_map = array(0 => '待处理', 1 => '成功', 2 => '失败');

function build_qs($overrides = array())
{
    $params = array('orderid', 'roleid', 'stat', 'date_from', 'date_to', 'p');
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
<head><meta charset="UTF-8"><title>订单查询</title>
<style>
body { font-family: Arial, sans-serif; background: #f0f2f5; padding: 20px; }
.filter { background: #fff; padding: 16px; border-radius: 6px; margin-bottom: 16px; box-shadow: 0 1px 4px rgba(0,0,0,0.08); }
.filter label { font-size: 14px; margin-right: 8px; }
.filter input[type="text"], .filter input[type="date"], .filter select { padding: 6px 10px; border: 1px solid #d9d9d9; border-radius: 4px; margin-right: 12px; }
.filter input[type="submit"] { padding: 6px 16px; background: #1890ff; color: #fff; border: none; border-radius: 4px; cursor: pointer; }
table { border-collapse: collapse; width: 100%; background: #fff; }
th, td { border: 1px solid #e8e8e8; padding: 8px 12px; text-align: left; font-size: 13px; }
th { background: #fafafa; }
.pager { margin-top: 12px; text-align: center; }
.pager a { display: inline-block; padding: 4px 12px; margin: 0 2px; border: 1px solid #d9d9d9; border-radius: 4px; color: #1890ff; text-decoration: none; font-size: 13px; }
.pager a.cur { background: #1890ff; color: #fff; border-color: #1890ff; }
</style>
</head>
<body>
<h2>充值订单查询 <a href="../index.php" style="font-size:14px;color:#1890ff;text-decoration:none;">返回首页</a></h2>
<div class="filter">
<form method="get">
    <label>订单号:</label><input type="text" name="orderid" value="<?php echo htmlspecialchars($q_orderid); ?>"/>
    <label>角色ID:</label><input type="text" name="roleid" value="<?php echo htmlspecialchars($q_roleid); ?>"/>
    <label>状态:</label><select name="stat"><option value="">全部</option><option value="0" <?php echo $q_stat==='0'?'selected':''; ?>>待处理</option><option value="1" <?php echo $q_stat==='1'?'selected':''; ?>>成功</option><option value="2" <?php echo $q_stat==='2'?'selected':''; ?>>失败</option></select>
    <label>从:</label><input type="date" name="date_from" value="<?php echo htmlspecialchars($q_date_from); ?>"/>
    <label>到:</label><input type="date" name="date_to" value="<?php echo htmlspecialchars($q_date_to); ?>"/>
    <input type="submit" value="查询"/>
</form>
</div>

<p>共 <?php echo $total; ?> 条记录，第 <?php echo $page; ?>/<?php echo $total_pages; ?> 页</p>
<table>
<tr><th>订单号</th><th>充值ID</th><th>角色ID</th><th>GM</th><th>状态</th><th>创建时间</th></tr>
<?php foreach ($orders as $o): ?>
<tr>
    <td><?php echo htmlspecialchars($o['orderid']); ?></td>
    <td><?php echo $o['rechargeid']; ?></td>
    <td><?php echo $o['roleid']; ?></td>
    <td><?php echo $o['gm']; ?></td>
    <td><?php echo isset($stat_map[$o['stat']]) ? $stat_map[$o['stat']] : $o['stat']; ?></td>
    <td><?php echo $o['createtime']; ?></td>
</tr>
<?php endforeach; ?>
</table>

<div class="pager">
<?php for ($i = 1; $i <= $total_pages; $i++): ?>
<a href="<?php echo htmlspecialchars(build_qs(array('p' => $i))); ?>" class="<?php echo $i === $page ? 'cur' : ''; ?>"><?php echo $i; ?></a>
<?php endfor; ?>
</div>
</body>
</html>
