<?php
require_once dirname(__FILE__) . '/auth.php';
check_login();

$a = isset($_GET['a']) ? $_GET['a'] : '';

if ($a === 'updatelist')
{
    update_actionlist();
    header('Location: index.php');
    exit;
}

if ($a === 'chpass')
{
    include dirname(__FILE__) . '/account/chpass.php';
    exit;
}

if ($a === 'gmuser')
{
    check_action(201);
    include dirname(__FILE__) . '/account/gmuser.php';
    exit;
}

if ($a === 'chmod')
{
    check_action(201);
    include dirname(__FILE__) . '/account/chmod.php';
    exit;
}

$actionlist = isset($_SESSION['actionlist']) ? $_SESSION['actionlist'] : array();

$groups = array();
foreach ($actionlist as $id => $action)
{
    if ($action['pid'] == 0)
    {
        $groups[$id] = array('name' => $action['name'], 'children' => array());
    }
}
foreach ($actionlist as $id => $action)
{
    if ($action['pid'] != 0 && isset($groups[$action['pid']]) && isset($action['page']))
    {
        $groups[$action['pid']]['children'][$id] = $action;
    }
}
?>
<html>
<head>
<meta charset="UTF-8">
<title>NGL Admin</title>
<style>
body { font-family: Arial, sans-serif; background: #f0f2f5; margin: 0; }
.header { background: #001529; color: #fff; padding: 12px 24px; display: flex; justify-content: space-between; align-items: center; }
.header .title { font-size: 18px; font-weight: bold; }
.header .user a { color: #69c0ff; text-decoration: none; }
.container { max-width: 960px; margin: 24px auto; display: flex; flex-wrap: wrap; gap: 16px; padding: 0 16px; }
.module { background: #fff; border-radius: 6px; padding: 16px 20px; box-shadow: 0 1px 4px rgba(0,0,0,0.08); min-width: 200px; flex: 1; }
.module h3 { margin: 0 0 12px; color: #333; font-size: 15px; border-bottom: 1px solid #f0f0f0; padding-bottom: 8px; }
.module a { display: block; color: #1890ff; text-decoration: none; padding: 4px 0; font-size: 14px; }
.module a:hover { color: #40a9ff; }
</style>
</head>
<body>
<div class="header">
    <span class="title">NGL Admin</span>
    <span class="user"><?php echo htmlspecialchars($_SESSION['admin_username']); ?> | <a href="logout.php">退出</a></span>
</div>
<div class="container">
<?php foreach ($groups as $gid => $group): ?>
<?php if (!empty($group['children'])): ?>
<div class="module">
    <h3><?php echo htmlspecialchars($group['name']); ?></h3>
    <?php foreach ($group['children'] as $aid => $act): ?>
    <a href="<?php echo htmlspecialchars($act['page']); ?>"><?php echo htmlspecialchars($act['name']); ?></a>
    <?php endforeach; ?>
</div>
<?php endif; ?>
<?php endforeach; ?>
</div>
</body>
</html>
