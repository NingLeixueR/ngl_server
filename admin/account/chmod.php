<?php
$con = mysql_connect(DB_IP . ':' . DB_PORT, DB_USER, DB_PASS);
mysql_select_db(GMSYS, $con);
mysql_query("set names 'utf8'");

$id = isset($_REQUEST['id']) ? intval($_REQUEST['id']) : 0;
if ($id <= 0) { echo '参数错误'; exit; }

$result = mysql_query("SELECT id, username, permissions FROM db_admin WHERE id={$id}");
$user = mysql_fetch_assoc($result);
if (!$user) { echo '用户不存在'; exit; }

if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
    $act = isset($_POST['act']) ? $_POST['act'] : array();
    if (isset($_POST['allperm']) && $_POST['allperm'] === '1')
    {
        $perm_str = 'all';
    }
    else
    {
        $ids = array_map('intval', $act);
        $perm_str = json_encode(array_values($ids));
    }
    $safe = mysql_real_escape_string($perm_str);
    mysql_query("UPDATE db_admin SET permissions='{$safe}' WHERE id={$id}");
    echo "<script>alert('权限更新成功');location.href='index.php?a=gmuser';</script>";
    exit;
}

$current_ids = ActionManager::getAllowActionIDList($id);
$all_actions = ActionManager::$ActionList;
?>
<html>
<head><meta charset="UTF-8"><title>设置权限</title>
<style>
body { font-family: Arial, sans-serif; background: #f0f2f5; padding: 20px; }
.group { background: #fff; border-radius: 6px; padding: 12px 16px; margin-bottom: 12px; box-shadow: 0 1px 4px rgba(0,0,0,0.08); }
.group h3 { margin: 0 0 8px; font-size: 15px; color: #333; }
.group label { display: inline-block; margin-right: 16px; font-size: 14px; cursor: pointer; }
</style>
</head>
<body>
<h2>设置权限 - <?php echo htmlspecialchars($user['username']); ?> <a href="index.php?a=gmuser">返回</a></h2>
<form method="post">
<p><label><input type="checkbox" name="allperm" value="1" <?php echo $user['permissions'] === 'all' ? 'checked' : ''; ?>/> 全部权限</label></p>
<?php
foreach ($all_actions as $aid => $act)
{
    if ($act['pid'] == 0)
    {
        echo "<div class='group'>";
        $checked = in_array($aid, $current_ids) ? 'checked' : '';
        echo "<h3><label><input type='checkbox' name='act[]' value='{$aid}' {$checked}/> {$act['name']}</label></h3>";
        foreach ($all_actions as $cid => $child)
        {
            if ($child['pid'] == $aid)
            {
                $cc = in_array($cid, $current_ids) ? 'checked' : '';
                echo "<label><input type='checkbox' name='act[]' value='{$cid}' {$cc}/> {$child['name']}</label>";
            }
        }
        echo "</div>";
    }
}
?>
<input type="submit" value="保存" style="padding:8px 24px;background:#1890ff;color:#fff;border:none;border-radius:4px;cursor:pointer;margin-top:12px;"/>
</form>
</body>
</html>
