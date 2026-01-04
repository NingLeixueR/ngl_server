#include "ndefine.h"

namespace ngl
{
	struct bbbbb
	{
		int32_t m_1;
		bool m_2;
		std::vector<int32_t> m_3;

		def_jsonfunction_parm("m_1", m_1, "m_2", m_2, "m_3", m_3)
	};

	struct abcccc
	{
		int32_t m_1;
		bool m_2;
		std::vector<int32_t> m_3;
		std::vector<bbbbb> m_4;

		bool json_pop(cJSON* ajson) {
			return ngl::njson::pop(ajson, "m_1", m_1, "m_2", m_2, "m_3", m_3, "m_4", m_4);
		} void json_push(cJSON* ajson, const char* akey) const {
			if (akey != nullptr) {
				ngl::ncjson ltemp; cJSON* ret = cJSON_GetObjectItem(ltemp.json(), akey); if (nullptr == ret) {
					return;
				} ngl::njson::push(ret, "m_1", m_1, "m_2", m_2, "m_3", m_3, "m_4", m_4);
			}
			else {
				ngl::njson::push(ajson, "m_1", m_1, "m_2", m_2, "m_3", m_3, "m_4", m_4);
			}
		} bool json_pop(const char* ajson, const char* akey) {
			ngl::ncjson ltemp(ajson); if (akey != nullptr) {
				cJSON* ret = cJSON_GetObjectItem(ltemp.json(), akey); if (nullptr == ret) {
					return false;
				} return json_pop(ret);
			} return json_pop(ltemp.json());
		} 
		void json_push(std::string& ajson, const char* akey) const 
		{
			ngl::ncjson ltemp; 
			json_push(ltemp.json(), akey);
			ajson = ltemp.nonformatstr();
		}
	};

}