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

namespace ngl
{
    class ThreadCreationProgram
    {
        static void Main(string[] args)
        {

            var kcptemp = new NglKcp(12345);
            

            manage_csv<tab_servers>.load("C:\\Users\\Administrator\\Documents\\GitHub\\ngl\\bin\\csv\\tab_servers.csv");
            xmlprotocol.load("C:\\Users\\Administrator\\Documents\\GitHub\\ngl\\bin\\Debug\\config\\net_protocol.xml");
            nconfig.load("C:\\Users\\Administrator\\Documents\\GitHub\\ngl\\bin\\config\\config.template.xml");

            if (Int32.TryParse(args[1], out Int32 larea) == false)
                return;
            if (Int32.TryParse(args[2], out Int32 ltcount) == false)
                return;
            var tab = ttab_servers.tab(args[0], larea, ltcount);
            if (tab == null)
                return;
            nconfig.set_server(args[0], tab.Id());

            var ltcp = new tcp();

            protocol_pack pp = new protocol_pack();

            pp.registry<PROBUFF_NET_ACOUNT_LOGIN>(
                item =>
                Console.WriteLine("##[PROBUFF_NET_RECHARGE]##")
                );
            Int32 lvalue = 0;
            Int64 roleid = 0;
            Int32 lloginconnect = 0;
            Int32 lgatewayconnect = 0;
            pp.registry<PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>(
                item =>
                {
                    Console.WriteLine($"{++lvalue}=>{item.MAccount}##{item.MArea}##{item.MSession}##{item.MGatewayid}");
                    roleid = item.MRoleid;
                    // 连接GateWay服务器
                    var tab = ttab_servers.tab(item.MGatewayid);
                    if (tab == null)
                        return;
                    ltcp.m_connectSuccessful = (tcp.tcp_connect aconnect) =>
                    {
                        // 登陆游戏
                        var pro = new PROBUFF_NET_ROLE_LOGIN();
                        pro.MSession = item.MSession;
                        pro.MGatewayid = item.MGatewayid;
                        pro.MArea = item.MArea;
                        pro.MRoleid = item.MRoleid;
                        pro.MIscreate = false;
                        ltcp.send(aconnect.m_session, pro);

                      
                    };
                    if (!IPAddress.TryParse(tab.m_ip, out IPAddress GatewayIPAddress))
                        return;
                    IPEndPoint GatewayIpPort = new IPEndPoint(GatewayIPAddress, tab.m_port);
                    lgatewayconnect = ltcp.connect(GatewayIpPort);
                }
                );

            pp.registry<PROBUFF_NET_ROLE_SYNC_RESPONSE>(
               item =>
               {
                   var pro = new PROBUFF_NET_KCPSESSION();
                   ltcp.send(lgatewayconnect, pro);
               }
               );

            pp.registry<PROBUFF_NET_KCPSESSION_RESPONSE>(
                item =>
                {
                    // 连接GAME udp服务器
                    var tab = ttab_servers.tab();
                    if (tab == null)
                        return;
                    var tabgame = ttab_servers.tab("game", tab.m_area, 1);
                    if (tab == null || tabgame == null)
                        return;
                    if (!IPAddress.TryParse(tabgame.m_ip, out IPAddress kcpIPAddress))
                        return;
                    IPEndPoint kcpIpPort = new IPEndPoint(kcpIPAddress, tabgame.m_uport);
                    kcptemp.connect(kcpIpPort, roleid, item.MKcpsession);
                }
                );

            pp.registry<PROBUFF_NET_GET_TIME_RESPONSE>(
               item =>
               {
                   DateTime dt = DateTime.SpecifyKind(new DateTime(1970, 1, 1).AddSeconds(item.MUtc), DateTimeKind.Utc);
                   string str = dt.ToString("yyyy/MM/dd dddd HH:mm:ss");
                   Console.WriteLine($"{str}");
               }
               );


            ltcp.m_connectSuccessful = (tcp.tcp_connect aconnect) =>
            {
                PROBUFF_NET_ACOUNT_LOGIN pro = new PROBUFF_NET_ACOUNT_LOGIN();
                pro.MAccount = "libo1";
                pro.MArea = 1;
                pro.MPassword = "123456";
                ltcp.send(aconnect.m_session, pro);
            };
            ltcp.m_connectFail = () => Console.WriteLine("##[connectFail]##");



            ltcp.set_registry(pp);
            kcptemp.set_registry(pp);

            // 连接服务器
            //IPAddress lIPAddress = null;
            //if (!IPAddress.TryParse("152.136.107.233", out lIPAddress))
            if (!IPAddress.TryParse("127.0.0.1", out IPAddress lIPAddress))
                return;
            IPEndPoint _IpPort = new IPEndPoint(lIPAddress, 10006);
            lloginconnect = ltcp.connect(_IpPort);
            Thread.Sleep(5000);

            

            Thread t1 = new Thread(() =>
            {
                while (true)
                {
                    Thread.Sleep(500);
                    ltcp.receive_allpack();
                    kcptemp.receive_allpack();
                }
            });
            t1.Start();

            // 等待输入命令测试udp消息通信
            char zx = Console.ReadKey().KeyChar;
            Console.WriteLine($"##[{zx}]##");
            var protm = new PROBUFF_NET_GET_TIME();
            kcptemp.send(protm);

            while (true)
            {
                

                Thread.Sleep(500);
            }
        }
    }
}