<?php
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: db.proto

namespace Pbdb\db_activity\data;

use Google\Protobuf\Internal\GPBType;
use Google\Protobuf\Internal\RepeatedField;
use Google\Protobuf\Internal\GPBUtil;

/**
 * tab_activity.m_type == EActivityDrawCompliance
 *
 * Generated from protobuf message <code>pbdb.db_activity.data.drawcompliance</code>
 */
class drawcompliance extends \Google\Protobuf\Internal\Message
{
    /**
     * 累计次数
     *
     * Generated from protobuf field <code>optional int32 m_count = 1;</code>
     */
    protected $m_count = null;
    /**
     * 累计领取哪些奖励
     *
     * Generated from protobuf field <code>repeated int32 m_reward = 2;</code>
     */
    private $m_reward;

    /**
     * Constructor.
     *
     * @param array $data {
     *     Optional. Data for populating the Message object.
     *
     *     @type int $m_count
     *           累计次数
     *     @type array<int>|\Google\Protobuf\Internal\RepeatedField $m_reward
     *           累计领取哪些奖励
     * }
     */
    public function __construct($data = NULL) {
        \GPBMetadata\Db::initOnce();
        parent::__construct($data);
    }

    /**
     * 累计次数
     *
     * Generated from protobuf field <code>optional int32 m_count = 1;</code>
     * @return int
     */
    public function getMCount()
    {
        return isset($this->m_count) ? $this->m_count : 0;
    }

    public function hasMCount()
    {
        return isset($this->m_count);
    }

    public function clearMCount()
    {
        unset($this->m_count);
    }

    /**
     * 累计次数
     *
     * Generated from protobuf field <code>optional int32 m_count = 1;</code>
     * @param int $var
     * @return $this
     */
    public function setMCount($var)
    {
        GPBUtil::checkInt32($var);
        $this->m_count = $var;

        return $this;
    }

    /**
     * 累计领取哪些奖励
     *
     * Generated from protobuf field <code>repeated int32 m_reward = 2;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getMReward()
    {
        return $this->m_reward;
    }

    /**
     * 累计领取哪些奖励
     *
     * Generated from protobuf field <code>repeated int32 m_reward = 2;</code>
     * @param array<int>|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setMReward($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::INT32);
        $this->m_reward = $arr;

        return $this;
    }

}

// Adding a class alias for backwards compatibility with the previous class name.
class_alias(drawcompliance::class, \Pbdb\db_activity_data_drawcompliance::class);

