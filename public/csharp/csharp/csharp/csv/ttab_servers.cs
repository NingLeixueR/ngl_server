using ngl;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ngl
{
    class ttab_servers
    {
        static Dictionary<Int16, List<tab_servers>> m_areaofserver = new Dictionary<Int16, List<tab_servers>>();

        public void reload()
        {
            m_areaofserver.Clear();
            manage_csv<tab_servers>.load("tab_servers.csv");
            foreach(var item in manage_csv<tab_servers>.tablecsv)
			{
                List<tab_servers>? ls = null;
                if (m_areaofserver.TryGetValue(item.Value.m_area, out ls) == false)
                {
                    ls = new List<tab_servers>();
                    m_areaofserver.Add(item.Value.m_area, ls);
                }
                ls.Add(item.Value);
            }
        }

        public static tab_servers? tab(Int32 aserverid)
        {
            tab_servers? val = null;
            if (manage_csv<tab_servers>.tablecsv.TryGetValue(aserverid, out val))
                return val;
            return null;
        }

        public static tab_servers? tab()
        {
            return tab(nconfig.m_nodeid);
        }

        public static tab_servers? tab(string aname, Int32 area, Int32 atcount)
		{
			foreach (var item in manage_csv<tab_servers>.tablecsv)
			{
				if (item.Value.m_area == area && item.Value.m_name == aname && item.Value.m_tcount == atcount)
					return item.Value;
			}
			return null;
		}

        public static ENET_PROTOCOL netprotocol(Int32 aserverid)
        {
            var ltab = tab(aserverid);
            if (ltab == null)
                return ENET_PROTOCOL.ENET_TCP;
            return ltab.m_net;
        }

        public static ENET_PROTOCOL netprotocol()
        {
            return netprotocol(nconfig.m_nodeid);
        }

        public static NODE_TYPE node_type(Int32 aserverid)
        {
            var ltab = tab(aserverid);
            if (ltab == null)
                return NODE_TYPE.FAIL;
            return ltab.m_type;
        }

        public static NODE_TYPE node_type()
        {
            return node_type(nconfig.m_nodeid);
        }

        public static tab_servers? node_tnumber(NODE_TYPE atype, Int32 anumber)
        {
            foreach (var item in manage_csv<tab_servers>.tablecsv)
            {
                if (item.Value.m_type == atype && item.Value.m_tcount == anumber)
                    return item.Value;
            }
            return null;
        }

        public static void foreach_server(Action<tab_servers> afun)
        {
            foreach (var item in m_areaofserver)
            {
                foreach (tab_servers iserver in item.Value)
                {
                    afun(iserver);
                }
            }
        }

        public static void foreach_server(NODE_TYPE atype, Action<tab_servers> afun)
        {
            foreach (var item in m_areaofserver)
            {
                //if (aislocal && _area != tab()->m_area)
                //	continue;
                foreach (tab_servers iserver in item.Value)
                {
                    if (iserver.m_type == atype)
                    {
                        afun(iserver);
                    }
                }
            }
        }

        public static void foreach_server(NODE_TYPE atype, Int16 area, Action<tab_servers> afun)
        {
            foreach (var item in m_areaofserver)
            {
                if (item.Key != area)
                    continue;
                foreach (tab_servers iserver in item.Value)
                {
                    if (iserver.m_type == atype)
                    {
                        afun(iserver);
                    }
                }
            }
        }

        public delegate bool checkfun(tab_servers aserver);
        public static tab_servers? find_first(NODE_TYPE atype, checkfun afun)
        {
            if (m_areaofserver.TryGetValue((Int16)atype, out List<tab_servers> ls) == false)
                return null;
            foreach (var iserver in ls)
            {
                if (iserver.m_type == atype && afun(iserver))
                {
                    return iserver;
                }
            }
            return null;
        }
    }
}
