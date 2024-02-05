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
            pp.registry<PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>(
                item =>
                {
                    Console.WriteLine($"{++lvalue}=>{item.MAccount}##{item.MArea}##{item.MSession}##{item.MGatewayid}");
                }
                );

            ltcp.m_connectSuccessful = () =>
            {
                PROBUFF_NET_ACOUNT_LOGIN pro = new PROBUFF_NET_ACOUNT_LOGIN();
                pro.MAccount = "libo1";
                pro.MArea = 1;
                pro.MPassword = "123456";
                ltcp.send(pro);
            };
            ltcp.m_connectFail = () => Console.WriteLine("##[connectFail]##");



            ltcp.set_registry(pp);

            // 连接服务器
            //IPAddress lIPAddress = null;
            //if (!IPAddress.TryParse("152.136.107.233", out lIPAddress))
            if (!IPAddress.TryParse("127.0.0.1", out IPAddress lIPAddress))
                return;
            IPEndPoint _IpPort = new IPEndPoint(lIPAddress, 10006);
            ltcp.connect(_IpPort);
            Thread.Sleep(5000);



            Thread t1 = new Thread(() =>
            {
                while (true)
                {
                    Thread.Sleep(500);
                    ltcp.receive_allpack();
                }
            });
            t1.Start();

            while (true)
            {
                Thread.Sleep(500);
            }
        }
    }
}