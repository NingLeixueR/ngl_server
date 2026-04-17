<?php
require_once dirname(__FILE__) . '/auth.php';

if (!empty($_SESSION['admin_logged_in']))
{
    header('Location: index.php');
    exit;
}

$error = '';
if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
    $username = isset($_POST['username']) ? trim($_POST['username']) : '';
    $password = isset($_POST['password']) ? $_POST['password'] : '';
    if ($username === '' || $password === '')
    {
        $error = '请输入用户名和密码';
    }
    else if (do_login($username, $password))
    {
        header('Location: index.php');
        exit;
    }
    else
    {
        $error = '用户名或密码错误';
    }
}
?>
<html>
<head>
<meta charset="UTF-8">
<title>管理后台登录</title>
<style>
body { font-family: Arial, sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }
.login-box { background: #fff; padding: 32px; border-radius: 8px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); width: 320px; }
.login-box h2 { text-align: center; margin: 0 0 24px; color: #333; }
.login-box input[type="text"], .login-box input[type="password"] { width: 100%; padding: 10px; margin-bottom: 16px; border: 1px solid #d9d9d9; border-radius: 4px; box-sizing: border-box; font-size: 14px; }
.login-box input[type="submit"] { width: 100%; padding: 10px; background: #1890ff; color: #fff; border: none; border-radius: 4px; font-size: 16px; cursor: pointer; }
.login-box input[type="submit"]:hover { background: #40a9ff; }
.error { color: #ff4d4f; text-align: center; margin-bottom: 16px; font-size: 14px; }
</style>
</head>
<body>
<div class="login-box">
    <h2>NGL Admin</h2>
    <?php if ($error !== '') { echo "<div class='error'>{$error}</div>"; } ?>
    <form method="post">
        <input type="text" name="username" placeholder="用户名" value="<?php echo isset($_POST['username']) ? htmlspecialchars($_POST['username']) : ''; ?>"/>
        <input type="password" name="password" placeholder="密码"/>
        <input type="submit" value="登录"/>
    </form>
</div>
</body>
</html>
