<?php
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: db.proto

namespace Pbdb;

use Google\Protobuf\Internal\GPBType;
use Google\Protobuf\Internal\RepeatedField;
use Google\Protobuf\Internal\GPBUtil;

/**
 * Generated from protobuf message <code>pbdb.db_task</code>
 */
class db_task extends \Google\Protobuf\Internal\Message
{
    /**
     * roleid
     *
     * Generated from protobuf field <code>optional int64 m_id = 1;</code>
     */
    protected $m_id = null;
    /**
     * 任务数据
     *
     * Generated from protobuf field <code>repeated .pbdb.db_task.data m_datas = 2;</code>
     */
    private $m_datas;

    /**
     * Constructor.
     *
     * @param array $data {
     *     Optional. Data for populating the Message object.
     *
     *     @type int|string $m_id
     *           roleid
     *     @type array<\Pbdb\db_task\data>|\Google\Protobuf\Internal\RepeatedField $m_datas
     *           任务数据
     * }
     */
    public function __construct($data = NULL) {
        \GPBMetadata\Db::initOnce();
        parent::__construct($data);
    }

    /**
     * roleid
     *
     * Generated from protobuf field <code>optional int64 m_id = 1;</code>
     * @return int|string
     */
    public function getMId()
    {
        return isset($this->m_id) ? $this->m_id : 0;
    }

    public function hasMId()
    {
        return isset($this->m_id);
    }

    public function clearMId()
    {
        unset($this->m_id);
    }

    /**
     * roleid
     *
     * Generated from protobuf field <code>optional int64 m_id = 1;</code>
     * @param int|string $var
     * @return $this
     */
    public function setMId($var)
    {
        GPBUtil::checkInt64($var);
        $this->m_id = $var;

        return $this;
    }

    /**
     * 任务数据
     *
     * Generated from protobuf field <code>repeated .pbdb.db_task.data m_datas = 2;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getMDatas()
    {
        return $this->m_datas;
    }

    /**
     * 任务数据
     *
     * Generated from protobuf field <code>repeated .pbdb.db_task.data m_datas = 2;</code>
     * @param array<\Pbdb\db_task\data>|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setMDatas($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::MESSAGE, \Pbdb\db_task\data::class);
        $this->m_datas = $arr;

        return $this;
    }

}
