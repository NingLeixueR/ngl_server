local dbdata = require("dbdata").getInstance()
require("dbdata")

function handle(data)
	-- 如何获取db数据:明确需求是否修改
	-- 无需修改数据使用 dbdata:getconst("xxxxx")
	-- 需修改数据使用 dbdata:get("xxxxx")
	print("222222222222222222222222222222222222")
	ldata1 = dbdata:get("PROBUFF_NET_DELIVER_GOODS_RECHARGE")
	ldata1['mgold'] = 789;
	dbdata:printf(ldata1)
	print("2222222222222222222222222222222222222")
	print("3333333333333333333333333333333333333")
	ldata2 = dbdata:getconst("PROBUFF_NET_DELIVER_GOODS_RECHARGE")
	
	print(ldata2['mgold'])
	print("3333333333333333333333333333333333333")
end