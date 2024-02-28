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
        UdpClient client;
        public void close()
        {
            client.Close();
        }

        public SimpleKcpClient(int port)
            : this(port, null)
        {
        }

        public SimpleKcpClient(int port, IPEndPoint endPoint)
        {
            client = new UdpClient(port);
            kcp = new SimpleSegManager.Kcp(1, this);
            this.EndPoint = endPoint;
        }

        public SimpleSegManager.Kcp kcp { get; set; }
        public IPEndPoint EndPoint { get; set; }

        public void Output(IMemoryOwner<byte> buffer, int avalidLength)
        {
            var s = buffer.Memory.Span.Slice(0, avalidLength).ToArray();
            client.SendAsync(s, s.Length, EndPoint);
            buffer.Dispose();
        }

        public void UdpSend(byte[] buff, IPEndPoint aEndPoint)
        {
            client.Send(buff, buff.Length, aEndPoint);
        }

        public async void SendAsync(byte[] datagram, int bytes)
        {
            kcp.Send(datagram.AsSpan().Slice(0, bytes));
        }

        public async ValueTask<byte[]> ReceiveAsync()
        {
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
            while (true)
            {
                var res = await client.ReceiveAsync();
                EndPoint = res.RemoteEndPoint;
                kcp.Input(res.Buffer);
            }
        }

        public byte[] Recv()
        {
            IPEndPoint? lep = null;
            return client.Receive(ref lep);
        }
    }
}

#endif


