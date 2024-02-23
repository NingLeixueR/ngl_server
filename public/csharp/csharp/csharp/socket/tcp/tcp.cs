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
using System.Collections;
using System.Security.Cryptography;
using System.Xml.Linq;

namespace ngl
{
    class tcp
    {
        public class tcp_connect
        {
            public IPEndPoint? m_endpoint = null;
            public Socket? m_socket = null;
            public byte[] m_temp = new byte[8192];
            public int m_session = 0;
        }

        private Dictionary<int, tcp_connect>? m_tcp = null;
        private int m_gsesson = 0;
        //  连接成功的回调函数
        public Action<tcp_connect>? m_connectSuccessful = null;
        //  连接失败的回调函数
        public Action? m_connectFail = null;

        //byte[] m_temp = new byte[8192];

        private bool m_noDelay;

        private pack? m_pack = null;
        private List<pack> m_packlist = new List<pack>();
        private protocol_pack? m_propack = null;
        public tcp()
        {
            m_tcp = new Dictionary<int, tcp_connect>();
        }
        Socket? get_socket(int asession)
        {
            if (m_tcp == null)
                return null;
            if (m_tcp.TryGetValue(asession, out tcp_connect so))
                return so.m_socket;
            return null;
        }

        tcp_connect? get_tcp_connect(int asession)
        {
            if (m_tcp == null)
                return null;
            if (m_tcp.TryGetValue(asession, out tcp_connect so))
                return so;
            return null;
        }
        public void set_nodelay(int asession, bool anodelay)
        {
            var socket = get_socket(asession);
            if (socket == null)
                return;
            if (m_noDelay != anodelay)
            {
                m_noDelay = anodelay;
                socket.SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.NoDelay, m_noDelay);
            }
        }

        public bool get_nodelay()
        {
            return m_noDelay;
        }

