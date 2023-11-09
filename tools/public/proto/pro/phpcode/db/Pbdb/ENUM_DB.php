<?php
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: db.proto

namespace Pbdb;

use UnexpectedValueException;

/**
 * Protobuf type <code>pbdb.ENUM_DB</code>
 */
class ENUM_DB
{
    /**
     * 账号数据
     *
     * Generated from protobuf enum <code>ENUM_DB_ACCOUNT = 0;</code>
     */
    const ENUM_DB_ACCOUNT = 0;
    /**
     * 玩家数据
     *
     * Generated from protobuf enum <code>ENUM_DB_ROLE = 1;</code>
     */
    const ENUM_DB_ROLE = 1;
    /**
     * 玩家简要数据
     *
     * Generated from protobuf enum <code>ENUM_DB_BRIEF = 2;</code>
     */
    const ENUM_DB_BRIEF = 2;
    /**
     * 背包数据
     *
     * Generated from protobuf enum <code>ENUM_DB_BAG = 3;</code>
     */
    const ENUM_DB_BAG = 3;
    /**
     * 键值数据
     *
     * Generated from protobuf enum <code>ENUM_DB_KEYVALUE = 4;</code>
     */
    const ENUM_DB_KEYVALUE = 4;
    /**
     * 邮件数据
     *
     * Generated from protobuf enum <code>ENUM_DB_MAIL = 5;</code>
     */
    const ENUM_DB_MAIL = 5;
    /**
     * 工会数据
     *
     * Generated from protobuf enum <code>ENUM_DB_GUILD = 6;</code>
     */
    const ENUM_DB_GUILD = 6;
    /**
     * 公告数据
     *
     * Generated from protobuf enum <code>ENUM_DB_NOTICE = 7;</code>
     */
    const ENUM_DB_NOTICE = 7;
    /**
     * 排行数据
     *
     * Generated from protobuf enum <code>ENUM_DB_RANKLIST = 8;</code>
     */
    const ENUM_DB_RANKLIST = 8;
    /**
     * 活动数据
     *
     * Generated from protobuf enum <code>ENUM_DB_ACTIVITY = 9;</code>
     */
    const ENUM_DB_ACTIVITY = 9;
    /**
     * Generated from protobuf enum <code>ENUM_DB_COUNT = 10;</code>
     */
    const ENUM_DB_COUNT = 10;

    private static $valueToName = [
        self::ENUM_DB_ACCOUNT => 'ENUM_DB_ACCOUNT',
        self::ENUM_DB_ROLE => 'ENUM_DB_ROLE',
        self::ENUM_DB_BRIEF => 'ENUM_DB_BRIEF',
        self::ENUM_DB_BAG => 'ENUM_DB_BAG',
        self::ENUM_DB_KEYVALUE => 'ENUM_DB_KEYVALUE',
        self::ENUM_DB_MAIL => 'ENUM_DB_MAIL',
        self::ENUM_DB_GUILD => 'ENUM_DB_GUILD',
        self::ENUM_DB_NOTICE => 'ENUM_DB_NOTICE',
        self::ENUM_DB_RANKLIST => 'ENUM_DB_RANKLIST',
        self::ENUM_DB_ACTIVITY => 'ENUM_DB_ACTIVITY',
        self::ENUM_DB_COUNT => 'ENUM_DB_COUNT',
    ];

    public static function name($value)
    {
        if (!isset(self::$valueToName[$value])) {
            throw new UnexpectedValueException(sprintf(
                    'Enum %s has no name defined for value %s', __CLASS__, $value));
        }
        return self::$valueToName[$value];
    }


    public static function value($name)
    {
        $const = __CLASS__ . '::' . strtoupper($name);
        if (!defined($const)) {
            throw new UnexpectedValueException(sprintf(
                    'Enum %s has no value defined for name %s', __CLASS__, $name));
        }
        return constant($const);
    }
}

