local cjson = require("cjson")
local logger = require("nlog").get_instance("./log/lua/ngldata"..os.date("%Y_%m_%d__%H_%M_%S")..".json") -- 获取日志实例

local ngldata = {}

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


    function instance:print_table(t, indent, visited)
		indent = indent or 0
		visited = visited or {}
		local indent_str = string.rep("  ", indent)

		if type(t) ~= "table" then
			local str = tostring(t)
			if type(t) == "string" then str = '"' .. str .. '"' end
			logger:write(indent_str .. str)
			return
		end

		if visited[t] then
			logger:write(indent_str .. "table (循环引用)")
			return
		end
		visited[t] = true

		logger:write(indent_str .. "{")
		
		for k, v in pairs(t) do
			local key_str
			if type(k) == "string" and k:match("^[%a_][%a%d_]*$") then
				key_str = k .. " = "
			else
				key_str = "[" .. tostring(k) .. "] = "
			end
			
			local line_start = indent_str .. "  " .. key_str
			if type(v) == "table" then
				logger:write(line_start)
				self:print_table(v, indent + 1, visited)
			else
				local value_str = tostring(v)
				if type(v) == "string" then value_str = '"' .. value_str .. '"' end
				logger:write(line_start .. value_str)
			end
		end

		logger:write(indent_str .. "}")
	end

    function instance:json_decode(adatajson)
        local success, parsedData = pcall(function()
            return cjson.decode(adatajson)
        end)
        
        if not success then
            logger:write("JSON decode error:", adatajson)
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
            logger:write("Error: adatajson is nil or empty")
            return
        end
        
        local parsedData = self:json_decode(adatajson)
        
        if not self.data[adbname] then
            self.data[adbname] = {}
        end

         if aactorid == "-1" then
            for k,v in pairs(parsedData) do
                for k1,v1 in pairs(v) do
                    self.data[adbname][k1] = {
                        parsed_data = v1,
                        change = false
                    }
                end
	        end
        else
            for k,v in pairs(parsedData) do
                self.data[adbname][aactorid] = {
                    parsed_data = v,
                    change = false
                }
	        end
        end
		if aactorid == "-1" then
            logger:write("##"..adbname.."##")
			self:print_table(self.data[adbname])
		else
            logger:write("##"..adbname.."("..aactorid..")##")
			self:print_table(self.data[adbname][aactorid])
		end
        logger:write("####")
    end

    function instance:get(adbname, aactorid)
       if aactorid == "-1" then
            if self.data[adbname] then
                return self.data[adbname]
            end
       else
           --self:print_table(self.data[adbname])
           --self:print_table(self.data[adbname][aactorid])
           if self.data[adbname] and self.data[adbname][aactorid] then
              self.data[adbname][aactorid]["change"] = true
              return self.data[adbname][aactorid]["parsed_data"]
           end
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
        ret = {}
        if aactorid == "-1" then
            ret[adbname] = {}
            if self.data[adbname] then
                local retbool = false
                for k,v in pairs(self.data[adbname]) do
                    self:print_table(v)
		            if v["change"] then
                        ret[adbname][k] = self:json_encode(v["parsed_data"])
                        v["change"] = false
                        retbool = true
		            end
	            end
                if retbool then
                    return true, self:json_encode(ret)
                end
            end
        else
            ret[adbname] = {}
            if self.data[adbname][aactorid]["change"] then
                ret[adbname][aactorid] = self:json_encode(v["parsed_data"])
                self.data[adbname][aactorid]["change"] = false
                return true, self:json_encode(ret)
            end
        end
        return false, ""
    end

    return instance
end

-- 静态方法：获取单例实例
function ngldata.getInstance()
    if not ngldata._instance then
        ngldata._instance = new()
    end
    return ngldata._instance
end

return ngldata