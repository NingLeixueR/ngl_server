<?php
session_start();
header('Content-Type: text/html; charset=utf-8');
require_once dirname(__FILE__) . '/config.php';
require_once dirname(__FILE__) . '/actionmanager.php';

function check_login()
{
    if (empty($_SESSION['admin_logged_in']))
    {
        $dir = rtrim(dirname($_SERVER['SCRIPT_NAME']), '/\\');
        $admin_dir = '/' . basename(dirname(__FILE__));
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
    if (!$con) return false;
    mysql_select_db(GMSYS, $con);
    mysql_query("set names 'utf8'");

    $safe_user = mysql_real_escape_string($username);
    $safe_pass = md5($password);
    $result = mysql_query("SELECT id, username FROM db_admin WHERE username='{$safe_user}' AND password='{$safe_pass}' LIMIT 1");
    if ($result && mysql_num_rows($result) > 0)
    {
        $row = mysql_fetch_assoc($result);
        $_SESSION['admin_logged_in'] = true;
        $_SESSION['admin_username'] = $row['username'];
        $_SESSION['admin_id'] = $row['id'];
        $_SESSION['actionlist'] = ActionManager::getAllowActionList($row['id']);
        return true;
    }
    return false;
}

function do_logout()
{
    session_destroy();
}

function update_actionlist()
{
    $_SESSION['actionlist'] = ActionManager::getAllowActionList($_SESSION['admin_id']);
}

function check_action($actionid)
{
    check_login();
    $actionlist = isset($_SESSION['actionlist']) ? $_SESSION['actionlist'] : array();
    if (!isset($actionlist[$actionid]))
    {
        echo '<h3>无权限访问此功能</h3>';
        exit;
    }
}
?>
