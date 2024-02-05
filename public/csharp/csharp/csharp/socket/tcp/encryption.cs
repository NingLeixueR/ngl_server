using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ngl
{
    class encryption
    {
        private static bool m_isxor = false;
        private static byte[] m_xorkey;
        private static Int32 m_xorkeynum = 0; //2^n-1 n为xorkey的字符数
        public static void set_xor(bool aisxor, byte[] axorkey, Int32 axorkeylen)
        {
            m_isxor = aisxor;
            m_xorkey = axorkey;
            for (int i = 1; ; ++i)
            {
                if (axorkeylen < ((1 << i) - 1))
                {
                    m_xorkeynum = i - 1;
                    break;
                }
            }
            if (m_xorkeynum <= 0)
            {
                m_isxor = false;
            }
        }
        public static void bytexor(byte[] ap, Int32 aplen, int apos)
        {
            if (m_isxor == false)
                return;
            if (m_xorkeynum < 1 || m_xorkey.Length <= 0)
                return;
            if (ap == null)
                return;
            for (int i = 0, j = apos; i < aplen; ++i, ++j)
            {
                ap[i] = (byte)(ap[i] ^ m_xorkey[j & m_xorkeynum]);
            }
        }
    }
}
