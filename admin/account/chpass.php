<?php
$error = '';
$success = '';
if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
    $old_pass = isset($_POST['old_password']) ? $_POST['old_password'] : '';
    $new_pass = isset($_POST['new_password']) ? $_POST['new_password'] : '';
    $confirm  = isset($_POST['confirm_password']) ? $_POST['confirm_password'] : '';

    if ($old_pass === '' || $new_pass === '')
    {
        $error = '请填写完整';
    }
    else if ($new_pass !== $confirm)
    {
        $error = '两次密码不一致';
    }
    else
    {
        $con = mysql_connect(DB_IP . ':' . DB_PORT, DB_USER, DB_PASS);
        mysql_select_db(GMSYS, $con);
        mysql_query("set names 'utf8'");
        $uid = intval($_SESSION['admin_id']);
        $old_md5 = md5($old_pass);
        $result = mysql_query("SELECT id FROM db_admin WHERE id={$uid} AND password='{$old_md5}' LIMIT 1");
        if ($result && mysql_num_rows($result) > 0)
        {
            $new_md5 = md5($new_pass);
            mysql_query("UPDATE db_admin SET password='{$new_md5}' WHERE id={$uid}");
            gm_log('修改密码', '');
            $success = '密码修改成功，请重新登录';
            do_logout();
            header('Location: ../login.php');
            exit;
        }
        else
        {
            $error = '原密码错误';
        }
    }
}
?>
<html>
<head><meta charset="UTF-8"><title>修改密码</title>
<link rel="stylesheet" href="style.css">
<style>
body { display: flex; justify-content: center; padding-top: 80px; }
.box { background: #fff; padding: 32px; border-radius: 8px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); width: 320px; }
.box h2 { margin: 0 0 20px; font-size: 18px; }
.box input[type="password"] { width: 100%; padding: 8px; margin-bottom: 12px; border: 1px solid #d9d9d9; border-radius: 4px; box-sizing: border-box; }
.box input[type="submit"] { width: 100%; padding: 8px; background: #1890ff; color: #fff; border: none; border-radius: 4px; cursor: pointer; }
.error { color: #ff4d4f; margin-bottom: 12px; }
</style>
</head>
<body>
<div class="box">
    <h2>修改密码</h2>
    <?php if ($error) echo "<div class='error'>{$error}</div>"; ?>
    <form method="post">
        <input type="password" name="old_password" placeholder="原密码"/>
        <input type="password" name="new_password" placeholder="新密码"/>
        <input type="password" name="confirm_password" placeholder="确认新密码"/>
        <input type="submit" value="修改"/>
    </form>
    <p><a href="index.php">返回首页</a></p>
</div>
</body>
</html>
