CREATE TABLE IF NOT EXISTS db_admin (
  id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(32) NOT NULL UNIQUE,
  password VARCHAR(64) NOT NULL,
  permissions VARCHAR(512) NOT NULL DEFAULT '*',
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- permissions: 逗号分隔的功能模块名, '*' 表示全部权限
-- 可选值: ban,bantalk,db,family,guid,mail,notice,openserver,pay,protocol,ranklist,server_stat,sys,sysmail,time
INSERT INTO db_admin (username, password, permissions) VALUES ('admin', MD5('admin123'), '*');
