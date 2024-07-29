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
    public class PackHead
    {
        Byte[] m_data = new Byte[(Int32)EPH.EPH_SUM * sizeof(Int32)];
        Int32 m_pos = 0;

        public Int32 Pos            //m_data 已接收数据
        {
            set { m_pos = value; }
            get { return m_pos; }
        }
        public Byte[] Buff
        {
            get { return m_data; }
        }
        public PackHead()
        {
            for (int i = 0; i < (Int32)EPH.EPH_SUM * sizeof(Int32); ++i)
                m_data[i] = 0;
        }
        public static Int32 PackHeadByte //包头长度
        {
            get { return (Int32)EPH.EPH_SUM * sizeof(Int32); }
        }
        void SetValue(EPH aenum, Int32 avalue)
        {
            Buffer.BlockCopy(
                BitConverter.GetBytes(avalue), 0, m_data, (Int32)aenum * sizeof(Int32), sizeof(Int32)
            );
        }
        void SetUValue(EPH aenum, UInt32 avalue)
        {
            Buffer.BlockCopy(
                BitConverter.GetBytes(avalue), 0, m_data, (Int32)aenum * sizeof(Int32), sizeof(Int32)
            );
        }
        Int32 GetValue(EPH aenum)
        {
            return BitConverter.ToInt32(m_data, (Int32)aenum * sizeof(Int32));
        }
        UInt32 GetUValue(EPH aenum)
        {
            return BitConverter.ToUInt32(m_data, (Int32)aenum * sizeof(Int32));
        }

        void SetValue64(EPH aenum, Int64 avalue)
        {
            Buffer.BlockCopy(
                BitConverter.GetBytes(avalue), 0, m_data, (Int32)aenum * sizeof(Int32), sizeof(Int64)
            );
        }
        Int64 GetValue64(EPH aenum)
        {
            return BitConverter.ToInt64(m_data, (Int32)aenum * sizeof(Int32));
        }

        //    EPH.EPH_BYTES,                    // 协议字节数
        public Int32 Bytes
        {
            get { return GetValue(EPH.EPH_BYTES); }
            set { SetValue(EPH.EPH_BYTES, value); }
        }
        //    EPH.EPH_TIME,                     // 发送端的时间戳
        public Int32 Time
        {
            get { return GetValue(EPH.EPH_TIME); }
            set { SetValue(EPH.EPH_TIME, value); }
        }

        //    EPH_MASK = 0,	                    // 用于确认是否使用包
        public UInt32 Mask1
        {
            get { return GetUValue(EPH.EPH_MASK); }
            set { SetUValue(EPH.EPH_MASK, value); }
        }

        public UInt32 Mask2
        {
            get { return GetUValue(EPH.EPH_MASK+1); }
            set { SetUValue(EPH.EPH_MASK+1, value); }
        }

        //    EPH.EPH_PROTOCOLNUM,              // 协议号
        public Int32 ProtocolNum
        {
            get { return GetValue(EPH.EPH_PROTOCOLNUM); }
            set { SetValue(EPH.EPH_PROTOCOLNUM, value); }
        }
        //    EPH.EPH_PROTOCOLTYPE,             // 协议类型 EPROTOCOL_TYPE
        public Int32 ProtocolType
        {
            get { return GetValue(EPH.EPH_PROTOCOLTYPE); }
            set { SetValue(EPH.EPH_PROTOCOLTYPE, value); }
        }
        //    EPH_ACTOR_TYPEAREA,           // ACTOR_TYPE_AREA
        //    EPH_ACTOR_ID,                 // ACTOR_ID
        public Int64 ActorId
        {
            get { return GetValue64(EPH.EPH_ACTOR_TYPEAREA); }
            set { SetValue64(EPH.EPH_ACTOR_TYPEAREA, value); }
        }
        //    EPH_REQUEST_ACTOR_TYPEAREA,   // Request REQUEST_ACTOR_TYPE_AREA
        //    EPH_REQUEST_ACTOR_ID,         // Request ACTOR_ID
        public Int64 RequestActorId
        {
            get { return GetValue64(EPH.EPH_REQUEST_ACTOR_TYPEAREA); }
            set { SetValue64(EPH.EPH_REQUEST_ACTOR_TYPEAREA, value); }
        }

        public EPH_HEAD_VAL IsReady()
        {
            EPH_HEAD_VAL lval = IsMask();
            if (lval != EPH_HEAD_VAL.EPH_HEAD_MASK_SUCCESS)
                return lval;
            return m_pos >= PackHeadByte ? EPH_HEAD_VAL.EPH_HEAD_SUCCESS : EPH_HEAD_VAL.EPH_HEAD_FOLLOW;
        }

        static Int32 m_versionpos = ((Int32)EPH.EPH_MASK + 1) * sizeof(Int32);
        public EPH_HEAD_VAL IsMask()
        {
            if (m_pos < m_versionpos)
            {
                return EPH_HEAD_VAL.EPH_HEAD_MASK_SUCCESS;
            }

            UInt32 lmask1 = GetUValue(EPH.EPH_MASK);
            UInt32 lmask2 = GetUValue(EPH.EPH_MASK + 1);
            if (lmask1 == 0xffffffff && lmask2 == 0xffffffff)
            {
                return EPH_HEAD_VAL.EPH_HEAD_MASK_SUCCESS;
            }
            return EPH_HEAD_VAL.EPH_HEAD_MASK_FAIL;
        }

        public EPH_HEAD_VAL PushBuff(tcp_buff abuff)
        {
            if (abuff.m_buff == null)
                return EPH_HEAD_VAL.EPH_HEAD_MASK_FAIL;
            EPH_HEAD_VAL lval = IsReady();
            if (lval == EPH_HEAD_VAL.EPH_HEAD_FOLLOW)
            {
                int ltemp = PackHeadByte - m_pos;
                int lpos = abuff.m_len - abuff.m_pos;
                ltemp = (lpos < ltemp) ? lpos : ltemp;

                Buffer.BlockCopy(abuff.m_buff, abuff.m_pos, m_data, m_pos, ltemp);
                m_pos += ltemp;
                abuff.m_pos += ltemp;
                return IsReady();
            }
            return lval;
        }
    }

    public class Pack
    {
        public Int32 m_session = 0;
        public PackHead m_head = new PackHead();

        public byte[]? m_buff = null;
        public Int32 m_pos = 0;

        public bool IsReady()
        {
            return m_head.IsReady() == EPH_HEAD_VAL.EPH_HEAD_SUCCESS ? m_pos >= m_head.Bytes /*+ EPH_SUM * sizeof(uint32_t)*/ : false;
        }

        public EPH_HEAD_VAL PushBuff(tcp_buff abuff)
        {
            if (abuff.m_buff == null)//不可恢复的错误需要断线
                return EPH_HEAD_VAL.EPH_HEAD_MASK_FAIL;

            EPH_HEAD_VAL lval = m_head.IsReady();
            if (lval == EPH_HEAD_VAL.EPH_HEAD_FOLLOW)
            {////需要补全包头
                m_head.PushBuff(abuff);
            }

            if (IsReady())
                return EPH_HEAD_VAL.EPH_HEAD_SUCCESS;

            if (m_buff == null)
            {
                m_buff = new byte[m_head.Bytes];
                m_pos = 0;
            }

            //// 说明包头已经收到
            Int32 haveByte = m_head.Bytes - m_pos;
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
