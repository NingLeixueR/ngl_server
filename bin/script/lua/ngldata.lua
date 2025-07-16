local cjson = require("cjson")

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
        visited = visited or {}  -- 记录已访问的 table，避免循环引用死循环
    
        -- 非 table 类型直接打印
        if type(t) ~= "table" then
            local str = tostring(t)
            -- 字符串加引号，方便区分
            if type(t) == "string" then
                str = '"' .. str .. '"'
            end
            print(string.rep("  ", indent) .. str)
            return
        end
    
        -- 处理循环引用
        if visited[t] then
            print(string.rep("  ", indent) .. "table (循环引用)")
            return
        end
        visited[t] = true
    
        local indent_str = string.rep("  ", indent)
        print(indent_str .. "{")  -- 开始打印 table
    
        -- 遍历 table 中的键值对
        for k, v in pairs(t) do
            -- 处理键的格式
            local key_str
            if type(k) == "string" and k:match("^[%a_][%a%d_]*$") then
                -- 合法标识符（字母/下划线开头）直接作为键
                key_str = k .. " = "
            else
                -- 其他类型的键用 [键] 表示（如数字、特殊字符字符串）
                key_str = "[" .. tostring(k) .. "] = "
            end
        
            -- 打印键和值（值递归处理）
            io.write(indent_str .. "  " .. key_str)
            self:print_table(v, indent + 1, visited)
        end
    
        print(indent_str .. "}")  -- 结束打印 table
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

         if aactorid == "-1" then
            for k,v in pairs(parsedData) do
                for k1,v1 in pairs(v) do
                    print("k1="..k1)
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
        print("#########################################")
        self:print_table(self.data)
        print("#########################################")
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
function ngldata.getInstance()
    if not ngldata._instance then
        ngldata._instance = new()
    end
    return ngldata._instance
end

return ngldata