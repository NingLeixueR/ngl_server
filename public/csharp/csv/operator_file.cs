using System;
using System.IO;
using System.Security.Cryptography.X509Certificates;
using System.Xml.Linq;

namespace ngl
{
    class readfile
    {
        private string m_filename;
        private System.IO.StreamReader m_file;

        public readfile(string filename)
        {
            m_filename = filename;
            m_file = null;
        }

        public void open()
        {
            m_file = new System.IO.StreamReader(m_filename);
        }

        public void close()
        {
            m_file.Close();
        }

        //跳过前三行
        public void jumpbegin(int anum, bool aiscsv)
        {
            if (aiscsv == false)
            {
                while (--anum >= 0)
                {
                    m_file.ReadLine();
                }
            }
            else
            {
                Int32 lisshuyin = 0;
                while (anum > 0)
                {
                    string line;
                    line = m_file.ReadLine();
                    if (line == null)
                        return;
                    for (int i = 0; i < line.Length; ++i)
                    {
                        if (line[i] == '\"')
                            ++lisshuyin;
                    }
                    if (lisshuyin % 2 == 0)
                        --anum;
                }
            }
        }

        public string? readline()
        {
            return m_file.ReadLine();
        }

        public string readcurrent()
        {
            return m_file.ReadToEnd();
        }
    };
}