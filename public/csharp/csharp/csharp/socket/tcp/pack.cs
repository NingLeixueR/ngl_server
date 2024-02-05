using Google.Protobuf.Compiler;
using Pbdb;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ngl
{
    enum EPROTOCOL_TYPE
    {
        EPROTOCOL_TYPE_CUSTOM,      // 自定义二进制协议
        EPROTOCOL_TYPE_PROTOCOLBUFF,    // protobuff协议
        EPROTOCOL_TYPE_COUNT,
        EPROTOCOL_TYPE_ERROR,
    }
    public class tcp_pack
    {
        public Byte[]? m_buff = null;
        public int m_len = 0;
        public int m_pos = 0;
    }

    public class tcp_buff
    {
        public Byte[]? m_buff = null;
        public int m_len = 0;
        public int m_pos = 0;
    }

    //enum EPH
    //{
    //    EPH_BYTES = 0,                // 协议字节数
    //    EPH_TIME,                     // 发送端的时间戳
    //    EPH_PROTOCOLNUM,              // 协议号
    //    EPH_PROTOCOLTYPE,             // 协议类型 EPROTOCOL_TYPE
    //    EPH_ACTOR_TYPEAREA,           // ACTOR_TYPE_AREA
    //    EPH_ACTOR_ID,                 // ACTOR_ID
    //    EPH_REQUEST_ACTOR_TYPEAREA,   // Request REQUEST_ACTOR_TYPE_AREA
    //    EPH_REQUEST_ACTOR_ID,         // Request ACTOR_ID
    //    EPH_SUM,
    //}
    class pack_head
    {
        Byte[] m_data = new Byte[(Int32)EPH.EPH_SUM * sizeof(Int32)];
        Int32 m_pos = 0;

        public Int32 pos            //m_data 已接收数据
        {
            set { m_pos = value; }
            get { return m_pos; }
        }
        public Byte[] buff
        {
            get { return m_data; }
        }
        public pack_head()
        {
            for (int i = 0; i < (Int32)EPH.EPH_SUM * sizeof(Int32); ++i)
                m_data[i] = 0;
        }
        public static Int32 packheadbyte //包头长度
        {
            get { return (Int32)EPH.EPH_SUM * sizeof(Int32); }
        }
        void set_value(EPH aenum, Int32 avalue)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(avalue)
                , 0
                , m_data
                , (Int32)aenum * sizeof(Int32)
                , sizeof(Int32)
                );
        }
        Int32 get_value(EPH aenum)
        {
            return BitConverter.ToInt32(m_data, (Int32)aenum * sizeof(Int32));
        }
        void set_value64(EPH aenum, Int64 avalue)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(avalue)
                , 0
                , m_data
                , (Int32)aenum * sizeof(Int32)
                , sizeof(Int64)
                );
        }
        Int64 get_value64(EPH aenum)
        {
            return BitConverter.ToInt64(m_data, (Int32)aenum * sizeof(Int32));
        }
        //    EPH_VERSION = 0,	                // 协议版本号
        public Int32 version
        {
            get { return get_value(EPH.EPH_VERSION); }
            set { set_value(EPH.EPH_VERSION, value); }
        }

        //    EPH.EPH_BYTES,                    // 协议字节数
        public Int32 bytes
        {
            get { return get_value(EPH.EPH_BYTES); }
            set { set_value(EPH.EPH_BYTES, value); }
        }
        //    EPH.EPH_TIME,                     // 发送端的时间戳
        public Int32 time
        {
            get { return get_value(EPH.EPH_TIME); }
            set { set_value(EPH.EPH_TIME, value); }
        }
        //    EPH.EPH_PROTOCOLNUM,              // 协议号
        public Int32 protocolnum
        {
            get { return get_value(EPH.EPH_PROTOCOLNUM); }
            set { set_value(EPH.EPH_PROTOCOLNUM, value); }
        }
        //    EPH.EPH_PROTOCOLTYPE,             // 协议类型 EPROTOCOL_TYPE
        public Int32 protocoltype
        {
            get { return get_value(EPH.EPH_PROTOCOLTYPE); }
            set { set_value(EPH.EPH_PROTOCOLTYPE, value); }
        }
        //    EPH_ACTOR_TYPEAREA,           // ACTOR_TYPE_AREA
        //    EPH_ACTOR_ID,                 // ACTOR_ID
        public Int64 actorid
        {
            get { return get_value64(EPH.EPH_ACTOR_TYPEAREA); }
            set { set_value64(EPH.EPH_ACTOR_TYPEAREA, value); }
        }
        //    EPH_REQUEST_ACTOR_TYPEAREA,   // Request REQUEST_ACTOR_TYPE_AREA
        //    EPH_REQUEST_ACTOR_ID,         // Request ACTOR_ID
        public Int64 request_actorid
        {
            get { return get_value64(EPH.EPH_REQUEST_ACTOR_TYPEAREA); }
            set { set_value64(EPH.EPH_REQUEST_ACTOR_TYPEAREA, value); }
        }

        public EPH_HEAD_VAL isready()
        {
            EPH_HEAD_VAL lval = isversion();
            if (lval != EPH_HEAD_VAL.EPH_HEAD_VERSION_SUCCESS)
                return lval;
            return m_pos >= packheadbyte ? EPH_HEAD_VAL.EPH_HEAD_SUCCESS : EPH_HEAD_VAL.EPH_HEAD_FOLLOW;
        }

        static Int32 m_versionpos = ((Int32)EPH.EPH_VERSION + 1) * sizeof(Int32);
        EPH_HEAD_VAL isversion()
        {
            if (m_pos < m_versionpos)
            {
                return EPH_HEAD_VAL.EPH_HEAD_VERSION_FOLLOW;
            }
            return version == nconfig.m_head_version ? EPH_HEAD_VAL.EPH_HEAD_VERSION_SUCCESS : EPH_HEAD_VAL.EPH_HEAD_VERSION_FAIL;
        }

        public EPH_HEAD_VAL push_buff(tcp_buff abuff)
        {
            if (abuff.m_buff == null)
                return EPH_HEAD_VAL.EPH_HEAD_VERSION_FAIL;
            EPH_HEAD_VAL lval = isready();
            if (lval == EPH_HEAD_VAL.EPH_HEAD_VERSION_FOLLOW || lval == EPH_HEAD_VAL.EPH_HEAD_FOLLOW)
            {
                int ltemp = packheadbyte - m_pos;
                int lpos = abuff.m_len - abuff.m_pos;
                ltemp = (lpos < ltemp) ? lpos : ltemp;

                Buffer.BlockCopy(abuff.m_buff, abuff.m_pos, m_data, m_pos, ltemp);
                m_pos += ltemp;
                abuff.m_pos += ltemp;
                return isready();
            }
            return lval;
        }
        //public bool push_buff(tcp_buff abuff)
        //{
        //    if (abuff.m_buff == null)
        //        return false;
        //    Int32 lrel = abuff.m_len - abuff.m_pos;
        //    if (lrel <= 0)
        //        return false;
        //    Int32 lhavebyte = pack_head.packheadbyte - m_pos;
        //    if (lhavebyte <= 0)
        //        return false;
        //    if (lhavebyte <= lrel)
        //    {
        //        Buffer.BlockCopy(abuff.m_buff, abuff.m_pos, m_data, m_pos, lhavebyte);
        //        m_pos += lhavebyte;
        //        abuff.m_pos += lhavebyte;
        //        return true;
        //    }
        //    else
        //    {
        //        Buffer.BlockCopy(abuff.m_buff, abuff.m_pos, m_data, m_pos, lrel);
        //        m_pos += lrel;
        //        abuff.m_pos = abuff.m_len;
        //        return false;
        //    }
        //}
    }

    class pack
    {
        public Int32 m_session = 0;
        public pack_head m_head = new pack_head();

        public byte[]? m_buff = null;
        public Int32 m_pos = 0;

        public bool isready()
        {
            return m_head.isready() == EPH_HEAD_VAL.EPH_HEAD_SUCCESS ? m_pos >= m_head.bytes /*+ EPH_SUM * sizeof(uint32_t)*/ : false;
        }

        public EPH_HEAD_VAL push_buff(tcp_buff abuff)
        {
            if (abuff.m_buff == null)//不可恢复的错误需要断线
                return EPH_HEAD_VAL.EPH_HEAD_VERSION_FAIL;

            EPH_HEAD_VAL lval = m_head.isready();
            if (lval == EPH_HEAD_VAL.EPH_HEAD_VERSION_FOLLOW)
            {////需要补全包头
                m_head.push_buff(abuff);
            }

            if (isready())
                return EPH_HEAD_VAL.EPH_HEAD_SUCCESS;

            if (m_buff == null)
            {
                m_buff = new byte[m_head.bytes];
                m_pos = 0;
            }

            //// 说明包头已经收到
            Int32 haveByte = m_head.bytes - m_pos;
            Int32 lrel = abuff.m_len - abuff.m_pos;
            if (haveByte <= lrel)
            {
                Buffer.BlockCopy(abuff.m_buff, abuff.m_pos, m_buff, m_pos, haveByte);
                m_pos += haveByte;
                abuff.m_pos += haveByte;
                return EPH_HEAD_VAL.EPH_HEAD_SUCCESS;
            }
            else
            {
                Buffer.BlockCopy(abuff.m_buff, abuff.m_pos, m_buff, m_pos, lrel);
                m_pos += lrel;
                abuff.m_pos = abuff.m_len;
                return EPH_HEAD_VAL.EPH_HEAD_FOLLOW;
            }
        }
    }
}
