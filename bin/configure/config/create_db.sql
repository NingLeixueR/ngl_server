/*Date:2026-02-28 19:07:27*/

 DROP Database IF EXISTS `lbtest`;
 CREATE DATABASE lbtest default charset utf8 COLLATE utf8_general_ci;
 use lbtest;

-- ----------------------------
-- Table structure for db_account
-- ----------------------------
DROP TABLE IF EXISTS `db_account`;
CREATE TABLE `db_account` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_brief
-- ----------------------------
DROP TABLE IF EXISTS `db_brief`;
CREATE TABLE `db_brief` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_role
-- ----------------------------
DROP TABLE IF EXISTS `db_role`;
CREATE TABLE `db_role` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_bag
-- ----------------------------
DROP TABLE IF EXISTS `db_bag`;
CREATE TABLE `db_bag` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_keyvalue
-- ----------------------------
DROP TABLE IF EXISTS `db_keyvalue`;
CREATE TABLE `db_keyvalue` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_rolekeyvalue
-- ----------------------------
DROP TABLE IF EXISTS `db_rolekeyvalue`;
CREATE TABLE `db_rolekeyvalue` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_mail
-- ----------------------------
DROP TABLE IF EXISTS `db_mail`;
CREATE TABLE `db_mail` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_notice
-- ----------------------------
DROP TABLE IF EXISTS `db_notice`;
CREATE TABLE `db_notice` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_ranklist
-- ----------------------------
DROP TABLE IF EXISTS `db_ranklist`;
CREATE TABLE `db_ranklist` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_activitytimes
-- ----------------------------
DROP TABLE IF EXISTS `db_activitytimes`;
CREATE TABLE `db_activitytimes` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_activity
-- ----------------------------
DROP TABLE IF EXISTS `db_activity`;
CREATE TABLE `db_activity` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_task
-- ----------------------------
DROP TABLE IF EXISTS `db_task`;
CREATE TABLE `db_task` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_familyer
-- ----------------------------
DROP TABLE IF EXISTS `db_familyer`;
CREATE TABLE `db_familyer` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_family
-- ----------------------------
DROP TABLE IF EXISTS `db_family`;
CREATE TABLE `db_family` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_friends
-- ----------------------------
DROP TABLE IF EXISTS `db_friends`;
CREATE TABLE `db_friends` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
-- ----------------------------
-- Table structure for db_testlua
-- ----------------------------
DROP TABLE IF EXISTS `db_testlua`;
CREATE TABLE `db_testlua` (
    `id` bigint(63) NOT NULL,
    `area` int(11) NOT NULL,
    `data` LongBlob,
    PRIMARY KEY (`id`)
)ENGINE = InnoDB DEFAULT CHARSET = utf8; 