        public int connect(IPEndPoint aendpoint)
        {
            if (m_tcp == null)
                return -1;
            try
            {
                var ltcp = new tcp_connect
                {
                    m_endpoint = aendpoint,
                    m_socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp),
                    m_session = ++m_gsesson,
                };
                m_tcp.Add(m_gsesson, ltcp);
                ltcp.m_socket.NoDelay = true;
                ltcp.m_socket.BeginConnect(aendpoint, (IAsyncResult result) => { on_connect(result, ltcp); }, null);
                return m_gsesson;
            }
            catch (System.Exception ex)
            {
                // Debug.LogWarning (ex);
            }
            return -1;
        }

        private void on_connect(IAsyncResult result, tcp_connect aconnect)
        {
            if (aconnect.m_socket == null)
                return;
            try
            {
                aconnect.m_socket.EndConnect(result);
            }
            catch (System.Exception ex)
            {
                aconnect.m_socket.Close();
                if (m_connectFail != null)
                    m_connectFail();
                return;
            }
            start_receiving(aconnect);
            if(m_connectSuccessful != null)
                m_connectSuccessful(aconnect);
        }

        private void start_receiving(tcp_connect aconnect)
        {
            if (aconnect.m_socket != null && aconnect.m_socket.Connected)
            {
                try
                {
                    //IPEndPoint remoteTcpEndPoint = (IPEndPoint)m_socket.RemoteEndPoint;
                    aconnect.m_socket.BeginReceive(aconnect.m_temp, 0, aconnect.m_temp.Length, SocketFlags.None, (IAsyncResult result) => { on_receive(result, aconnect); }, null);
                }
                catch (System.Exception ex)
                {
                    //if (!(ex is SocketException)) Debug.LogWarning(ex);
                    close(aconnect.m_session);
                    if (m_connectFail != null)
                        m_connectFail();
                    return;
                }
            }
        }

        public void close(int asession)
        {
            var socket = get_socket(asession);
            if (socket == null)
                return;
            if (socket != null)
            {
                try
                {
                    if (socket.Connected)
                        socket.Shutdown(SocketShutdown.Both);
                    socket.Close();
                }
                catch (System.Exception) 
                { 
                }
                socket = null;
            }
        }

        private void on_receive(IAsyncResult result, tcp_connect aconnect)
        {
            if (aconnect.m_socket == null)
                return;
            int bytes = 0;
            try
            {
                bytes = aconnect.m_socket.EndReceive(result);
            }
            catch (System.Exception ex)
            {
                close(aconnect.m_session);
                if (m_connectFail != null)
                    m_connectFail();
                return;
            }
            if (bytes > 0 && process_buffer(bytes, aconnect))
            {
                try
                {
                    aconnect.m_socket.BeginReceive(aconnect.m_temp, 0, aconnect.m_temp.Length, SocketFlags.None, (IAsyncResult result) => { on_receive(result, aconnect); }, null);
                    return;
                }
                catch (System.Exception ex)
                {
                }

            }
            close(aconnect.m_session);
            if (m_connectFail != null)
                m_connectFail();
        }

        private bool process_buffer(int bytes, tcp_connect aconnect)
        {
            // 收取包头
            tcp_buff lbuff = new tcp_buff();
            lbuff.m_buff = aconnect.m_temp;
            lbuff.m_len = bytes;
            lbuff.m_pos = 0;

            if (m_pack == null)
                m_pack = new pack();

            
            while (lbuff.m_pos < lbuff.m_len)
            {
                EPH_HEAD_VAL lval = m_pack.push_buff(lbuff);
                if (lval == EPH_HEAD_VAL.EPH_HEAD_SUCCESS)
                {
                    lock (m_packlist)
                    {
                        m_packlist.Add(m_pack);
                    }
                    m_pack = new pack();
                    continue;
                }
                else if (lval == EPH_HEAD_VAL.EPH_HEAD_VERSION_FAIL)
                {
                    close(aconnect.m_session);
                }
                break;
            }            
            return true;
        }

        public void receive_allpack()
        {
            if (m_propack == null)
                return;
            List<pack> list = new List<pack>();
            lock (m_packlist)
            {
                if (m_packlist.Count <= 0)
                    return;
                list.AddRange(m_packlist);
                m_packlist.Clear();
            }
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

        public void send<T>(int asession, T apro) where T : IMessage, new()
        {
            var ltcp_connect = get_tcp_connect(asession);
            if (ltcp_connect == null || ltcp_connect.m_socket == null)
                return;
            byte[] lbuff = apro.ToByteArray();
            pack_head lhead = new pack_head();
            lhead.bytes = lbuff.Length;
            lhead.time = utc();
            lhead.version = nconfig.m_head_version;
            lhead.protocolnum = xmlprotocol.protocol(apro.Descriptor.Name);
            lhead.protocoltype = (Int32)EPROTOCOL_TYPE.EPROTOCOL_TYPE_PROTOCOLBUFF;
            lhead.actorid = -1;
            lhead.request_actorid = -1;

            tcp_buff lbuffall = new tcp_buff();
            lbuffall.m_buff = new byte[pack_head.packheadbyte + lbuff.Length];
            lhead.buff.CopyTo(lbuffall.m_buff, 0);
            encryption.bytexor(lbuff, lbuff.Length, 0);
            lbuff.CopyTo(lbuffall.m_buff, pack_head.packheadbyte);

            ltcp_connect.m_socket.BeginSend(lbuffall.m_buff, 0, pack_head.packheadbyte + lbuff.Length, SocketFlags.None, (IAsyncResult result) => { on_send(result, ltcp_connect); }, lbuffall);
        }

        private void on_send(IAsyncResult result, tcp_connect atcp_connect)
        {
            if (atcp_connect.m_socket == null)
                return;
            int bytes = atcp_connect.m_socket.EndSend(result);
            tcp_buff? buff = result.AsyncState as tcp_buff;
            if (buff == null || buff.m_buff == null)
                return;
            if (bytes < buff.m_len)
            {
                try
                {
                    buff.m_pos += bytes;
                    atcp_connect.m_socket.BeginSend(buff.m_buff, buff.m_pos, buff.m_len, SocketFlags.None, (IAsyncResult result) => { on_send(result, atcp_connect); }, buff);
                    return;
                }
                catch (Exception ex)
                {
                    //Debug.LogWarning (ex);
                    close(atcp_connect.m_session);
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


    public class protocol_pack
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