<?php
$con = mysql_connect(DB_IP . ':' . DB_PORT, DB_USER, DB_PASS);
mysql_select_db(GMSYS, $con);
mysql_query("set names 'utf8'");

$t = isset($_REQUEST['t']) ? intval($_REQUEST['t']) : 0;
$id = isset($_REQUEST['id']) ? intval($_REQUEST['id']) : 0;

if ($t === 1 && $id > 0)
{
    mysql_query("DELETE FROM db_admin WHERE id={$id}");
    echo "<script>alert('删除成功');location.href='index.php?a=gmuser';</script>";
    exit;
}
else if ($t === 2 && $id > 0)
{
    $new_pass = md5('123456');
    mysql_query("UPDATE db_admin SET password='{$new_pass}' WHERE id={$id}");
    echo "<script>alert('密码已重置为 123456');location.href='index.php?a=gmuser';</script>";
    exit;
}
else if ($t === 3 && $_SERVER['REQUEST_METHOD'] === 'POST')
{
    $name = mysql_real_escape_string(trim($_POST['username']));
    $pass = md5(trim($_POST['password']));
    if ($name === '' || trim($_POST['password']) === '')
    {
        echo "<script>alert('用户名和密码不能为空');history.back();</script>";
        exit;
    }
    $check = mysql_query("SELECT id FROM db_admin WHERE username='{$name}'");
    if ($check && mysql_num_rows($check) > 0)
    {
        echo "<script>alert('用户名已存在');history.back();</script>";
        exit;
    }
    mysql_query("INSERT INTO db_admin (username, password, permissions) VALUES ('{$name}', '{$pass}', '[]')");
    echo "<script>alert('添加成功');location.href='index.php?a=gmuser';</script>";
    exit;
}

$users = array();
$result = mysql_query("SELECT id, username, permissions, created_at FROM db_admin ORDER BY id");
while ($row = mysql_fetch_assoc($result))
{
    $users[] = $row;
}
?>
<html>
<head><meta charset="UTF-8"><title>GM账号管理</title>
<style>
body { font-family: Arial, sans-serif; background: #f0f2f5; padding: 20px; }
table { border-collapse: collapse; width: 100%; background: #fff; }
th, td { border: 1px solid #e8e8e8; padding: 8px 12px; text-align: left; font-size: 14px; }
th { background: #fafafa; }
a { color: #1890ff; text-decoration: none; margin-right: 8px; }
input[type="text"], input[type="password"] { padding: 6px 10px; border: 1px solid #d9d9d9; border-radius: 4px; margin-right: 8px; }
input[type="submit"] { padding: 6px 16px; background: #1890ff; color: #fff; border: none; border-radius: 4px; cursor: pointer; }
</style>
</head>
<body>
<h2>GM账号管理 <a href="index.php">返回首页</a></h2>
<table>
<tr><th>ID</th><th>用户名</th><th>权限</th><th>创建时间</th><th>操作</th></tr>
<?php foreach ($users as $u): ?>
<tr>
    <td><?php echo $u['id']; ?></td>
    <td><?php echo htmlspecialchars($u['username']); ?></td>
    <td><?php echo $u['permissions'] === 'all' ? '全部权限' : '部分权限'; ?></td>
    <td><?php echo $u['created_at']; ?></td>
    <td>
        <a href="index.php?a=chmod&id=<?php echo $u['id']; ?>">设置权限</a>
        <a href="index.php?a=gmuser&t=2&id=<?php echo $u['id']; ?>" onclick="return confirm('确认重置密码为123456?')">重置密码</a>
        <a href="index.php?a=gmuser&t=1&id=<?php echo $u['id']; ?>" onclick="return confirm('确认删除?')">删除</a>
    </td>
</tr>
<?php endforeach; ?>
</table>

<h3>添加新账号</h3>
<form method="post" action="index.php?a=gmuser&t=3">
    <input type="text" name="username" placeholder="用户名"/>
    <input type="password" name="password" placeholder="密码"/>
    <input type="submit" value="添加"/>
</form>
</body>
</html>
