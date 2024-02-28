using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
//using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;
using Google.Protobuf;
using System.Text.RegularExpressions;
using System.Xml;
using System.Runtime.InteropServices;
using System.Net;
using Newtonsoft.Json;
using System.Threading;
using static ngl.udp_cmd;

namespace ngl
{
    class udp_cmd
    {
        public enum ecmd
        {
            ecmd_connect,               // 发起连接
            ecmd_connect_ret,           // 被发起连接者的返回
            ecmd_ping,                  // 定时ping
            ecmd_close,                 // 主动断开连接
            ecmd_close_ret,             // 主动断开连接的返回
            ecmd_minlen = 5,
        };

        byte[]? m_cmdbyte = null;
        Dictionary<ecmd, Action<string>>? m_cmdfun = null;
        Action<byte[]>? m_sendfun = null;

        public udp_cmd(Action<byte[]> afun)
        {
            m_cmdfun = new Dictionary<ecmd, Action<string>>();
            m_cmdbyte = System.Text.Encoding.UTF8.GetBytes("ecmd*");
            m_sendfun = afun;
        }

        public void sendcmd(ecmd acmd, string ajson)
        {
            if (m_sendfun == null)
                return;
            string lcmd = "ecmd*";
            lcmd += ((int)acmd).ToString();
            lcmd += "*";
            lcmd += ajson;
            var buffer = System.Text.Encoding.UTF8.GetBytes(lcmd);
            m_sendfun(buffer);
            //kcpClient.SendAsync(buffer, buffer.Length);
        }

        static public bool CheckByteEquals(byte[] b1, byte[] b2, int alen)
        {
            if (b1.Length < alen || b2.Length < alen)
                return false;
            for (int i = 0; i < alen; i++)
            {
                if (b1[i] != b2[i])
                    return false;
            }
            return true;
        }

        public bool cmd(byte[] abuf)
        {
            if (abuf.Length < (int)ecmd.ecmd_minlen)
                return false;
            if (m_cmdbyte == null || m_cmdfun == null)
                return false;
            if (CheckByteEquals(abuf, m_cmdbyte, (int)ecmd.ecmd_minlen) == false)
                return false;
            var str = System.Text.Encoding.UTF8.GetString(abuf);
            string[] words = str.Split('*');
            if (words.Length < 3)
                return true;
            if (Int32.TryParse(words[1], out Int32 lcmd) == false)
                return true;
            if (m_cmdfun.TryGetValue((ecmd)lcmd, out Action<string>? lfun) == false)
                return true;
            lfun(words[2]);
            return true;
        }

        public void register_fun(ecmd anum, Action<string> afun)
		{
            m_cmdfun?.Add(anum, afun);
		}

}

    public class NglKcp
    {
        SimpleKcpClient? kcpClient = null;

        private pack? m_pack = null;
        private List<pack> m_packlist = new List<pack>();
        private ProtocolPack? m_propack = null;
        private int m_port = 0;
        IPEndPoint? m_endpoint = null;
        private CancellationTokenSource? m_cancel = null;

        private udp_cmd cmd;

        public static uint conv = 1;
        // 重新建立连接
        public static Action? reconnect = null;

        static NglKcp m_instance = new NglKcp();

        public static NglKcp getInstance()
        {
            return m_instance;
        }

        public void Create(int port)
        {
            m_port = port;
            cmd = new udp_cmd((byte[] abuff) =>
            {
                if (kcpClient == null)
                    return;
                kcpClient.SendAsync(abuff, abuff.Length);
            });
            cmd.register_fun(udp_cmd.ecmd.ecmd_connect_ret, (string ajson) =>
            {
                // 定时ecmd ping 
                if (m_cancel == null)
                    return;
                Task.Factory.StartNew(async () =>
                {
                    while (true)
                    {
                        await Task.Delay(nconfig.m_kcp_ping);
                        cmd.sendcmd(udp_cmd.ecmd.ecmd_ping, "");
                    }
                }, m_cancel.Token);
            });
        }

