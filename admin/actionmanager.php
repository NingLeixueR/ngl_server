<?php
class ActionManager
{
    public static $ActionList = array(
        1 => array('name' => '我的账号', 'pid' => 0),
            101 => array('name' => '修改密码',     'pid' => 1, 'page' => 'index.php?a=chpass'),
            102 => array('name' => '退出登录',     'pid' => 1, 'page' => 'logout.php'),
            103 => array('name' => '刷新功能列表', 'pid' => 1, 'page' => 'index.php?a=updatelist'),

        2 => array('name' => '账号管理', 'pid' => 0),
            201 => array('name' => 'GM账号管理', 'pid' => 2, 'page' => 'index.php?a=gmuser'),

        3 => array('name' => '封号管理', 'pid' => 0),
            301 => array('name' => '封号',     'pid' => 3, 'page' => 'ban/roleban_html.php'),
            302 => array('name' => '查询封号', 'pid' => 3, 'page' => 'ban/get_roleban_html.php'),

        4 => array('name' => '禁言管理', 'pid' => 0),
            401 => array('name' => '禁言', 'pid' => 4, 'page' => 'bantalk/bantalk_html.php'),

        5 => array('name' => '数据库查询', 'pid' => 0),
            501 => array('name' => '查询',     'pid' => 5, 'page' => 'db/db_html.php'),
            502 => array('name' => '分页查询', 'pid' => 5, 'page' => 'db/dball_html.php'),
            503 => array('name' => '修改数据', 'pid' => 5, 'page' => 'db/changedb_html.php'),

        6 => array('name' => '家族管理', 'pid' => 0),
            601 => array('name' => '查询家族', 'pid' => 6, 'page' => 'family/get_family_html.php'),
            602 => array('name' => '家族改名', 'pid' => 6, 'page' => 'family/changname_family_html.php'),

        7 => array('name' => 'GUID查询', 'pid' => 0),
            701 => array('name' => '获取GUID', 'pid' => 7, 'page' => 'guid/get_guid_html.php'),

        8 => array('name' => '邮件管理', 'pid' => 0),
            801 => array('name' => '发送邮件', 'pid' => 8, 'page' => 'mail/add_mail_html.php'),
            802 => array('name' => '查询邮件', 'pid' => 8, 'page' => 'mail/get_mail_html.php'),
            803 => array('name' => '删除邮件', 'pid' => 8, 'page' => 'mail/del_mail_html.php'),

        9 => array('name' => '公告管理', 'pid' => 0),
            901 => array('name' => '添加公告', 'pid' => 9, 'page' => 'notice/add_notice_html.php'),
            902 => array('name' => '查询公告', 'pid' => 9, 'page' => 'notice/get_notice_html.php'),
            903 => array('name' => '删除公告', 'pid' => 9, 'page' => 'notice/del_notice_html.php'),

        10 => array('name' => '开服管理', 'pid' => 0),
            1001 => array('name' => '设置开服', 'pid' => 10, 'page' => 'openserver/set_openserver_html.php'),
            1002 => array('name' => '查询开服', 'pid' => 10, 'page' => 'openserver/get_openserver_html.php'),

        11 => array('name' => '充值管理', 'pid' => 0),
            1101 => array('name' => 'GM充值', 'pid' => 11, 'page' => 'pay/gmpay_html.php'),

        12 => array('name' => '协议查询', 'pid' => 0),
            1201 => array('name' => '全部协议', 'pid' => 12, 'page' => 'protocol/get_allprotocol_html.php'),

        13 => array('name' => '排行榜', 'pid' => 0),
            1301 => array('name' => '查询排行', 'pid' => 13, 'page' => 'ranklist/get_ranklist_html.php'),

        14 => array('name' => '服务器状态', 'pid' => 0),
            1401 => array('name' => '查看状态', 'pid' => 14, 'page' => 'server_stat/server_stat_html.php'),

        15 => array('name' => '系统操作', 'pid' => 0),
            1501 => array('name' => '关闭Actor', 'pid' => 15, 'page' => 'sys/close_actor_html.php'),

        16 => array('name' => '时间管理', 'pid' => 0),
            1601 => array('name' => '查询时间', 'pid' => 16, 'page' => 'time/get_time_html.php'),
            1602 => array('name' => '设置时间', 'pid' => 16, 'page' => 'time/set_time_html.php'),

        17 => array('name' => '操作日志', 'pid' => 0),
            1701 => array('name' => '查看日志', 'pid' => 17, 'page' => 'log/gmlog_html.php'),
    );

    public static function getAllowActionIDList($id)
    {
        require_once dirname(__FILE__) . '/config.php';
        $con = mysql_connect(DB_IP . ':' . DB_PORT, DB_USER, DB_PASS);
        if (!$con) return array();
        mysql_select_db(GMSYS, $con);
        mysql_query("set names 'utf8'");
        $result = mysql_query("SELECT permissions FROM db_admin WHERE id=" . intval($id));
        if (!$result) return array();
        $row = mysql_fetch_assoc($result);
        if (!$row) return array();
        if ($row['permissions'] === 'all')
        {
            return array_keys(self::$ActionList);
        }
        $ids = json_decode($row['permissions'], true);
        if (!is_array($ids)) return array();
        return $ids;
    }

    public static function getAllowActionList($id)
    {
        $allow = self::getAllowActionIDList($id);
        $rst = array();
        foreach (self::$ActionList as $actionid => $content)
        {
            if (self::isAllowActionImpl($actionid, $allow))
            {
                $rst[$actionid] = $content;
            }
        }
        return $rst;
    }

    public static function isAllowAction($id, $actionid)
    {
        $allow = self::getAllowActionIDList($id);
        return self::isAllowActionImpl($actionid, $allow);
    }

    private static function isAllowActionImpl($actionid, $allow)
    {
        while (true)
        {
            $action = isset(self::$ActionList[$actionid]) ? self::$ActionList[$actionid] : null;
            if (!$action) return false;
            if (!in_array($actionid, $allow)) return false;
            if ($action['pid'] == 0) return true;
            $actionid = $action['pid'];
        }
    }

    public static function findActionByPage($page)
    {
        foreach (self::$ActionList as $id => $action)
        {
            if (isset($action['page']) && $action['page'] === $page)
            {
                return $id;
            }
        }
        return 0;
    }
}
