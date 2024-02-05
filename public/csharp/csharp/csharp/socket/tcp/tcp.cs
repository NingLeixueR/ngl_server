using System;
using System.IO;
using System.Net.Sockets;
using System.Collections.Generic;
using System.Net;
using System.Threading;
using Google.Protobuf;
using System.Text.RegularExpressions;
using System.Xml;
using System.Text;
using System.Timers;
using static Pbdb.db_activity.Types;
using ProtoBuf;
using Google.Protobuf.Compiler;
using Pbnet;

namespace ngl
{

    class tcp
    {
        IPEndPoint? m_endpoint = null;
        Socket? m_socket = null;
        //  连接成功的回调函数
        public Action? m_connectSuccessful = null;
        //  连接失败的回调函数
        public Action? m_connectFail = null;
         
        byte[] m_temp = new byte[8192];

        private bool m_noDelay;

        private pack? m_pack = null;
        private List<pack> m_packlist = new List<pack>();
        private protocol_pack? m_propack = null;

        public void set_nodelay(bool anodelay)
        {
            if(m_socket != null && m_noDelay != anodelay)
            {
                m_noDelay = anodelay;
                m_socket.SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.NoDelay, m_noDelay);
            }
        }

        public bool get_nodelay()
        {
            return m_noDelay;
        }

        public bool connect(IPEndPoint aendpoint)
        {
            if (m_socket != null || m_endpoint != null || aendpoint == null)
                return false;
            m_endpoint = aendpoint;
            try
            {
                m_socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                m_socket.NoDelay = true;
                m_socket.BeginConnect(m_endpoint, on_connect, null);
                return true;
            }
            catch (System.Exception ex)
            {
                // Debug.LogWarning (ex);
            }
            return false;
        }

        private void on_connect(IAsyncResult result)
        {
            if (m_socket == null)
                return;
            try
            {
                m_socket.EndConnect(result);
            }
            catch (System.Exception ex)
            {
                m_socket.Close();
                if (m_connectFail != null)
                    m_connectFail();
                return;
            }
            start_receiving();
            if(m_connectSuccessful != null)
                m_connectSuccessful();
        }

        private void start_receiving()
        {
            if (m_socket != null && m_socket.Connected)
            {
                try
                {
                    //IPEndPoint remoteTcpEndPoint = (IPEndPoint)m_socket.RemoteEndPoint;
                    m_socket.BeginReceive(m_temp, 0, m_temp.Length, SocketFlags.None, on_receive, null);
                }
                catch (System.Exception ex)
                {
                    //if (!(ex is SocketException)) Debug.LogWarning(ex);
                    close();
                    if (m_connectFail != null)
                        m_connectFail();
                    return;
                }
            }
        }

        public void close()
        {
            if (m_socket != null)
            {
                try
                {
                    if (m_socket.Connected)
                        m_socket.Shutdown(SocketShutdown.Both);
                    m_socket.Close();
                }
                catch (System.Exception) 
                { 
                }
                m_socket = null;
            }
        }

        private void on_receive(IAsyncResult result)
        {
            if (m_socket == null)
                return;
            int bytes = 0;
            try
            {
                bytes = m_socket.EndReceive(result);
            }
            catch (System.Exception ex)
            {
                close();
                if (m_connectFail != null)
                    m_connectFail();
                return;
            }
            if (bytes > 0 && process_buffer(bytes))
            {
                try
                {
                    m_socket.BeginReceive(m_temp, 0, m_temp.Length, SocketFlags.None, on_receive, null);
                    return;
                }
                catch (System.Exception ex)
                {
                }

            }
            close();
            if (m_connectFail != null)
                m_connectFail();
        }

        private bool process_buffer(int bytes)
        {
            // 收取包头
            tcp_buff lbuff = new tcp_buff();
            lbuff.m_buff = m_temp;
            lbuff.m_len = bytes;
            lbuff.m_pos = 0;

            if (m_pack == null)
                m_pack = new pack();
            while (m_pack.push_buff(lbuff))
            {
                lock (m_packlist)
                {
                    m_packlist.Add(m_pack);
                }
                m_pack = new pack();
            }
            return true;
        }

        public void receive_allpack()
        {
            if (m_propack == null)
                return;
            List<pack> list = new List<pack>();
            List<pack> temp = null;
            lock (m_packlist)
            {
                if (m_packlist.Count <= 0)
                    return;
                temp = m_packlist;
                m_packlist = list;
            }
            list = temp;
            list.ForEach(itempack => m_propack.logic_fun(itempack));
            list.Clear();
        }

