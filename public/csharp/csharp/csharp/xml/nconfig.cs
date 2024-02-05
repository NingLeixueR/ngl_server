using Google.Protobuf.Compiler;
using Google.Protobuf.WellKnownTypes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;
using static Pbdb.db_activity.Types;

namespace ngl
{
    class nconfig
    {
        public static Int32 m_area = 0;
        public static string m_name = "";
        public static Int32 m_tnum = 0;
        public static Int32 m_actor_delayed = 0;
        public static Int32 m_logserverid = 0;
        public static bool m_varint = false;
        public static bool m_isxor = false;
        public static Int32 m_waiting = 0;
        public static bool m_robot_test = false;
        public static string m_xor_str = "";
        public static Int32 m_reloadcsv_version = 0;
        public static Dictionary<string, Int32> m_reloadcsv = new Dictionary<string, Int32>();
        public static string m_gmurl = "";
        public static string m_push_server_config = "";

        public static Int32 m_nodeid = 0;
        public static string m_servername = "";
        public static void set_server(string aservertypename, Int32 anodeid)
        {
            m_servername = aservertypename;
            m_nodeid = anodeid;
        }

        public static Int32 xmlgetInt32(XmlAttributeCollection axml, string akey)
        {
            var ltemp = axml[akey];
            if (ltemp == null)
                return -1;
            if (Int32.TryParse(ltemp.Value, out Int32 lval))
                return lval;
            return -1;
        }

        public static string xmlgetString(XmlAttributeCollection axml, string akey)
        {
            var ltemp = axml[akey];
            if (ltemp == null)
                return "";
            return ltemp.Value;
        }

        public static void load(string apath)
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(apath);
            XmlNode root = xmlDoc.DocumentElement;
            XmlNode configNode = root.SelectSingleNode("//config");
            if (configNode == null)
                return;
            var lAttributes = configNode.Attributes;
            if (lAttributes == null)
                return;

            m_area = xmlgetInt32(lAttributes, "area");
            string name = xmlgetString(lAttributes, "name");
            m_tnum = xmlgetInt32(lAttributes, "tnum");
            m_actor_delayed = xmlgetInt32(lAttributes, "actor_delayed");
            m_logserverid = xmlgetInt32(lAttributes, "logserverid");
            if (xmlgetString(lAttributes, "varint") == "true")
                m_varint = true;
            if (xmlgetString(lAttributes, "isxor") == "true")
                m_isxor = true;
            m_waiting = xmlgetInt32(lAttributes, "waiting");
            if (xmlgetString(lAttributes, "robot_test") == "true")
                m_robot_test = true;
            m_xor_str = xmlgetString(lAttributes, "xor_str");
            m_reloadcsv_version = xmlgetInt32(lAttributes, "reloadcsv_version");
            string lstr = xmlgetString(lAttributes, "reloadcsv");
            string[] result = Regex.Split(lstr, "\\*");
            foreach (var item in result)
                m_reloadcsv.Add(item, 1);
            m_gmurl = xmlgetString(lAttributes, "gmurl");
            m_push_server_config = xmlgetString(lAttributes, "push_server_config");
            var lbytes = System.Text.Encoding.Default.GetBytes(m_xor_str);
            encryption.set_xor(m_isxor, lbytes, lbytes.Length);
        }
    }
}
