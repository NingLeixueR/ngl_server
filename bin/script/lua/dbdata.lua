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

    function instance:printf(adata)
	    for k,v in pairs(adata) do
		    if type(v) == "table" then
			    self:printf(v)
		    else
			    print(k, v)
		    end
	    end
    end

    function instance:json_decode(adatajson)
        local success, parsedData = pcall(function()
            return cjson.decode(adatajson)
        end)
        
        if not success then
            print("JSON decode error:", adatajson)
            return {}
        end
        return parsedData
    end
    -- cjson.encode(self.data[adbname][aactorid]["data"])
    function instance:json_encode(adata)
        return cjson.encode(adata)
    end

    function instance:push_data(adbname, aactorid, adatajson)
        if not adatajson or adatajson == "" then
            print("Error: adatajson is nil or empty")
            return
        end
        
        local parsedData = self:json_decode(adatajson)
        
        if not self.data[adbname] then
            self.data[adbname] = {}
        end

        self.data[adbname][aactorid] = {
            parsed_data = parsedData,
            change = false
        }
    end

    function instance:get(adbname, aactorid)
       if self.data[adbname][aactorid] then
          self.data[adbname][aactorid]["change"] = true
          return self.data[adbname][aactorid]["parsed_data"]
       end
       return nil
    end

    function instance:getconst(adbname, aactorid)
       local data = self.data[adbname][aactorid] and self.data[adbname][aactorid]["parsed_data"] or nil
       if data then
           return setmetatable({}, {
                __index = data,
                __newindex = readonlyMT.__newindex,
                __metatable = readonlyMT.__metatable
           })
       end
       return nil
    end

    function instance:check_outdata(adbname, aactorid)
        if self.data[adbname][aactorid] then
            if self.data[adbname][aactorid]["change"] then
                return true, self:json_encode(self.data[adbname][aactorid]["data"])
            end
        end
        return false, ""
    end

    function instance:check_outdata_all(adbname)
        local ret = {}
        ret["m_data"] = {}
        if self.data[adbname] then
            for k,v in pairs(self.data[adbname]) do
                print(k)
                self:printf(v)
		        if v["change"] then
                    ret["m_data"][k] = self:json_encode(v["parsed_data"])
                    v["change"] = false
		        end
	        end
        end
        return true, self:json_encode(ret)
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