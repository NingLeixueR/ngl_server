* ¶ÔjsonµÄÐ´Óë¶Á
```
ngl::ijson ltempjson;
ltempjson << std::make_pair("appid", 44);
ltempjson << std::make_pair("uid", 1406739);
ltempjson << std::make_pair("sessionid", "i461qbg8pia3pe04fdenue1hu4");
std::string ljsonstr1;
std::string ljsonstr2;
ltempjson >> ljsonstr1;
ltempjson.set_nonformatstr(true);
ltempjson >> ljsonstr2;
// ljsonstr1
{
	"appid":	44,
	"uid":	1406739,
	"sessionid":	"i461qbg8pia3pe04fdenue1hu4"
}
// ljsonstr2
{"appid":44,"uid":1406739,"sessionid":"i461qbg8pia3pe04fdenue1hu4"}


ngl::ojson ltempjson2(ljsonstr2.c_str());
std::pair<const char*, int> lappid("appid", 0);
ltempjson2 >> lappid;
std::pair<const char*, int> luid("uid", 0);
ltempjson2 >> luid;
std::pair<const char*, std::string> lsessionid("sessionid", "");
ltempjson2 >> lsessionid;

```
