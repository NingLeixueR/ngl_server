local cjson = require("cjson")

local dbdata = {}

-- 私有构造函数
local function new()
    local instance = 
    {
        data = {},  -- 数据
    }

    local readonlyMT = {
        __newindex = function()
            error("Cannot modify readonly table", 2)
        end,
        __metatable = false  -- 防止获取或修改元表
    }

    function instance:printf(data)
	    for k,v in pairs(data) do
		    if type(v) == "table" then
			    self:printf(v)
		    else
			    print(k, v)
		    end
	    end
    end

    function instance:push_data(adbname, adatajson)
        if not adatajson or adatajson == "" then
            print("Error: adatajson is nil or empty")
            return
        end
        
        local success, parsedData = pcall(function()
            return cjson.decode(adatajson)
        end)
        
        if not success then
            print("JSON decode error:", parsedData)
            parsedData = {}
        end
        
        self.data[adbname] = {
            data = parsedData,
            change = false
        }
        self:printf(self.data)
    end

    function instance:get(adbname)
        self.data[adbname]["change"] = true
       return self.data[adbname] and self.data[adbname]["data"] or nil
    end

    function instance:getconst(adbname)
       local data = self.data[adbname] and self.data[adbname]["data"] or nil
       if data then
           return setmetatable({}, {
                __index = data,
                __newindex = readonlyMT.__newindex,
                __metatable = readonlyMT.__metatable
           })
       end
       return nil
    end

    function instance:check_outdata(adbname)
        if self.data[adbname] then
            if self.data[adbname]["change"] then
                return true, cjson.encode(luaTable)
            end
        end
        return false, ""
    end

    return instance
end

-- 静态方法：获取单例实例
function dbdata.getInstance()
    if not dbdata._instance then
        dbdata._instance = new()
    end
    return dbdata._instance
end

return dbdata