#pragma once
#ifdef USE_BOOST_INTERPROCESS

#include "tools.h"
#include "regular.h"

#include <string>
#include <memory>
#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace ngl
{
	template <typename T>
	class shared_memory
	{
		std::string														m_meryname;
		std::unique_ptr<boost::interprocess::shared_memory_object>		m_title;
		std::unique_ptr<boost::interprocess::mapped_region>				m_titleregion;

		std::map<int64_t, 
			std::pair<
			std::unique_ptr<boost::interprocess::shared_memory_object>,
			std::unique_ptr<boost::interprocess::mapped_region>
			>
		>	m_memory;
		int32_t m_bytes;

		enum
		{
			enum_title_incremental = 10, // titleµÝÔöÊýÁ¿
		};

	private:
		bool sm_open(
			std::unique_ptr<boost::interprocess::shared_memory_object>& ashm, 
			const char* aname
		)
		{
			try
			{
				ashm = std::make_unique<boost::interprocess::shared_memory_object>(
					boost::interprocess::open_only
					, aname
					, boost::interprocess::read_write
				);
			}
			catch (const boost::interprocess::interprocess_exception& aerror)
			{
				std::cout << aerror.what() << std::endl;
				return false;
			}
			return true;
		}

		bool sm_create(
			std::unique_ptr<boost::interprocess::shared_memory_object>& ashm, 
			const char* aname
		)
		{
			try
			{
				ashm = std::make_unique<boost::interprocess::shared_memory_object>(
					boost::interprocess::create_only
					, aname
					, boost::interprocess::read_write
				);
			}
			catch (const boost::interprocess::interprocess_exception& aerror)
			{
				std::cout << aerror.what() << std::endl;
				return false;
			}
			return true;
		}

		int32_t& sm_title_pos()
		{
			return ((int32_t*)m_titleregion->get_address())[0];
		}

		int32_t& sm_title_size()
		{
			return ((int32_t*)m_titleregion->get_address())[1];
		}

		int64_t* sm_title()
		{
			return &((int64_t*)m_titleregion->get_address())[1];
		}

		// À©ÈÝ
		void resize()
		{
			int32_t lpos = sm_title_pos();
			std::unique_ptr<int64_t[]> lbuff = std::make_unique<int64_t[]>(lpos + 1);
			memcpy(lbuff.get(), &((int64_t*)m_titleregion->get_address())[0], (lpos + 1) * sizeof(int64_t));
			m_title->truncate(sm_title_size()+ enum_title_incremental);
			*m_titleregion = boost::interprocess::mapped_region(*m_title, boost::interprocess::read_write);
			memcpy(&((int64_t*)m_titleregion->get_address())[0], lbuff.get(), (lpos + 1) * sizeof(int64_t));
		}
	public:
		shared_memory() :
			m_meryname(dtype_name(T)),
			m_bytes(sizeof(T))
		{
			ngl::regular::replace(" ", "", m_meryname, m_meryname);
			ngl::regular::replace("::", "_", m_meryname, m_meryname);
			try
			{
				if (sm_open(m_title, m_meryname.c_str()) == false)
				{
					if (sm_create(m_title, m_meryname.c_str()))
					{
						m_title->truncate((1 + enum_title_incremental) * sizeof(int64_t));
						m_titleregion = std::make_unique<boost::interprocess::mapped_region>(
							*m_title,
							boost::interprocess::read_write
						);
						sm_title_size() = 10;
						sm_title_pos() = 0;
					}
					else
					{
						throw "error sm_create fail";
					}
				}
				else
				{
					m_titleregion = std::make_unique<boost::interprocess::mapped_region>(
						*m_title,
						boost::interprocess::read_write
					);
					std::cout << "#############################################" << std::endl;
					std::cout << "sm_title_size = " << sm_title_size() << std::endl;
					std::cout << "sm_title_pos = " << sm_title_pos() << std::endl;
					for (int i = 0; i < sm_title_pos(); ++i)
					{
						std::cout << sm_title()[i] << std::endl;
					}
					std::cout << "#############################################" << std::endl;
				}
			}
			catch (const boost::interprocess::interprocess_exception& aerror)
			{
				std::cout << aerror.what() << std::endl;
			}
		}

		void init()
		{
			int64_t* lptitle = sm_title();
			std::vector<int64_t> lidvec;
			for (int i = 0; i < sm_title_pos(); ++i)
			{
				int64_t lid = lptitle[i];
				std::string lname = name(lid);
				std::pair<
					std::unique_ptr<boost::interprocess::shared_memory_object>,
					std::unique_ptr<boost::interprocess::mapped_region>
				>& lpair = m_memory[lid];
				if (sm_open(lpair.first, lname.c_str()) == false)
				{
					m_memory.erase(lid);
					continue;
				}
				lidvec.push_back(lid);
				lpair.second = std::make_unique<boost::interprocess::mapped_region>(
					*lpair.first,
					boost::interprocess::read_write
				);
			}
			memset(lptitle, 0, sm_title_size() * sizeof(int64_t));
			sm_title_pos() = lidvec.size();
			if (lidvec.empty() == false)
			{
				memcpy(lptitle, lidvec.data(), sm_title_pos()*sizeof(int64_t));
			}
		}

		T* get(int64_t aid)
		{
			auto itor = m_memory.find(aid);
			if (itor == m_memory.end())
				return nullptr;
			return (T*)itor->second.second->get_address();
		}

		void add_title(int64_t aid)
		{
			if (sm_title_pos()+1 >= sm_title_size())
			{
				resize();
			}
			sm_title()[++sm_title_pos()] = aid;
		}

		bool set(int64_t aid, const T& adata)
		{
			T* lpdata = get(aid);
			if (lpdata == nullptr)
			{
				std::string lname = name(aid);
				std::pair<
					std::unique_ptr<boost::interprocess::shared_memory_object>,
					std::unique_ptr<boost::interprocess::mapped_region>
				>& lpair = m_memory[aid];
				if (sm_create(lpair.first, lname.c_str()) == false)
				{
					m_memory.erase(aid);
					return false;
				}
				lpair.second = std::make_unique<boost::interprocess::mapped_region>(
					*lpair.first,
					boost::interprocess::read_write
				);
				*(T*)lpair.second->get_address() = adata;
				add_title(aid);
			}
			else
			{
				*lpdata = adata;
			}
			return true;
		}

		std::string name(int64_t aid)
		{
			std::string lname(m_meryname);
			lname += tools::lexical_cast<std::string>(aid);
			return std::move(lname);
		}

		void remove(int64_t aid)
		{
			remove_title([aid](int64_t avalid)
				{
					return avalid == aid;
				});

			remove_data(aid);
		}

		void remove_title(const std::function<bool(int64_t)>& afun)
		{
			int64_t* lpids_beg = sm_title();
			int64_t* lpids_end = lpids_beg + sm_title_pos();
			lpids_end = std::remove_if(lpids_beg, lpids_end, afun);
			sm_title_pos() = lpids_end - lpids_beg;
		}

		void remove_data(int64_t aid)
		{
			auto itor = m_memory.find(aid);
			if (itor == m_memory.end())
				return;
			m_memory.erase(itor);
			std::string lname = name(aid);
			boost::interprocess::shared_memory_object::remove(lname.c_str());
		}

		void remove(std::set<int64_t>& aset)
		{
			remove_title([&aset](int64_t aid)
				{
					return aset.find(aid) != aset.end();
				});

			for (int64_t lid : aset)
			{
				remove_data(lid);
			}			
		}

		void remove(std::vector<int64_t>& avec)
		{
			std::set<int64_t> lset;
			lset.insert(avec.begin(), avec.end());
			remove(lset);
		}

		void clear()
		{
			int64_t* lpids_beg = sm_title();
			int64_t* lpids_end = lpids_beg + sm_title_pos();
			std::for_each(lpids_beg, lpids_end, [this](int64_t aid)
				{
					remove_data(aid);
				});
			memset(lpids_beg, 0, sm_title_size() * sizeof(int64_t));
			sm_title_pos() = 0;
		}

		void foreach(const std::function<void(T&)>& afun)
		{
			int64_t* lpids_beg = sm_title();
			int64_t* lpids_end = lpids_beg + sm_title_pos();
			for (; lpids_beg != lpids_end; ++lpids_beg)
			{
				T* lp = get(*lpids_beg);
				if (lp != nullptr)
				{
					afun(*lp);
				}
			}
		}

		void foreach_index(const std::function<void(int, T&)>& afun)
		{
			int64_t* lpids_beg = sm_title();
			int64_t* lpids_end = lpids_beg + sm_title_pos();
			int lindex = 0;
			for (; lpids_beg != lpids_end; ++lpids_beg)
			{
				T* lp = get(*lpids_beg);
				if (lp != nullptr)
				{
					afun(++lindex, *lp);
				}
			}
		}
	};
}

#endif//USE_BOOST_INTERPROCESS