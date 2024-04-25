#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/typeof/typeof.hpp>
#include <functional>
#include <string>
#include <map>

#include "csvtable.h"
#include "regular.h"
#include "xmlinfo.h"
#include "type.h"

namespace ngl
{
	using boost_ptree = boost::property_tree::ptree;

	class xml
	{
		xml() {}
	public:
		static xml& getInstance() 
		{ 
			static xml ltemp; 
			return ltemp; 
		}

		static bool read(std::string aname, boost_ptree& apt);

		static bool write(std::string aname, boost_ptree& apt);

		template <typename T>
		static T get(boost_ptree& apt, const std::string& anode)
		{
			return apt.get<T>(anode);
		}

		template <typename T>
		static void set(boost_ptree& apt, const std::string& anode, const T& adata)
		{
			apt.put<T>(anode, adata);
		}

		template <typename T>
		static T get_xmlattr(boost_ptree& apt, const std::string& anode)
		{
			std::string lnode = "<xmlattr>.";
			lnode += anode;
			return apt.get<T>(lnode);
		}

		template <typename T>
		static void set_xmlattr(boost_ptree& apt, const std::string& anode, T& avalue)
		{
			std::string lnode = "<xmlattr>.";
			lnode += anode;
			apt.put<T>(lnode, avalue);
		}

		static std::string data(boost_ptree& apt);
		static boost_ptree get_child(boost_ptree& apt, const std::string& anode);
		static void add_child(boost_ptree& apt, const std::string& anodename, boost_ptree& anode);
		static boost_ptree::assoc_iterator findchild(boost_ptree& apt, const std::string& anode);
		static void _foreach(boost_ptree& apt, const std::string& anode, const std::function<void(boost_ptree& apt)> afun);
		static void _foreach(boost_ptree& apt, const std::function<void(boost_ptree& apt, const std::string&)> afun);
		static void _foreach_xmlattr(boost_ptree& apt, const std::function<void(boost_ptree& apt, const std::string&)> afun);
	};

	class xmlnode
	{
	public:
		static boost_ptree		m_root;
		static dbserver_info	m_db;
		static xmlinfo			m_publicinfo;
		static std::string		m_nodename;
		static NODE_TYPE		m_nodetype;
		static i32_id			m_nodeid;

		static void init();

		static NODE_TYPE node_type();
		static void set_server(const char* aservertypename, int anodeid);

		static void load(const std::string& axml);
		static void read_config(boost_ptree& apt, xmlinfo& anfo);
		static void read_config(const char* akey, std::map<i32_serverid, xmlinfo>& anfo);
		static void read_public_config(xmlinfo& anfo);
		static void read_db_arg(dbarg& m_dbarg);

		static void read_net_tab(const char* akey, const char* akey2, std::set<NODE_TYPE>& anet);
		static void loaddb();
		static void loadpublic();

		static xmlinfo* get_publicconfig() { return &m_publicinfo; }
	};
}// namespace ngl
using nconfig = ngl::xmlnode;