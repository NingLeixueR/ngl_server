local dbdata = require("dbdata").getInstance()
require("dbdata")

function handle(data)
	-- 如何获取db数据:明确需求是否修改
	-- 无需修改数据使用 dbdata:getconst("数据名称")
	-- 需修改数据使用 dbdata:get("数据名称")

	-- 如何发送数据给client,actor
	-- nguid:actor_type#areaid#dataid
	send_client("guid", "msgname", "msgjson")
	send_actor("guid", "msgname", "msgjson")

	dbdata:get("kkk","1")
	dbdata:get("kkk","777")
end
