<?php
require_once __DIR__ . '/auth.php';
check_login();

$modules = array(
    'ban'         => array('name' => '封号管理',     'pages' => array('roleban_html.php' => '封号', 'get_roleban_html.php' => '查询封号')),
    'bantalk'     => array('name' => '禁言管理',     'pages' => array('bantalk_html.php' => '禁言')),
    'db'          => array('name' => '数据库查询',   'pages' => array('db_html.php' => '查询', 'dball_html.php' => '分页查询', 'changedb_html.php' => '修改数据')),
    'family'      => array('name' => '家族管理',     'pages' => array('get_family_html.php' => '查询家族', 'changname_family_html.php' => '改名')),
    'guid'        => array('name' => 'GUID查询',     'pages' => array('get_guid_html.php' => '获取GUID')),
    'mail'        => array('name' => '邮件管理',     'pages' => array('add_mail_html.php' => '发送邮件', 'get_mail_html.php' => '查询邮件', 'del_mail_html.php' => '删除邮件')),
    'notice'      => array('name' => '公告管理',     'pages' => array('add_notice_html.php' => '添加公告', 'get_notice_html.php' => '查询公告', 'del_notice_html.php' => '删除公告')),
    'openserver'  => array('name' => '开服管理',     'pages' => array('set_openserver_html.php' => '设置开服', 'get_openserver_html.php' => '查询开服')),
    'pay'         => array('name' => '充值管理',     'pages' => array('gmpay_html.php' => 'GM充值')),
    'protocol'    => array('name' => '协议查询',     'pages' => array('get_allprotocol_html.php' => '全部协议')),
    'ranklist'    => array('name' => '排行榜',       'pages' => array('get_ranklist_html.php' => '查询排行')),
    'server_stat' => array('name' => '服务器状态',   'pages' => array('server_stat_html.php' => '查看状态')),
    'sys'         => array('name' => '系统操作',     'pages' => array('close_actor_html.php' => '关闭Actor')),
    'time'        => array('name' => '时间管理',     'pages' => array('get_time_html.php' => '查询时间', 'set_time_html.php' => '设置时间')),
);
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
<?php foreach ($modules as $key => $mod): ?>
<?php if (check_permission($key)): ?>
<div class="module">
    <h3><?php echo $mod['name']; ?></h3>
    <?php foreach ($mod['pages'] as $file => $label): ?>
    <a href="<?php echo $key . '/' . $file; ?>"><?php echo $label; ?></a>
    <?php endforeach; ?>
</div>
<?php endif; ?>
<?php endforeach; ?>
</div>
</body>
</html>
