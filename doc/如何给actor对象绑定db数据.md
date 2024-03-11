## 如何给actor对象绑定db数据
拿actor_login举例<br/>
1. 在db.proto文件中定义要存储的数据结构与其对应的枚举<br/>
```
enum ENUM_DB
{
	ENUM_DB_ACCOUNT = 0;		// 账号数据
	ENUM_DB_ROLE = 1;		// 玩家数据
	ENUM_DB_BRIEF = 2;		// 玩家简要数据
	ENUM_DB_BAG = 3;		// 背包数据
	ENUM_DB_KEYVALUE = 4;		// 键值数据
	ENUM_DB_MAIL = 5;		// 邮件数据
	ENUM_DB_GUILD = 6;		// 工会数据
	ENUM_DB_NOTICE = 7;		// 公告数据
	ENUM_DB_RANKLIST = 8;		// 排行数据
	ENUM_DB_ACTIVITY = 9;		// 活动数据
	ENUM_DB_TASK = 10;		// 任务数据
	ENUM_DB_COUNT = 11;
};
// ENUM_DB_ACCOUNT,				// 账号数据
message db_account
{
	optional int64	m_id			= 1;				// 帐号id
	optional int64	m_roleid		= 2;				// 角色id
	optional string m_account		= 3;				// 账号
	optional string m_passworld		= 4;				// 密码
	optional int32	m_area			= 5;				// 区服id
}
```
2. 在函数[void init_protobuf::init_customs()]中对其进行绑定，<br/>
让其支持网络序列化<br/>
自动生成协议号<br/>
init_customs_db<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account>();
3. 实现ndb_modular<>用于数据管理类的父类<br/>
```
using account_db_modular = ndb_modular<
	EPROTOCOL_TYPE_PROTOCOLBUFF,  //数据格式  此处位protobuf
	pbdb::ENUM_DB_ACCOUNT,        //刚定义的数据枚举
	pbdb::db_account,             //刚定义的数据结构
	actor_login                   //其绑定的actor对象
>;
```
4. 定义数据管理类account<br/>
```
class account : public account_db_modular
```
其中<br/>
virtual void initdata();会在数据加载成功后被调用，我们可以在此处对数据进行索引<br/>
virtual void set_id();用来指定调用的数据的id，此处为-1表示加载全部数据<br/>
<br/>
至此“actor对象绑定db数据”就完成了<br/>
如何使用，就要修改、使用、删除数据就需要借助[ndb_modular](https://github.com/NingLeixueR/ngl/wiki/%E5%9F%BA%E6%9C%AC%E7%BB%93%E6%9E%84#ndb_modular)基类提供的方法

