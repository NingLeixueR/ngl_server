local ngldata = require("ngldata").getInstance()
require("ngldata")

function db_loadfinish()
    -- 数据加载完成
end

function handle(amsgname, amsgjson)
	-- 可以通过[amsgname]和[amsgjson]进行消息处理
	print("actor_testlua.lua")
	print("msg name:" .. amsgname)
	print(amsgjson)
end
