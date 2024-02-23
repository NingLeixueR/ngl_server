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

        public SimpleKcpClient(int port)
            : this(port, null)
        {
        }

        public SimpleKcpClient(int port, IPEndPoint endPoint)
        {
            client = new UdpClient(port);
            kcp = new SimpleSegManager.Kcp(1, this);
            this.EndPoint = endPoint;
            BeginRecv();
        }

        public SimpleSegManager.Kcp kcp { get; }
        public IPEndPoint EndPoint { get; set; }

        public void Output(IMemoryOwner<byte> buffer, int avalidLength)
        {
            var s = buffer.Memory.Span.Slice(0, avalidLength).ToArray();
            client.SendAsync(s, s.Length, EndPoint);
            buffer.Dispose();
        }

        public async void SendAsync(byte[] datagram, int bytes)
        {
            kcp.Send(datagram.AsSpan().Slice(0, bytes));
        }

        //public async ValueTask<byte[]> ReceiveAsync()
        //{
        //    var (buffer, avalidLength) = kcp.TryRecv();
        //    while (buffer == null)
        //    {
        //        await Task.Delay(10);
        //        (buffer, avalidLength) = kcp.TryRecv();
        //    }

        //    var s = buffer.Memory.Span.Slice(0, avalidLength).ToArray();
        //    return s;
        //}
        //public async ValueTask<tcp_buff> ReceiveAsync()
        //{
        //    var (buffer, avalidLength) = kcp.TryRecv();
        //    while (buffer == null)
        //    {
        //        await Task.Delay(10);
        //        (buffer, avalidLength) = kcp.TryRecv();
        //    }

        //    tcp_buff ret = new tcp_buff();
        //    ret.m_buff = buffer.Memory.Span.Slice(0, avalidLength).ToArray();
        //    ret.m_len = avalidLength;
        //    ret.m_pos = 0;
        //    return ret;
        //}

        //private async void BeginRecv()
        //{
        //    var res = await client.ReceiveAsync();
        //    EndPoint = res.RemoteEndPoint;
        //    kcp.Input(res.Buffer);
        //    BeginRecv();
        //}

        // 对于 ReceiveAsync() 方法
        private TaskCompletionSource<byte[]> receiveTaskCompletionSource;

        public async ValueTask<byte[]> ReceiveAsync()
        {
            var (buffer, availableLength) = kcp.TryRecv();
            if (buffer != null)
            {
                // 如果有可用数据，立即返回
                return buffer.Memory.Slice(0, availableLength).ToArray();
            }

            // 否则，等待数据到来
            receiveTaskCompletionSource = new TaskCompletionSource<byte[]>();
            return await receiveTaskCompletionSource.Task;
        }

        // 在 BeginRecv() 方法中
        private async void BeginRecv()
        {
            var res = await client.ReceiveAsync();
            EndPoint = res.RemoteEndPoint;
            kcp.Input(res.Buffer);

            // 检查是否有等待的任务
            if (receiveTaskCompletionSource != null && !receiveTaskCompletionSource.Task.IsCompleted)
            {
                var buffer = await ReceiveAsync();
                receiveTaskCompletionSource.SetResult(buffer);
            }

            BeginRecv();
        }
    }
}

#endif


