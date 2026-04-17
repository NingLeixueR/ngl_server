CREATE TABLE IF NOT EXISTS db_admin (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(32) NOT NULL UNIQUE,
  password VARCHAR(64) NOT NULL,
  permissions TEXT NOT NULL,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- permissions: "all" 表示全部权限, 或 JSON 数组存储允许的 action ID
-- 例: "all" 或 [1,101,102,103,3,301,302,8,801,802,803]
-- action ID 定义见 ActionManager.php
INSERT INTO db_admin (username, password, permissions) VALUES ('admin', MD5('admin123'), 'all');

-- ----------------------------
-- Table structure for `db_recharge`
-- ----------------------------
DROP TABLE IF EXISTS `db_recharge`;
CREATE TABLE `db_recharge` (
  `orderid` varchar(128) NOT NULL,
  `rechargeid` int(11) DEFAULT NULL,
  `createtime` datetime DEFAULT NULL,
  `gm` int(11) DEFAULT NULL,
  `stat` int(11) DEFAULT NULL,
  `roleid` bigint(63) DEFAULT NULL,
  PRIMARY KEY (`orderid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for `db_server`
-- ----------------------------
DROP TABLE IF EXISTS `db_server`;
CREATE TABLE `db_server` (
  `id` int(11) NOT NULL,
  `area` int(11) NOT NULL,
  `name` char(128) NOT NULL,
  `ip` char(128) NOT NULL,
  `nip` char(128) NOT NULL,
  `port` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of db_server
-- ----------------------------
INSERT INTO `db_server` VALUES ('1', '1', 'actor', '127.0.0.1', '127.0.0.1', '10000', '2');
INSERT INTO `db_server` VALUES ('2', '1', 'game', '127.0.0.1', '127.0.0.1', '10010', '3');
INSERT INTO `db_server` VALUES ('3', '1', 'db', '127.0.0.1', '127.0.0.1', '10020', '1');
INSERT INTO `db_server` VALUES ('4', '1', 'reloadcsv', '127.0.0.1', '127.0.0.1', '10030', '9');
INSERT INTO `db_server` VALUES ('5', '1', 'login', '127.0.0.1', '127.0.0.1', '10040', '5');
INSERT INTO `db_server` VALUES ('6', '1', 'gateway', '127.0.0.1', '127.0.0.1', '10050', '4');
INSERT INTO `db_server` VALUES ('7', '1', 'world', '127.0.0.1', '127.0.0.1', '10060', '7');
INSERT INTO `db_server` VALUES ('8', '1', 'log', '127.0.0.1', '127.0.0.1', '10070', '8');
INSERT INTO `db_server` VALUES ('9', '1', 'robot', '127.0.0.1', '127.0.0.1', '10080', '6');
INSERT INTO `db_server` VALUES ('10', '1', 'robot', '127.0.0.1', '127.0.0.1', '10081', '6');
INSERT INTO `db_server` VALUES ('11', '1', 'robot', '127.0.0.1', '127.0.0.1', '10082', '6');
INSERT INTO `db_server` VALUES ('12', '1', 'robot', '127.0.0.1', '127.0.0.1', '10083', '6');
INSERT INTO `db_server` VALUES ('13', '1', 'robot', '127.0.0.1', '127.0.0.1', '10084', '6');
INSERT INTO `db_server` VALUES ('14', '1', 'robot', '127.0.0.1', '127.0.0.1', '10085', '6');
INSERT INTO `db_server` VALUES ('15', '1', 'pushserverconfig', '127.0.0.1', '127.0.0.1', '10090', '12');
INSERT INTO `db_server` VALUES ('21', '2', 'actor', '127.0.0.1', '127.0.0.1', '11000', '2');
INSERT INTO `db_server` VALUES ('22', '2', 'game', '127.0.0.1', '127.0.0.1', '11010', '3');
INSERT INTO `db_server` VALUES ('23', '2', 'db', '127.0.0.1', '127.0.0.1', '11020', '1');
INSERT INTO `db_server` VALUES ('24', '2', 'reloadcsv', '127.0.0.1', '127.0.0.1', '11030', '9');
INSERT INTO `db_server` VALUES ('25', '2', 'login', '127.0.0.1', '127.0.0.1', '11040', '5');
INSERT INTO `db_server` VALUES ('26', '2', 'gateway', '127.0.0.1', '127.0.0.1', '11050', '4');
INSERT INTO `db_server` VALUES ('27', '2', 'world', '127.0.0.1', '127.0.0.1', '11060', '7');
INSERT INTO `db_server` VALUES ('28', '2', 'log', '127.0.0.1', '127.0.0.1', '11070', '8');
INSERT INTO `db_server` VALUES ('29', '2', 'robot', '127.0.0.1', '127.0.0.1', '11080', '6');
INSERT INTO `db_server` VALUES ('101', '-100', 'cross', '127.0.0.1', '127.0.0.1', '20000', '10');
INSERT INTO `db_server` VALUES ('102', '-100', 'crossdb', '127.0.0.1', '127.0.0.1', '20010', '11');
