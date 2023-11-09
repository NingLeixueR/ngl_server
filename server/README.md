# 目录简介

## ./idl
	* csv表格工具项目，用来根据.idl配置生成对应的表格文件，与加载表格的源文件

## ./libmycorekkk
	* rebuild.bat 生成的ub.cpp 可以加快编译速度

## ./makeproto
	* 曾将为了将proto文件绑定lua,过程中我删除了对lua的支持  所以其中只有以下代码是有作用的
	```
	// 生成对应的sql文件
    ngl::writefile lsql("create_db.sql");
    lsql.write(g_stream_sql.str());
    // 自动关联结构体为其提供协议号    
    ngl::xmlprotocol::load();
    int32_t lnumber = 0;
    foreachProtobuf(sourceTree, lnumber, "net");
    lnumber = 100000000;
    foreachProtobuf(sourceTree, lnumber, "gm");
	```

	

	



