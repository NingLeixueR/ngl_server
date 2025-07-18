local ngldata = require("ngldata").getInstance()
require("ngldata")

function handle(amsgname, amsgjson)
	-- 如何获取db数据:明确需求是否修改
	-- 无需修改数据使用 ngldata:getconst("数据名称")
	-- 需修改数据使用 ngldata:get("数据名称")

	-- 如何发送数据给client,actor
	-- nguid:actor_type#areaid#dataid
	send_client("guid", "msgname", "msgjson")
	send_actor("guid", "msgname", "msgjson")

	ngldata:get("kkk","1")
	ngldata:get("kkk","777")

	-- 可以通过[amsgname]和[amsgjson]进行消息处理
	print("msg name:" .. amsgname)
	print(amsgjson)
end
