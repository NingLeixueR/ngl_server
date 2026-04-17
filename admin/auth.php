<?php
session_start();
require_once __DIR__ . '/config.php';

function check_login()
{
    if (empty($_SESSION['admin_logged_in']))
    {
        $dir = rtrim(dirname($_SERVER['SCRIPT_NAME']), '/\\');
        $admin_dir = '/' . basename(__DIR__);
        if ($dir === $admin_dir)
        {
            header('Location: login.php');
        }
        else
        {
            header('Location: ../login.php');
        }
        exit;
    }
}

function do_login($username, $password)
{
    $con = mysql_connect(DB_IP . ':' . DB_PORT, DB_USER, DB_PASS);
    if (!$con)
    {
        return false;
    }
    mysql_select_db(GMSYS, $con);
    mysql_query("set names 'utf8'");

    $safe_user = mysql_real_escape_string($username);
    $safe_pass = md5($password);
    $result = mysql_query("SELECT id, username, permissions FROM db_admin WHERE username='{$safe_user}' AND password='{$safe_pass}' LIMIT 1");
    if ($result && mysql_num_rows($result) > 0)
    {
        $row = mysql_fetch_assoc($result);
        $_SESSION['admin_logged_in'] = true;
        $_SESSION['admin_username'] = $row['username'];
        $_SESSION['admin_id'] = $row['id'];
        $_SESSION['admin_permissions'] = $row['permissions'];
        return true;
    }
    return false;
}

function do_logout()
{
    session_destroy();
}

function check_permission($module)
{
    $perms = isset($_SESSION['admin_permissions']) ? $_SESSION['admin_permissions'] : '';
    if ($perms === '*')
    {
        return true;
    }
    $list = array_map('trim', explode(',', $perms));
    return in_array($module, $list);
}
?>
