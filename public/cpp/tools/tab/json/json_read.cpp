#include "json_read.h"
#include "tools.h"
#include "nguid.h"

namespace ngl
{
	json_read::json_read(const char* astr) :
		m_json(cJSON_Parse(astr)),
		m_free(true)
	{}

	json_read::json_read() :
		m_json(nullptr),
		m_free(false)
	{}

	json_read::~json_read()
	{
		if (m_json != nullptr && m_free)
			cJSON_Delete(m_json);
	}

	bool json_read::read(const char* akey, std::string& adata) const
	{
		const cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret || ret->type != cJSON_String)
		{
			return false;
		}
		adata = ret->valuestring;
		return true;
	}

	class json_read_number
	{
	public:
		template <typename TNUMBER>
		static bool fun(cJSON* ajson, const char* akey, TNUMBER& adata)
		{
			const cJSON* ret = cJSON_GetObjectItem(ajson, akey);
			if (nullptr == ret)
			{
				return false;
			}
			return fun_value(ret, adata);
		}

		static bool fun(cJSON* ajson, const char* akey, int64_t& adata)
		{
			const cJSON* ret = cJSON_GetObjectItem(ajson, akey);
			if (nullptr == ret)
			{
				return false;
			}
			return fun_value(ret, adata);
		}

		static bool fun(cJSON* ajson, const char* akey, uint64_t& adata)
		{
			const cJSON* ret = cJSON_GetObjectItem(ajson, akey);
			if (nullptr == ret)
			{
				return false;
			}
			return fun_value(ret, adata);
		}

		static bool fun(cJSON* ajson, const char* akey, float& adata)
		{
			const cJSON* ret = cJSON_GetObjectItem(ajson, akey);
			if (nullptr == ret)
			{
				return false;
			}
			return fun_value(ret, adata);
		}

		static bool fun(cJSON* ajson, const char* akey, double& adata)
		{
			const cJSON* ret = cJSON_GetObjectItem(ajson, akey);
			if (nullptr == ret)
			{
				return false;
			}
			return fun_value(ret, adata);
		}

		static bool fun(cJSON* ajson, const char* akey, bool& adata)
		{
			const cJSON* ret = cJSON_GetObjectItem(ajson, akey);
			if (nullptr == ret)
			{
				return false;
			}
			return fun_value(ret, adata);
		}

		template <typename TNUMBER>
		static bool fun_value(const cJSON* ajson, TNUMBER& adata)
		{
			if (nullptr == ajson || (ajson->type != cJSON_Number && ajson->type != cJSON_String))
			{
				return false;
			}
			adata = (TNUMBER)ajson->valueint;
			if (ajson->type == cJSON_String)
			{
				try
				{
					adata = tools::lexical_cast<TNUMBER>(ajson->valuestring);
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
			return true;
			return true;
		}

		static bool fun_value(const cJSON* ajson, int64_t& adata)
		{
			if (ajson->type == cJSON_String)
			{
				adata = tools::lexical_cast<int64_t>(ajson->valuestring);
				return true;
			}
			// ³¢ÊÔint32_t
			if (ajson->type == cJSON_Number)
			{
				adata = ajson->valueint;
				return true;
			}
			return false;
		}

		static bool fun_value(const cJSON* ajson, uint64_t& adata)
		{
			if (ajson->type == cJSON_String)
			{
				adata = tools::lexical_cast<uint64_t>(ajson->valuestring);
				return true;
			}
			// ³¢ÊÔint32_t
			if (ajson->type == cJSON_Number)
			{
				adata = ajson->valueint;
				return true;
			}
			return false;
		}

		static bool fun_value(const cJSON* ajson, float& adata)
		{
			if (ajson->type == cJSON_Number)
			{
				adata = (float)ajson->valuedouble;
				return true;
			}
			if (ajson->type == cJSON_String)
			{
				try
				{
					adata = tools::lexical_cast<float>(ajson->valuestring);
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
			return true;
		}

		static bool fun_value(const cJSON* ajson, double& adata)
		{
			if (ajson->type == cJSON_Number)
			{
				adata = ajson->valuedouble;
				return true;
			}
			if (ajson->type == cJSON_String)
			{
				try
				{
					adata = tools::lexical_cast<double>(ajson->valuestring);
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
			return true;
		}

		static bool fun_value(const cJSON* ajson, bool& adata)
		{
			switch (ajson->type)
			{
			case cJSON_True:
			{
				adata = true;
				return true;
			}
			case cJSON_False:
			{
				adata = false;
				return true;
			}
			case cJSON_Number:
			{
				adata = ajson->valueint != 0;
				return true;
			}
			case cJSON_String:
			{
				std::string lvaluestr(ajson->valuestring);
				if (lvaluestr == "false" || lvaluestr == "FALSE" || lvaluestr == "0")
				{
					adata = false;
					return true;
				}
				if (lvaluestr == "true" || lvaluestr == "True" || lvaluestr == "1")
				{
					adata = true;
					return true;
				}
			}
			}
			return false;
		}
	};
	
	bool json_read::read(const char* akey, int8_t& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, int16_t& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, int32_t& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, int64_t& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, uint8_t& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, uint16_t& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, uint32_t& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, uint64_t& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, float& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, double& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, bool& adata) const
	{
		return json_read_number::fun(m_json, akey, adata);
	}

	bool json_read::read(const char* akey, cJSON*& adata) const
	{
		cJSON* ret = cJSON_GetObjectItem(m_json, akey);
		if (nullptr == ret)
		{
			return false;
		}
		if (ret->type == cJSON_Object || ret->type == cJSON_Array)
		{
			adata = ret;
			return true;
		}
		return false;
	}

	bool json_read::read(const char* akey, nguid& aval)const
	{
		int64_t lvalue = 0;
		if (read(akey, lvalue))
		{
			aval = nguid(lvalue);
			return true;
		}
		return false;
	}

	bool json_read::read(const char* akey, json_read& adata) const
	{
		if (read(akey, adata.m_json) == false)
		{
			return false;
		}
		adata.m_free = false;
		return true;
	}

	bool json_read::read(const char* akey, std::vector<int8_t>& adata) const
	{
		return read_number<int8_t>(akey, [&adata](const cJSON* ajson)
			{
				int8_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<int8_t>& adata) const
	{
		return read_number<int8_t>(akey, [&adata](const cJSON* ajson)
			{
				int8_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<int8_t>& adata) const
	{
		return read_number<int8_t>(akey, [&adata](const cJSON* ajson)
			{
				int8_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<int16_t>& adata) const
	{
		return read_number<int16_t>(akey, [&adata](const cJSON* ajson)
			{
				int16_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<int16_t>& adata) const
	{
		return read_number<int16_t>(akey, [&adata](const cJSON* ajson)
			{
				int16_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<int16_t>& adata) const
	{
		return read_number<int16_t>(akey, [&adata](const cJSON* ajson)
			{
				int16_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<int32_t>& adata) const
	{
		return read_number<int32_t>(akey, [&adata](const cJSON* ajson)
			{
				int32_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<int32_t>& adata) const
	{
		return read_number<int32_t>(akey, [&adata](const cJSON* ajson)
			{
				int32_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<int32_t>& adata) const
	{
		return read_number<int32_t>(akey, [&adata](const cJSON* ajson)
			{
				int32_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<int64_t>& adata) const
	{
		return read_number<int64_t>(akey, [&adata](const cJSON* ajson)
			{
				int64_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<int64_t>& adata) const
	{
		return read_number<int64_t>(akey, [&adata](const cJSON* ajson)
			{
				int64_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<int64_t>& adata) const
	{
		return read_number<int64_t>(akey, [&adata](const cJSON* ajson)
			{
				int64_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<uint8_t>& adata) const
	{
		return read_number<uint8_t>(akey, [&adata](const cJSON* ajson)
			{
				uint8_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<uint8_t>& adata) const
	{
		return read_number<uint8_t>(akey, [&adata](const cJSON* ajson)
			{
				uint8_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<uint8_t>& adata) const
	{
		return read_number<uint8_t>(akey, [&adata](const cJSON* ajson)
			{
				uint8_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<uint16_t>& adata) const
	{
		return read_number<uint16_t>(akey, [&adata](const cJSON* ajson)
			{
				uint16_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<uint16_t>& adata) const
	{
		return read_number<uint16_t>(akey, [&adata](const cJSON* ajson)
			{
				uint16_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<uint16_t>& adata) const
	{
		return read_number<uint16_t>(akey, [&adata](const cJSON* ajson)
			{
				uint16_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<uint32_t>& adata) const
	{
		return read_number<uint32_t>(akey, [&adata](const cJSON* ajson)
			{
				uint32_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<uint32_t>& adata) const
	{
		return read_number<uint32_t>(akey, [&adata](const cJSON* ajson)
			{
				uint32_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<uint32_t>& adata) const
	{
		return read_number<uint32_t>(akey, [&adata](const cJSON* ajson)
			{
				uint32_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<uint64_t>& adata) const
	{
		return read_number<uint64_t>(akey, [&adata](const cJSON* ajson)
			{
				uint64_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<uint64_t>& adata) const
	{
		return read_number<uint64_t>(akey, [&adata](const cJSON* ajson)
			{
				uint64_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<uint64_t>& adata) const
	{
		return read_number<uint64_t>(akey, [&adata](const cJSON* ajson)
			{
				uint64_t aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<float>& adata) const
	{
		return read_number<float>(akey, [&adata](const cJSON* ajson)
			{
				float aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<float>& adata) const
	{
		return read_number<float>(akey, [&adata](const cJSON* ajson)
			{
				float aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<float>& adata) const
	{
		return read_number<float>(akey, [&adata](const cJSON* ajson)
			{
				float aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<double>& adata) const
	{
		return read_number<double>(akey, [&adata](const cJSON* ajson)
			{
				double aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<double>& adata) const
	{
		return read_number<double>(akey, [&adata](const cJSON* ajson)
			{
				double aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<double>& adata) const
	{
		return read_number<double>(akey, [&adata](const cJSON* ajson)
			{
				double aval = 0;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}


	bool json_read::read(const char* akey, std::vector<bool>& adata) const
	{
		return read_number<bool>(akey, [&adata](const cJSON* ajson)
			{
				bool aval = true;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::list<bool>& adata) const
	{
		return read_number<bool>(akey, [&adata](const cJSON* ajson)
			{
				bool aval = true;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.push_back(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::set<bool>& adata) const
	{
		return read_number<bool>(akey, [&adata](const cJSON* ajson)
			{
				bool aval = true;
				if (json_read_number::fun_value(ajson, aval))
				{
					adata.insert(aval);
				}
			});
	}

	bool json_read::read(const char* akey, std::vector<std::string>& adata) const
	{
		cJSON* ltemp = nullptr;
		if (read(akey, ltemp) == false)
		{
			return false;
		}
		int lsize = cJSON_GetArraySize(ltemp);
		for (int i = 0; i < lsize; ++i)
		{
			const cJSON* ret = cJSON_GetArrayItem(ltemp, i);
			if (nullptr == ret || ret->type != cJSON_String)
			{
				continue;
			}
			std::string lval;
			adata.emplace_back(ret->valuestring);
		}
		return true;
	}

	bool json_read::read(const char* akey, std::list<std::string>& adata) const
	{
		cJSON* ltemp = nullptr;
		if (read(akey, ltemp) == false)
		{
			return false;
		}
		int lsize = cJSON_GetArraySize(ltemp);
		for (int i = 0; i < lsize; ++i)
		{
			const cJSON* ret = cJSON_GetArrayItem(ltemp, i);
			if (nullptr == ret || ret->type != cJSON_String)
			{
				continue;
			}
			std::string lval;
			adata.emplace_back(ret->valuestring);
		}
		return true;
	}

	bool json_read::read(const char* akey, std::set<std::string>& adata) const
	{
		cJSON* ltemp = nullptr;
		if (read(akey, ltemp) == false)
		{
			return false;
		}
		int lsize = cJSON_GetArraySize(ltemp);
		for (int i = 0; i < lsize; ++i)
		{
			const cJSON* ret = cJSON_GetArrayItem(ltemp, i);
			if (nullptr == ret || ret->type != cJSON_String)
			{
				continue;
			}
			std::string lval;
			adata.insert(ret->valuestring);
		}
		return true;
	}

}// namespace ngl