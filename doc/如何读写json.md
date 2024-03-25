1. 对结构体序列化json需要借助[define_json.h]中的宏
```
	struct abc
	{
		int m_1;
		float m_2;
		std::string m_3;
		std::vector<int32_t> m_4;
		std::map<int32_t, float> m_5;

		jsonfunc( "_1", m_1, "_2", m_2, "_3", m_3, "_4", m_4,"_5", m_5)
	};
```
2. json的写
```
	ngl::ijson lijson;
	std::vector<int32_t> lv;
	for (int i = 0; i < 10; ++i)
		lv.push_back(i);
	lijson.write("test", lv);

	abc ltemp;
	ltemp.m_1 = 1989;
	ltemp.m_2 = 123.3;
	ltemp.m_3 = "chinachina";
	ltemp.m_4.push_back(2);
	ltemp.m_4.push_back(3);
	ltemp.m_5.insert(std::make_pair(1, 1.1));
	ltemp.m_5.insert(std::make_pair(2, 2.1));

	ltemp.write(lijson, "kkk");

	std::vector<abc> lvecabc;
	lvecabc.push_back(ltemp);
	ltemp.m_1 += 1000;
	lvecabc.push_back(ltemp);
	lijson.write("kkk2", lvecabc);

	std::string ljson;
	lijson.get(ljson);

	// ljson 如下
	{
	"test":	[0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
	"kkk":	{
		"_1":	1989,
		"_2":	123.30000305175781,
		"_3":	"chinachina",
		"_4":	[2, 3],
		"_5_key":	[1, 2],
		"_5_val":	[1.1000000238418579, 2.0999999046325684]
	},
	"kkk2":	[{
			"_1":	1989,
			"_2":	123.30000305175781,
			"_3":	"chinachina",
			"_4":	[2, 3],
			"_5_key":	[1, 2],
			"_5_val":	[1.1000000238418579, 2.0999999046325684]
		}, {
			"_1":	2989,
			"_2":	123.30000305175781,
			"_3":	"chinachina",
			"_4":	[2, 3],
			"_5_key":	[1, 2],
			"_5_val":	[1.1000000238418579, 2.0999999046325684]
		}]
}
	```

	3. json的读
	```
	ngl::ojson lojson(ljson.c_str());
	std::vector<int32_t> lv2;
	lojson.read("test", lv2);
	abc ltemp2;
	ltemp2.read(lojson, "kkk");
	std::vector<abc> lvecabc2;
	lojson.read("kkk2", lvecabc2);
	```
