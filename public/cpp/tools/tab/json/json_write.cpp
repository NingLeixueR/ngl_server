#include "json_write.h"
#include "tools.h"

namespace ngl
{
	json_write::json_write() :
		m_json(cJSON_CreateObject()),
		m_nonformatstr(nullptr),
		m_str(nullptr),
		m_isnonformatstr(false),
		m_free(true)
	{
	}

	void free_json(const char* m_str)
	{
		free((void*)m_str);
	}

	json_write::~json_write()
	{
		free_nonformatstr();
		free_str();

		if (m_json != nullptr && m_free)
		{
			cJSON_Delete(m_json);
			m_json = nullptr;
		}
	}

	cJSON* json_write::get()
	{
		return m_json;
	}

	void json_write::write(const char* akey, const int8_t aval)
	{
		cJSON_AddNumberToObject(m_json, akey, aval);
	}

	void json_write::write(const char* akey, const int16_t aval)
	{
		cJSON_AddNumberToObject(m_json, akey, aval);
	}
	
	void json_write::write(const char* akey, const int32_t aval)
	{
		cJSON_AddNumberToObject(m_json, akey, aval);
	}
		
	void json_write::write(const char* akey, const int64_t aval)
	{
		std::string lvalue = tools::lexical_cast<std::string>(aval);
		write(akey, lvalue);
	}
		
	void json_write::write(const char* akey, const uint8_t aval)
	{
		cJSON_AddNumberToObject(m_json, akey, aval);
	}
		
	void json_write::write(const char* akey, const uint16_t aval)
	{
		std::string lvalue = tools::lexical_cast<std::string>(aval);
		write(akey, lvalue);
	}
		
	void json_write::write(const char* akey, const uint32_t aval)
	{
		cJSON_AddNumberToObject(m_json, akey, aval);
	}
		
	void json_write::write(const char* akey, const uint64_t aval)
	{
		std::string lvalue = tools::lexical_cast<std::string>(aval);
		write(akey, lvalue);
	}
		
	void json_write::write(const char* akey, const float aval)
	{
		cJSON_AddNumberToObject(m_json, akey, aval);
	}
		
	void json_write::write(const char* akey, const double aval)
	{
		cJSON_AddNumberToObject(m_json, akey, aval);
	}

	void json_write::write(const char* akey, const char* aval)
	{ 
		cJSON_AddItemToObject(m_json, akey, cJSON_CreateString(aval));
	}

	void json_write::write(const char* akey, const bool aval)
	{
		cJSON_AddItemToObject(m_json, akey, 
			aval? cJSON_CreateTrue() : cJSON_CreateFalse()
		);
	}

	void json_write::write(const char* akey, cJSON* aval)
	{
		cJSON_AddItemToObject(m_json, akey, aval);
	}

	void json_write::write(const char* akey, json_write& aval)
	{
		cJSON_AddItemToObject(m_json, akey, aval.nofree());
	}

	void json_write::write(const char* akey, const std::string& aval)
	{
		return write(akey, aval.c_str());
	}

	void json_write::writenumber(const char* akey, const std::vector<int32_t>& aval)
	{
		cJSON* ljson = cJSON_CreateIntArray(aval.data(), (int32_t)aval.size());
		write(akey, ljson);
	}

	void json_write::writenumber(const char* akey, const std::vector<uint32_t>& aval)
	{
		cJSON* ljson = cJSON_CreateIntArray((int32_t*)aval.data(), aval.size());
		write(akey, ljson);
	}

	void json_write::write(const char* akey, const std::vector<int8_t>& aval)
	{
		std::vector<int32_t> lvec(aval.begin(), aval.end());
		writenumber(akey, lvec);
	}

	void json_write::write(const char* akey, const std::vector<int16_t>& aval)
	{
		std::vector<int32_t> lvec(aval.begin(), aval.end());
		writenumber(akey, lvec);
	}

	void json_write::write(const char* akey, const std::vector<int32_t>& aval)
	{
		writenumber(akey, aval);
	}

	void json_write::write(const char* akey, const std::vector<uint8_t>& aval)
	{
		std::vector<uint32_t> lvec(aval.begin(), aval.end());
		writenumber(akey, lvec);
	}

	void json_write::write(const char* akey, const std::vector<uint16_t>& aval)
	{
		std::vector<uint32_t> lvec(aval.begin(), aval.end());
		writenumber(akey, lvec);
	}

	void json_write::write(const char* akey, const std::vector<uint32_t>& aval)
	{
		writenumber(akey, aval);
	}

	void json_write::write(const char* akey, const std::vector<float>& aval)
	{
		cJSON* ljson = cJSON_CreateFloatArray(aval.data(), aval.size());
		write(akey, ljson);
	}

	void json_write::write(const char* akey, const std::vector<double>& aval)
	{
		cJSON* ljson = cJSON_CreateDoubleArray(aval.data(), aval.size());
		write(akey, ljson);
	}

	void json_write::write(const char* akey, const std::vector<int64_t>& aval)
	{
		cJSON* ljson = cJSON_CreateArray();
		for (int64_t item : aval)
		{
			std::string lnumber = tools::lexical_cast<std::string>(item);
			cJSON_AddItemToArray(ljson, cJSON_CreateString(lnumber.c_str()));
		}
		write(akey, ljson);
	}

	void json_write::write(const char* akey, const std::vector<uint64_t>& aval)
	{
		cJSON* ljson = cJSON_CreateArray();
		for (int64_t item : aval)
		{
			std::string lnumber = tools::lexical_cast<std::string>(item);
			cJSON_AddItemToArray(ljson, cJSON_CreateString(lnumber.c_str()));
		}
		write(akey, ljson);
	}

	void json_write::write(const char* akey, const std::vector<std::string>& aval)
	{
		cJSON* ljson = cJSON_CreateArray();
		for (const std::string& item : aval)
		{
			cJSON_AddItemToArray(ljson, cJSON_CreateString(item.c_str()));
		}
		write(akey, ljson);
	}

	void json_write::get(std::string& adata)
	{
		if (m_isnonformatstr)
		{
			free_nonformatstr();
			m_nonformatstr = cJSON_PrintUnformatted(m_json);
			adata = m_nonformatstr;
		}
		else
		{
			free_str();
			m_str = cJSON_Print(m_json);
			adata = m_str;
		}
	}

	void json_write::set_nonformatstr(bool abool)
	{
		m_isnonformatstr = abool;
	}

	void json_write::write()
	{
	}

	void json_write::free_str()
	{
		if (m_str != nullptr)
		{
			free((void*)m_str);
			m_str = nullptr;
		}
	}

	void json_write::free_nonformatstr()
	{
		if (m_nonformatstr != nullptr)
		{
			free((void*)m_nonformatstr);
			m_nonformatstr = nullptr;
		}
	}

	cJSON* json_write::nofree()
	{
		m_free = false;
		return m_json;
	}
}//namespace ngl