        public void Connect(IPEndPoint aendpoint, Int64 aactorid, string asession)
        {
            if (m_cancel != null)
                return;
            kcpClient = null;
            m_cancel = new CancellationTokenSource();
            kcpClient = new SimpleKcpClient(conv++, m_port, aendpoint);
            kcpClient.kcp.TraceListener = new ConsoleTraceListener();
            kcpClient.kcp.NoDelay(1, 10, 2, 1);//fast
            kcpClient.kcp.WndSize(128, 128);
            kcpClient.kcp.SetMtu(512);

            Task.Factory.StartNew(async () =>
            {
                while (true)
                {
                    kcpClient.kcp.Update(DateTimeOffset.UtcNow);
                    await Task.Delay(10);
                }
            }, m_cancel.Token);

            Task.Factory.StartNew(async () =>
            {
                while (true)
                {
                    var res = await kcpClient.ReceiveAsync();
                    if (res == null)
                        continue;

                    if (cmd.cmd(res) == true)
                        continue;

                    tcp_buff ret = new tcp_buff();
                    ret.m_buff = res;
                    ret.m_len = res.Length;
                    ret.m_pos = 0;

                    if (m_pack == null)
                        m_pack = new pack();
                    EPH_HEAD_VAL lval = m_pack.push_buff(ret);
                    if (lval == EPH_HEAD_VAL.EPH_HEAD_SUCCESS)
                    {
                        lock (m_packlist)
                        {
                            m_packlist.Add(m_pack);
                        }
                        m_pack = null;
                        continue;
                    }
                }
            }, m_cancel.Token);

            kcpClient.EndPoint = aendpoint;
            m_endpoint = aendpoint;
            cmd.sendcmd(ecmd.ecmd_connect, JsonConvert.SerializeObject(
                new { actorid = aactorid, session = asession }
                ));
        }

        public void Close()
        {
            if (kcpClient != null)
                kcpClient.close();
            if (m_cancel != null)
                m_cancel.Cancel();
            m_cancel = null;
        }

        private Int32 utc()
        {
            System.DateTime startTime = TimeZone.CurrentTimeZone.ToLocalTime(new System.DateTime(1970, 1, 1));
            return (Int32)(DateTime.UtcNow - startTime).TotalSeconds;
        }

        public void Send<T>(T apro) where T : IMessage, new()
        {
            //if (m_socket == null)
            //    return;
            byte[] lbuff = apro.ToByteArray();
            pack_head lhead = new pack_head();
            lhead.bytes = lbuff.Length;
            lhead.time = utc();
            lhead.version = nconfig.m_head_version;
            lhead.protocolnum = xmlprotocol.Protocol(apro.Descriptor.Name);
            lhead.protocoltype = (Int32)EPROTOCOL_TYPE.EPROTOCOL_TYPE_PROTOCOLBUFF;
            lhead.actorid = -1;
            lhead.request_actorid = -1;

            tcp_buff lbuffall = new tcp_buff();
            lbuffall.m_buff = new byte[pack_head.packheadbyte + lbuff.Length];
            lhead.buff.CopyTo(lbuffall.m_buff, 0);
            encryption.bytexor(lbuff, lbuff.Length, 0);
            lbuff.CopyTo(lbuffall.m_buff, pack_head.packheadbyte);

            kcpClient?.SendAsync(lbuffall.m_buff, pack_head.packheadbyte + lbuff.Length);
            ////m_socket.BeginSend(lbuffall.m_buff, 0, pack_head.packheadbyte + lbuff.Length, SocketFlags.None, on_send, lbuffall);
        }
        public void SetRegistry(ProtocolPack apack)
        {
            m_propack = apack;
        }
        public void ReceiveAllPack()
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
            list.ForEach(itempack => m_propack.LogicFun(itempack));
            list.Clear();
        }

        public void ReceivePack()
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
                m_propack.LogicFun(lpack);
        }

    }
}
