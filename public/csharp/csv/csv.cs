using System;
using System.IO;
using System.Text.RegularExpressions;
using System.Xml.Linq;

namespace ngl
{
    class csvpair
    {
        public string m_data;
        public int m_pos;
        public bool m_doublequotationmarks;     // 双引号
        public char m_fg;                       // 默认,

        public csvpair()
        {
            m_data = "";
            m_pos = 0;
            m_doublequotationmarks = false;
            m_fg = ',';
        }
        public void clear()
        {
            m_data = "";
            m_pos = 0;
            m_doublequotationmarks = false;
        }
    }

    interface ICsvRead
    {
        public bool Read(csvpair apair);
    }

    interface ICsv
    {
        public Int32 Id();
    }

    partial class rcsv
    {
        public string m_data;
        public int m_pos;

        public bool read(string aname, ref Int32 aversion)
        {
            readfile lrf = new readfile(aname);
            lrf.open();
            //# 跳过前3行表头
            lrf.jumpbegin(3, true);
            string? lversion = lrf.readline();
            if (lversion != null)
            {
                string[] result1 = Regex.Split(lversion, ",");
                if (result1.Length == 0)
                    return false;
                string[] result2 = Regex.Split(result1[0], ":");
                if (result2.Length != 2)
                    return false;
                if (result2[0] != "version")
                    return false;
                if (Int32.TryParse(result2[1], out aversion) == false)
                    return false;
            }
            m_data = lrf.readcurrent();
            lrf.close();
            return true;
        }
        public bool readline(ref string adata)
        {
            adata = "";
            bool lbool = false;
            for (; m_pos < m_data.Length; ++m_pos)
            {
                if (!lbool)
                {
                    if (m_data[m_pos] == '\r')
                        continue;
                    if (m_data[m_pos] == '\n')
                    {
                        ++m_pos;
                        return true;
                    }
                }
                if (m_data[m_pos] == '\"') lbool = lbool ? false : true; ;
                adata += m_data[m_pos];
            }
            return adata != "";
        }

        public static string read(csvpair apair)
        {
            string ltemp = "";
            for (; apair.m_pos < apair.m_data.Length; ++apair.m_pos)
            {
                if (!apair.m_doublequotationmarks)
                {
                    if (apair.m_data[apair.m_pos] == apair.m_fg)
                    {
                        ++apair.m_pos;
                        break;
                    }
                    if (apair.m_data[apair.m_pos] == '\n')
                    {
                        ++apair.m_pos;
                        break;
                    }
                    if (apair.m_data[apair.m_pos] == '\r') 
                        continue;
                }
                if (apair.m_data[apair.m_pos] == '\"')
                {
                    apair.m_doublequotationmarks = apair.m_doublequotationmarks ? false : true;
                    continue;
                }
                ltemp += apair.m_data[apair.m_pos];
            }
            return ltemp;
        }

        public rcsv(string afilename, ref Int32 aversion)
        {
            read(afilename, ref aversion);
        }

        public rcsv()
        {
            m_pos = 0;
        }

        public static bool isok(csvpair apair)
        {
            return apair.m_pos >= apair.m_data.Length;
        }

        public static bool readcsv(csvpair apair, ref string adata)
        {
            adata = read(apair);
            return true;
        }

