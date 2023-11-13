<?php
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: gm.proto

namespace GM;

use Google\Protobuf\Internal\GPBType;
use Google\Protobuf\Internal\RepeatedField;
use Google\Protobuf\Internal\GPBUtil;

/**
 * Generated from protobuf message <code>GM.gm_notice</code>
 */
class gm_notice extends \Google\Protobuf\Internal\Message
{
    /**
     * 公告id
     *
     * Generated from protobuf field <code>optional int64 m_id = 1;</code>
     */
    protected $m_id = null;
    /**
     * Generated from protobuf field <code>optional string m_notice = 2;</code>
     */
    protected $m_notice = null;
    /**
     * 开始时间
     *
     * Generated from protobuf field <code>optional int32 m_starttime = 3;</code>
     */
    protected $m_starttime = null;
    /**
     * 结束时间
     *
     * Generated from protobuf field <code>optional int32 m_finishtime = 4;</code>
     */
    protected $m_finishtime = null;

    /**
     * Constructor.
     *
     * @param array $data {
     *     Optional. Data for populating the Message object.
     *
     *     @type int|string $m_id
     *           公告id
     *     @type string $m_notice
     *     @type int $m_starttime
     *           开始时间
     *     @type int $m_finishtime
     *           结束时间
     * }
     */
    public function __construct($data = NULL) {
        \GPBMetadata\Gm::initOnce();
        parent::__construct($data);
    }

    /**
     * 公告id
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
     * 公告id
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
     * Generated from protobuf field <code>optional string m_notice = 2;</code>
     * @return string
     */
    public function getMNotice()
    {
        return isset($this->m_notice) ? $this->m_notice : '';
    }

    public function hasMNotice()
    {
        return isset($this->m_notice);
    }

    public function clearMNotice()
    {
        unset($this->m_notice);
    }

    /**
     * Generated from protobuf field <code>optional string m_notice = 2;</code>
     * @param string $var
     * @return $this
     */
    public function setMNotice($var)
    {
        GPBUtil::checkString($var, True);
        $this->m_notice = $var;

        return $this;
    }

    /**
     * 开始时间
     *
     * Generated from protobuf field <code>optional int32 m_starttime = 3;</code>
     * @return int
     */
    public function getMStarttime()
    {
        return isset($this->m_starttime) ? $this->m_starttime : 0;
    }

    public function hasMStarttime()
    {
        return isset($this->m_starttime);
    }

    public function clearMStarttime()
    {
        unset($this->m_starttime);
    }

    /**
     * 开始时间
     *
     * Generated from protobuf field <code>optional int32 m_starttime = 3;</code>
     * @param int $var
     * @return $this
     */
    public function setMStarttime($var)
    {
        GPBUtil::checkInt32($var);
        $this->m_starttime = $var;

        return $this;
    }

    /**
     * 结束时间
     *
     * Generated from protobuf field <code>optional int32 m_finishtime = 4;</code>
     * @return int
     */
    public function getMFinishtime()
    {
        return isset($this->m_finishtime) ? $this->m_finishtime : 0;
    }

    public function hasMFinishtime()
    {
        return isset($this->m_finishtime);
    }

    public function clearMFinishtime()
    {
        unset($this->m_finishtime);
    }

    /**
     * 结束时间
     *
     * Generated from protobuf field <code>optional int32 m_finishtime = 4;</code>
     * @param int $var
     * @return $this
     */
    public function setMFinishtime($var)
    {
        GPBUtil::checkInt32($var);
        $this->m_finishtime = $var;

        return $this;
    }

}
