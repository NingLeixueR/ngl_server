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
                List<tab_servers> ls = null;
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
            // return tab(nconfig::m_nodeid);
            return null;
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

//static ENET_PROTOCOL netprotocol(i32_serverid aserverid)
//{
//    tab_servers* ltab = tab(aserverid);
//    if (ltab == nullptr)
//        return ENET_TCP;
//    return ltab->m_net;
//}

//static ENET_PROTOCOL netprotocol()
//{
//    return netprotocol(nconfig::m_nodeid);
//}

//static NODE_TYPE node_type(i32_serverid aserverid)
//{
//    tab_servers* ltab = tab(aserverid);
//    assert(ltab != nullptr);
//    return ltab->m_type;
//}

//static NODE_TYPE node_type()
//{
//    return node_type(nconfig::m_nodeid);
//}

//static tab_servers* node_tnumber(NODE_TYPE atype, int32_t anumber)
//{
//    ttab_servers* ttab = allcsv::get<ttab_servers>();
//    assert(ttab != nullptr);
//    for (std::pair <const int, tab_servers> &pair : ttab->tablecsv)
//			{
//    if (pair.second.m_type == atype && pair.second.m_tcount == anumber)
//        return &pair.second;
//}
//return nullptr;
//		}

//		static void foreach_server(const std::function<void(const tab_servers*)> &afun)
//		{
//    for (const auto&[_area, _vec] : m_areaofserver)
//			{
//        for (const tab_servers* iserver : _vec)
//				{
//            afun(iserver);
//        }
//    }
//}

//static void foreach_server(NODE_TYPE atype, const std::function<void(const tab_servers*)> &afun)
//		{
//    for (const auto&[_area, _vec] : m_areaofserver)
//			{
//        //if (aislocal && _area != tab()->m_area)
//        //	continue;
//        for (const tab_servers* iserver : _vec)
//				{
//            if (iserver->m_type == atype)
//            {
//                afun(iserver);
//            }
//        }
//    }
//}

//static void foreach_server(NODE_TYPE atype, i16_area area, const std::function<void(const tab_servers*)> &afun)
//		{
//    for (const auto&[_area, _vec] : m_areaofserver)
//			{
//        if (_area != area)
//            continue;
//        for (const tab_servers* iserver : _vec)
//				{
//            if (iserver->m_type == atype)
//            {
//                afun(iserver);
//            }
//        }
//    }
//}


//static tab_servers* find_first(NODE_TYPE atype, const std::function<bool(tab_servers*)> &afun)
//		{
//    std::vector<tab_servers*>* litem = tools::findmap(m_areaofserver, tab()->m_area);
//    if (litem == nullptr)
//        return nullptr;
//    for (tab_servers* iserver : *litem)
//    {
//        if (iserver->m_type == atype && afun(iserver))
//        {
//            return iserver;
//        }
//    }
//    return nullptr;
//}
    }
}