        public static bool readcsv(csvpair apair, ref byte adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (byte.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }

        public static bool readcsv(csvpair apair, ref sbyte adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (sbyte.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }

        public static bool readcsv(csvpair apair, ref Int16 adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (Int16.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }
        public static bool readcsv(csvpair apair, ref UInt16 adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (UInt16.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }

        public static bool readcsv(csvpair apair, ref Int32 adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (Int32.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }

        public static bool readcsv(csvpair apair, ref UInt32 adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (UInt32.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }

        public static bool readcsv(csvpair apair, ref Int64 adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (Int64.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }

        public static bool readcsv(csvpair apair, ref UInt64 adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (UInt64.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }

        public static bool readcsv(csvpair apair, ref float adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (float.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }

        public static bool readcsv(csvpair apair, ref double adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                if (double.TryParse(ltemp, out adata) == false)
                    return false;
            }
            else
                adata = 0;
            return true;
        }

        public static bool readcsv(csvpair apair, ref bool adata)
        {
            string ltemp = read(apair);
            if (ltemp != "")
            {
                adata = ltemp != "0";
            }
            else
                adata = false;
            return true;
        }

        public static bool readcsv<T>(csvpair apair, List<T> avec) where T : ICsvRead, new()
        {
            string ltempstr = read(apair);
            csvpair lpair = new csvpair();
            lpair.m_data = ltempstr;
            for (; !isok(lpair);)
            {
                T ltemp = new T();
                if (ltemp.Read(lpair))
                    avec.Add(ltemp);
            }
            return true;
        }

        public static bool readcsv(csvpair apair, List<Int16> avec)
        {
            string ltempstr = read(apair);
            csvpair lpair = new csvpair();
            lpair.m_data = ltempstr;
            for (; !isok(lpair);)
            {
                Int16 ltemp = 0;
                if (readcsv(lpair, ref ltemp))
                    avec.Add(ltemp);
            }
            return true;
        }
        public static bool readcsv(csvpair apair, List<UInt16> avec)
        {
            string ltempstr = read(apair);
            csvpair lpair = new csvpair();
            lpair.m_data = ltempstr;
            for (; !isok(lpair);)
            {
                UInt16 ltemp = 0;
                if (readcsv(lpair, ref ltemp))
                    avec.Add(ltemp);
            }
            return true;
        }
        public static bool readcsv(csvpair apair, List<Int32> avec)
        {
            string ltempstr = read(apair);
            csvpair lpair = new csvpair();
            lpair.m_data = ltempstr;
            for (; !isok(lpair);)
            {
                Int32 ltemp = 0;
                if (readcsv(lpair, ref ltemp))
                    avec.Add(ltemp);
            }
            return true;
        }
        public static bool readcsv(csvpair apair, List<UInt32> avec)
        {
            string ltempstr = read(apair);
            csvpair lpair = new csvpair();
            lpair.m_data = ltempstr;
            for (; !isok(lpair);)
            {
                UInt32 ltemp = 0;
                if (readcsv(lpair, ref ltemp))
                    avec.Add(ltemp);
            }
            return true;
        }
        public static bool readcsv(csvpair apair, List<Int64> avec)
        {
            string ltempstr = read(apair);
            csvpair lpair = new csvpair();
            lpair.m_data = ltempstr;
            for (; !isok(lpair);)
            {
                Int64 ltemp = 0;
                if (readcsv(lpair, ref ltemp))
                    avec.Add(ltemp);
            }
            return true;
        }
        public static bool readcsv(csvpair apair, List<UInt64> avec)
        {
            string ltempstr = read(apair);
            csvpair lpair = new csvpair();
            lpair.m_data = ltempstr;
            for (; !isok(lpair);)
            {
                UInt64 ltemp = 0;
                if (readcsv(lpair, ref ltemp))
                    avec.Add(ltemp);
            }
            return true;
        }
        public static bool readcsv(csvpair apair, List<byte> avec)
        {
            string ltempstr = read(apair);
            csvpair lpair = new csvpair();
            lpair.m_data = ltempstr;
            for (; !isok(lpair);)
            {
                byte ltemp = 0;
                if (readcsv(lpair, ref ltemp))
                    avec.Add(ltemp);
            }
            return true;
        }
        public static bool readcsv(csvpair apair, List<sbyte> avec)
        {
            string ltempstr = read(apair);
            csvpair lpair = new csvpair();
            lpair.m_data = ltempstr;
            for (; !isok(lpair);)
            {
                sbyte ltemp = 0;
                if (readcsv(lpair, ref ltemp))
                    avec.Add(ltemp);
            }
            return true;
        }
        public bool readcsv<T>(Dictionary<int, T> adata) where T : ICsvRead, ICsv, new()
        {
            csvpair lpair = new csvpair();
            for (; readline(ref lpair.m_data);)
            {
                if (lpair.m_data != "")
                {
                    if (lpair.m_data[0] == '#')
                        continue;
                    T ltemp = new T();
                    if (ltemp.Read(lpair))
                    {
                        adata[ltemp.Id()] = ltemp;
                        lpair.clear();
                    }
                }
            }
            return true;
        }
    }
}