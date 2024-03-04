using System;
using System.Buffers;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Net;

#if NETSTANDARD2_0_OR_GREATER || NET5_0_OR_GREATER

namespace ngl
{
    /// <summary>
    /// 简单例子
    /// </summary>
    public class SimpleKcpClient : IKcpCallback
    {
        UdpClient? client;
        public void close()
        {
            //kcp.Dispose();
            client?.Close();
            client = null;
        }


        public SimpleKcpClient()
        {}

        public void start_udp(int port)
        {
            if(client == null)
                client = new UdpClient(port);
        }

        public void start_kcp(uint conv_, IPEndPoint endPoint)
        {
            kcp = new SimpleSegManager.Kcp(conv_, this);
            this.EndPoint = endPoint;
            BeginRecv();
        }

        public SimpleSegManager.Kcp? kcp { get; set; } = null;
        public IPEndPoint? EndPoint { get; set; } = null;

        public void Output(IMemoryOwner<byte> buffer, int avalidLength)
        {
            var s = buffer.Memory.Span.Slice(0, avalidLength).ToArray();
            client?.SendAsync(s, s.Length, EndPoint);
            buffer.Dispose();
        }

        public void UdpSend(byte[] buff, IPEndPoint aEndPoint)
        {
            client?.Send(buff, buff.Length, aEndPoint);
        }

        public byte[]? UdpRecv()
        {
            if (client == null)
                return null;
            IPEndPoint? lep = null;
            return client.Receive(ref lep);
        }

        public async void SendAsync(byte[] datagram, int bytes)
        {
            kcp?.Send(datagram.AsSpan().Slice(0, bytes));
        }

        public async ValueTask<byte[]?> ReceiveAsync()
        {
            if (kcp == null)
                return null;
            var (buffer, avalidLength) = kcp.TryRecv();
            while (buffer == null)
            {
                await Task.Delay(10);
                (buffer, avalidLength) = kcp.TryRecv();
            }

            var s = buffer.Memory.Span.Slice(0, avalidLength).ToArray();
            return s;
        }

        public async void BeginRecv()
        {
            if (client == null)
                return;
            bool lbool = true;
            while (lbool)
            {
                try
                {
                    var res = await client.ReceiveAsync();
                    if (udp_cmd.CheckByteEquals(res.Buffer, System.Text.Encoding.Default.GetBytes("NFC"), 3))
                    {//NFC = not find connect 服务器维护的连接失效了
                        NglKcp.getInstance().Close();
                        if (NglKcp.reconnect != null)
                            NglKcp.reconnect();
                        return;
                    }
                    EndPoint = res.RemoteEndPoint;
                    kcp?.Input(res.Buffer);
                }
                catch (Exception e)
                {
                    lbool = false;
                }
            }
        }
    }
}

#endif


