using System;
using System.IO;
using System.Net;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Threading;
using System.Xml;
using System.Xml.Linq;
using ngl;
using Pbnet;
using System.Collections.Generic;
using Pbdb;

namespace ngl
{
    class ThreadCreationProgram
    {
        static void Main(string[] args)
        {
            if (Int32.TryParse(args[3], out Int32 luport) == false)
                return;
       
            NglKcp.getInstance().Create(luport);
            
            ManageCsv<tab_servers>.Load("C:\\Users\\Administrator\\Documents\\GitHub\\ngl\\bin\\csv\\tab_servers.csv");
            xmlprotocol.Load("C:\\Users\\Administrator\\Documents\\GitHub\\ngl\\bin\\Debug\\config\\net_protocol.xml");
            NConfig.Load("C:\\Users\\Administrator\\Documents\\GitHub\\ngl\\bin\\config\\config.template.xml");

            if (Int32.TryParse(args[1], out Int32 larea) == false)
                return;
            if (Int32.TryParse(args[2], out Int32 ltcount) == false)
                return;
            var tab = ttab_servers.Tab(args[0], larea, ltcount);
            if (tab == null)
                return;
            NConfig.SetServer(args[0], tab.Id());

            var ltcp = new Tcp();

            ProtocolPack pp = new ProtocolPack();

            pp.Registry<PROBUFF_NET_ACOUNT_LOGIN>(
                item =>
                Console.WriteLine("##[PROBUFF_NET_RECHARGE]##")
                );
            Int32 lvalue = 0;
            Int64 roleid = 0;
            Int32 lloginconnect = 0;
            Int32 lgatewayconnect = 0;
            pp.Registry<PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>(
                item =>
                {
                    Console.WriteLine($"{++lvalue}=>{item.MAccount}##{item.MArea}##{item.MSession}##{item.MGatewayid}");
                    roleid = item.MRoleid;
                    // 连接GateWay服务器
                    net_works? lnet = ttab_servers.NetProtocol(item.MGatewayid);
                    if (lnet == null)
                        return;
                    ltcp.m_connectSuccessful = (Tcp.TcpConnect aconnect) =>
                    {
                        // 登陆游戏
                        var pro = new PROBUFF_NET_ROLE_LOGIN();
                        pro.MSession = item.MSession;
                        pro.MGatewayid = item.MGatewayid;
                        pro.MArea = item.MArea;
                        pro.MRoleid = item.MRoleid;
                        pro.MIscreate = false;
                        ltcp.Send(aconnect.m_session, pro);
                    };
                    if (!IPAddress.TryParse(lnet.m_ip, out IPAddress? GatewayIPAddress))
                        return;
                    IPEndPoint GatewayIpPort = new IPEndPoint(GatewayIPAddress, lnet.m_port);
                    lgatewayconnect = ltcp.Connect(GatewayIpPort);
                }
                );

            NglKcp.reconnect = () =>
            {
                var pro = new PROBUFF_NET_KCPSESSION();
                var tab = ttab_servers.Tab();
                if (tab == null)
                    return;
                var tabgame = ttab_servers.Tab("game", tab.m_area, 1);
                if (tabgame == null)
                    return;
                pro.MServerid = tabgame.m_id;
                pro.MUport = luport;
                pro.MUip = NglKcp.m_ip;
                pro.MConv = (int)NglKcp.conv;
                ltcp.Send(lgatewayconnect, pro);
            };

            var lconnectgame = () =>
            {
                var tab = ttab_servers.Tab();
                if (tab == null)
                    return null;
                var tabgame = ttab_servers.Tab("game", tab.m_area, 1);
                if (tabgame == null)
                    return null;
                var lnet = ttab_servers.NetProtocol(tabgame.m_id, ENET_PROTOCOL.ENET_KCP);
                if (lnet == null)
                    return null;
                if (!IPAddress.TryParse(lnet.m_ip, out IPAddress? kcpIPAddress))
                    return null;
                IPEndPoint kcpIpPort = new IPEndPoint(kcpIPAddress, lnet.m_port);
                return kcpIpPort;
            };

            pp.Registry<PROBUFF_NET_ROLE_SYNC_RESPONSE>(
               item =>
               {
                   NglKcp.getInstance().get_localip(lconnectgame());
                   NglKcp.reconnect();
               }
               );

            string lMKcpsession = "";
            var lconnect = () =>
            {
               
                NglKcp.getInstance().Connect(lconnectgame(), roleid, lMKcpsession);
            };
            pp.Registry<PROBUFF_NET_KCPSESSION_RESPONSE>(
                item =>
                {
                    lMKcpsession = item.MKcpsession;
                    // 连接GAME udp服务器
                    lconnect();
                }
                );

            pp.Registry<PROBUFF_NET_GET_TIME_RESPONSE>(
               item =>
               {
                   DateTime dt = DateTime.SpecifyKind(new DateTime(1970, 1, 1).AddSeconds(item.MUtc), DateTimeKind.Utc);
                   string str = dt.ToString("yyyy/MM/dd dddd HH:mm:ss");
                   Console.WriteLine($"{str}");
               }
               );


            ltcp.m_connectSuccessful = (Tcp.TcpConnect aconnect) =>
            {
                PROBUFF_NET_ACOUNT_LOGIN pro = new PROBUFF_NET_ACOUNT_LOGIN();
                pro.MAccount = args[4];
                pro.MArea = 1;
                pro.MPassword = "123456";
                ltcp.Send(aconnect.m_session, pro);
            };
            ltcp.m_connectFail = () => Console.WriteLine("##[connectFail]##");



            ltcp.SetRegistry(pp);
            NglKcp.getInstance().SetRegistry(pp);

            // 连接服务器
            //IPAddress lIPAddress = null;
            //if (!IPAddress.TryParse("152.136.107.233", out lIPAddress))
            if (!IPAddress.TryParse("127.0.0.1", out IPAddress? lIPAddress))
                return;
            IPEndPoint _IpPort = new IPEndPoint(lIPAddress, 10006);
            lloginconnect = ltcp.Connect(_IpPort);
            Thread.Sleep(5000);

            

            Thread t1 = new Thread(() =>
            {
                while (true)
                {
                    Thread.Sleep(500);
                    ltcp.ReceiveAllPack();
                    NglKcp.getInstance().ReceiveAllPack();
                }
            });
            t1.Start();

           

            while (true)
            {
                // 等待输入命令测试udp消息通信
                char zx = Console.ReadKey().KeyChar;
                Console.WriteLine($"##[{zx}]##");
                if (zx == '1')
                {
                    NglKcp.getInstance().Close();
                    NglKcp.reconnect();
                }
                else
                {
                    var protm = new PROBUFF_NET_GET_TIME();
                    NglKcp.getInstance().Send(protm);
                }
              



                Thread.Sleep(500);
            }
        }
    }
}