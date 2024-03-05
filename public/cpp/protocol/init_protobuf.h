#pragma once
#include <map>
#include "gm.pb.h"
#include "net.pb.h"
#include "define.h"
#include "xmlprotocol.h"

// typeid(T).hash_code() 因为c++标准并没有强制要求只是建议编译器 不同类型生成不同的hash值
#define NOTUSING_BOOST_TYPEINFO
#ifndef NOTUSING_BOOST_TYPEINFO 
# include <boost/type_index.hpp>
# define HASH_CODE_VALUE(_TYPE)	boost::typeindex::type_id<_TYPE>().hash_code()
#else
# define HASH_CODE_VALUE(_TYPE)	typeid(T).hash_code()
#endif

#define TYPE_NAME(_TYPE) boost::typeindex::type_id_with_cvr<_TYPE>().pretty_name()

namespace ngl
{
	template <typename T, EPROTOCOL_TYPE PROTYPE, bool ISUSING, typename TREAL>
	struct actor_forward;

	// protobuf3不支持默认值
	class init_protobuf
	{
	private:
		struct pinfo
		{
			EPROTOCOL_TYPE m_type;
			int m_protocol;
			std::string m_name;
		};
		static std::multimap<size_t, pinfo> m_keyval;
		// net/gm		[1			-  100000000];
		// custom		[200000001	-  300000000];
		static int32_t lcustoms/* = 200000000*/;

		template <typename T>
		static size_t hash_code()
		{
			static size_t lcode = HASH_CODE_VALUE(T);
			return lcode;
		}
	public:
		//CUSTOM
		template <typename T>
		static void init_customs(EPROTOCOL_TYPE atype, const T*)
		{
			size_t lcode = hash_code<T>();
			if (m_keyval.find(lcode) != m_keyval.end())
			{
				LogLocalError("init_customs(%) is same!!!", typeid(T).name())
				return;
			}
			m_keyval.insert(std::make_pair(lcode, pinfo
				{
					.m_type = atype,
					.m_protocol = ++lcustoms,
					.m_name = TYPE_NAME(T)
				}));
			LogLocalInfo("#[%][EPROTOCOL_TYPE_CUSTOM][%]", lcustoms, typeid(T).name());
		}

		template <typename T, typename ...ARG>
		static void init_customs(EPROTOCOL_TYPE atype, const T* adata, const ARG*... args)
		{
			init_customs(atype, adata);
			init_customs(atype, args...);
		}

		template <typename T>
		static bool init_protobufs()
		{
			static std::string lname = TYPE_NAME(T);
			int32_t lprotocol = xmlprotocol::protocol(lname);
			if (lprotocol == -1)
			{
				LogLocalError("init_protobuf::init_protobufs(%) not xml!!!", lname)
				return false;
			}
			m_keyval.insert(std::make_pair(HASH_CODE_VALUE(T), pinfo
				{
					.m_type = EPROTOCOL_TYPE_PROTOCOLBUFF,
					.m_protocol = lprotocol,
					.m_name = lname
				}));
			LogLocalInfo("#[%][EPROTOCOL_TYPE_PROTOCOLBUFF][%]", lprotocol, lname);
			return true;
		}

		static void init_customs();

		template <typename T>
		static pinfo& get()
		{
			using TRC = std::remove_const<T>::type;
			size_t lcode = hash_code<T>();
			auto itor = m_keyval.find(lcode);
			if (itor == m_keyval.end())
			{
				std::cout << TYPE_NAME(TRC) << std::endl;
				assert(init_protobufs<TRC>());
				itor = m_keyval.find(lcode);
				assert(itor != m_keyval.end());
			}
			return itor->second;
		}

		template <typename T>
		static int32_t protocol()
		{
			pinfo& linfo = get<T>();
			return linfo.m_protocol;
		}

		template <typename T>
		static EPROTOCOL_TYPE protocol_type()
		{
			pinfo& linfo = get<T>();
			return linfo.m_type;
		}

		template <typename T>
		static const std::string& protocol_name()
		{
			pinfo& linfo = get<T>();
			return linfo.m_name;
		}

		static void initall();
	};
	
}

