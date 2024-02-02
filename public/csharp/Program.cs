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

namespace ngl
{
    class ThreadCreationProgram
    {
        static void Main(string[] args)
        {
            xmlprotocol.load("C:\\net_protocol.xml");

            var ltcp = new tcp();

            protocol_pack pp = new protocol_pack();

            pp.registry<PROBUFF_NET_ACOUNT_LOGIN>(
                item =>
                Console.WriteLine("##[PROBUFF_NET_RECHARGE]##")
                );
            pp.registry<PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>(
                item =>
                Console.WriteLine("##[PROBUFF_NET_ACOUNT_LOGIN_RESPONSE]##")
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
                    ltcp.receive_pack();
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