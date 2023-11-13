<?php
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: db.proto

namespace Pbdb;

use Google\Protobuf\Internal\GPBType;
use Google\Protobuf\Internal\RepeatedField;
use Google\Protobuf\Internal\GPBUtil;

/**
 *ENUM_DB_MAIL,			// 邮件数据
 *
 * Generated from protobuf message <code>pbdb.mailitem</code>
 */
class mailitem extends \Google\Protobuf\Internal\Message
{
    /**
     * Generated from protobuf field <code>optional int32 m_itemtid = 1;</code>
     */
    protected $m_itemtid = null;
    /**
     * Generated from protobuf field <code>optional int32 m_count = 2;</code>
     */
    protected $m_count = null;

    /**
     * Constructor.
     *
     * @param array $data {
     *     Optional. Data for populating the Message object.
     *
     *     @type int $m_itemtid
     *     @type int $m_count
     * }
     */
    public function __construct($data = NULL) {
        \GPBMetadata\Db::initOnce();
        parent::__construct($data);
    }

    /**
     * Generated from protobuf field <code>optional int32 m_itemtid = 1;</code>
     * @return int
     */
    public function getMItemtid()
    {
        return isset($this->m_itemtid) ? $this->m_itemtid : 0;
    }

    public function hasMItemtid()
    {
        return isset($this->m_itemtid);
    }

    public function clearMItemtid()
    {
        unset($this->m_itemtid);
    }

    /**
     * Generated from protobuf field <code>optional int32 m_itemtid = 1;</code>
     * @param int $var
     * @return $this
     */
    public function setMItemtid($var)
    {
        GPBUtil::checkInt32($var);
        $this->m_itemtid = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>optional int32 m_count = 2;</code>
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
     * Generated from protobuf field <code>optional int32 m_count = 2;</code>
     * @param int $var
     * @return $this
     */
    public function setMCount($var)
    {
        GPBUtil::checkInt32($var);
        $this->m_count = $var;

        return $this;
    }

}