        public void receive_pack()
        {
            pack? lpack = null;
            lock (m_packlist)
            {
                if (m_packlist.Count > 0)
                {
                    lpack = m_packlist[0];
                    m_packlist.RemoveAt(0);
                }
            }
            if (lpack != null && m_propack != null)
                m_propack.logic_fun(lpack);
        }

        public void set_registry(protocol_pack apack)
        {
            m_propack = apack;
        }

        public protocol_pack? get_registry()
        {
            return m_propack;
        }

        private Int32 utc()
        {
            System.DateTime startTime = TimeZone.CurrentTimeZone.ToLocalTime(new System.DateTime(1970, 1, 1));
            return (Int32)(DateTime.UtcNow - startTime).TotalSeconds;
        }
        public void send<T>(T apro) where T : IMessage, new()
        {
            if (m_socket == null)
                return;
            byte[] lbuff = apro.ToByteArray();
            pack_head lhead = new pack_head();
            lhead.bytes = lbuff.Length;
            lhead.time = utc();
            lhead.protocolnum = xmlprotocol.protocol(apro.Descriptor.Name);
            lhead.protocoltype = (Int32)EPROTOCOL_TYPE.EPROTOCOL_TYPE_PROTOCOLBUFF;
            lhead.actorid = -1;
            lhead.request_actorid = -1;

            tcp_buff lbuffall = new tcp_buff();
            lbuffall.m_buff = new byte[pack_head.packheadbyte + lbuff.Length];
            lhead.buff.CopyTo(lbuffall.m_buff, 0);
            encryption.bytexor(lbuff, lbuff.Length, 0);
            lbuff.CopyTo(lbuffall.m_buff, pack_head.packheadbyte);

            m_socket.BeginSend(lbuffall.m_buff, 0, pack_head.packheadbyte + lbuff.Length, SocketFlags.None, on_send, lbuffall);
        }


        private void on_send(IAsyncResult result)
        {
            if (m_socket == null)
                return;
            int bytes = m_socket.EndSend(result);
            tcp_buff? buff = result.AsyncState as tcp_buff;
            if (buff == null || buff.m_buff == null)
                return;
            if (bytes < buff.m_len)
            {
                try
                {
                    buff.m_pos += bytes;
                    m_socket.BeginSend(buff.m_buff, buff.m_pos, buff.m_len, SocketFlags.None, on_send, buff);
                    return;
                }
                catch (Exception ex)
                {
                    //Debug.LogWarning (ex);
                    close();
                    if (m_connectFail != null)
                        m_connectFail();
                    return;
                }
            }
        }
    }

    class xmlprotocol
    {
        private static Dictionary<string,Int32> m_protocol = new Dictionary<string, Int32>();

        public static void load(string apath)
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(apath);
            if (xmlDoc.DocumentElement == null)
                return;
            foreach (XmlNode node in xmlDoc.DocumentElement.ChildNodes)
            {
                if (node.Name == "config" && node.Attributes != null)
                {
                    string nameValue = nconfig.xmlgetString(node.Attributes, "name"); 
                    string[] result = Regex.Split(nameValue, "::");

                    Int32 lnumber = nconfig.xmlgetInt32(node.Attributes, "number");
                    Console.WriteLine($"type:number => {result[1]}:{lnumber}");
                    if(m_protocol.ContainsKey(result[1]) == false)
                        m_protocol.Add(result[1], lnumber);
                }
            }
        }

        // ### 获取协议号
        public static Int32 protocol(string aname)
        {
            if (m_protocol.TryGetValue(aname, out Int32 lproto) == false)
                return -1;
            return lproto;
        }
    }


    class protocol_pack
    {
        private Dictionary<Int32, Action<pack>> m_protocol = new Dictionary<Int32, Action<pack>>();

        public void registry<T>(Action<T> afun) where T : IMessage, new()
        {
            T lname = new T();
            Int32 lprotocol = xmlprotocol.protocol(lname.Descriptor.Name);
            m_protocol.Add(lprotocol, pack =>
            {
                if (pack.m_buff == null)
                    return;
                encryption.bytexor(pack.m_buff, pack.m_head.bytes, 0);

                T pro = new T();
                pro = (T)pro.Descriptor.Parser.ParseFrom(pack.m_buff);
                afun(pro);
            });
        }

        public void logic_fun(pack apack)
        {
            Action<pack>? lfun = null;
            if (m_protocol.TryGetValue(apack.m_head.protocolnum, out lfun) == false)
                return;
            lfun(apack);
        }
    }
}