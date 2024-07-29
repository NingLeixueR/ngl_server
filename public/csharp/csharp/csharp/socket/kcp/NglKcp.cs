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

        private Pack? m_pack = null;
        private List<Pack> m_packlist = new List<Pack>();
        private ProtocolPack? m_propack = null;
        private int m_port = 0;
        IPEndPoint? m_endpoint = null;
        private CancellationTokenSource? m_cancel = null;

        private udp_cmd? cmd = null;

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
                        await Task.Delay(NConfig.m_kcp_ping*1000);
                        cmd.sendcmd(udp_cmd.ecmd.ecmd_ping, "");
                    }
                }, m_cancel.Token);
            });
        }

        public static string? m_ip = null;

        public void get_localip(IPEndPoint aendpoint)
        {
            if (m_cancel != null && m_ip == null)
                return;
            kcpClient = null;
            kcpClient = new SimpleKcpClient();
            kcpClient.start_udp(m_port);
            var buffer = System.Text.Encoding.UTF8.GetBytes("GetIp");
            bool isconnect = true;
            var lfun = async () =>
            {
                while (isconnect)
                {
                    kcpClient.UdpSend(buffer, aendpoint);
                    await Task.Delay(1000);
                }
            };
            lfun();
            byte[]? lbuff = kcpClient.UdpRecv();
            if (lbuff == null)
                return;
            m_ip = System.Text.Encoding.UTF8.GetString(lbuff);
            isconnect = false;
        }

        public void Connect(IPEndPoint aendpoint, Int64 aactorid, string asession)
        {
            if (kcpClient == null)
                return;
            kcpClient.start_udp(m_port);

            m_cancel = new CancellationTokenSource();
            kcpClient.start_kcp(conv++, aendpoint);
            if (kcpClient.kcp == null)
                return;
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
                if (cmd == null)
                    return;
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
                        m_pack = new Pack();
                    EPH_HEAD_VAL lval = m_pack.PushBuff(ret);
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
            if (cmd == null)
                return;
            cmd.sendcmd(ecmd.ecmd_connect, JsonConvert.SerializeObject(
                new { actorid = aactorid.ToString(), session = asession }
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
            byte[] lbuff = apro.ToByteArray();
            PackHead lhead = new PackHead();
            lhead.Bytes = lbuff.Length;
            lhead.Time = utc();
            lhead.Mask1 = 0xffffff;
            lhead.Mask2 = 0xffffff;
            lhead.ProtocolNum = xmlprotocol.Protocol(apro.Descriptor.Name);
            lhead.ProtocolType = (Int32)EPROTOCOL_TYPE.EPROTOCOL_TYPE_PROTOCOLBUFF;
            lhead.ActorId = -1;
            lhead.RequestActorId = -1;

            tcp_buff lbuffall = new tcp_buff();
            lbuffall.m_buff = new byte[PackHead.PackHeadByte + lbuff.Length];
            lhead.Buff.CopyTo(lbuffall.m_buff, 0);
            encryption.bytexor(lbuff, lbuff.Length, 0);
            lbuff.CopyTo(lbuffall.m_buff, PackHead.PackHeadByte);

            kcpClient?.SendAsync(lbuffall.m_buff, PackHead.PackHeadByte + lbuff.Length);
        }
        public void SetRegistry(ProtocolPack apack)
        {
            m_propack = apack;
        }
        public void ReceiveAllPack()
        {
            if (m_propack == null)
                return;
            List<Pack> list = new List<Pack>();
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
            Pack? lpack = null;
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
