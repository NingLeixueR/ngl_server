-- File overview: Defines Lua script logic for actor testlua2.

local ngldata = require("ngldata").getInstance()
require("ngldata")

function db_loadfinish()
    -- Dataloadcomplete
end

function handle(amsgname, amsgjson)
	-- Canthrough[amsgname] [amsgjson] messagehandle
	print("actor_testlua2.lua")
	print("msg name:" .. amsgname)
	print(amsgjson)
end