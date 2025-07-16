local cjson = require("cjson")

local ngldata = {}

-- ˽�й��캯��
local function new()
    local instance = 
    {
        data = {},  -- ����
    }

    local readonlyMT = {
        __newindex = function()
            error("Cannot modify readonly table", 2)
        end,
        __metatable = false  -- ��ֹ��ȡ���޸�Ԫ��
    }

    function instance:print_table(t, indent, visited)
        indent = indent or 0
        visited = visited or {}  -- ��¼�ѷ��ʵ� table������ѭ��������ѭ��
    
        -- �� table ����ֱ�Ӵ�ӡ
        if type(t) ~= "table" then
            local str = tostring(t)
            -- �ַ��������ţ���������
            if type(t) == "string" then
                str = '"' .. str .. '"'
            end
            print(string.rep("  ", indent) .. str)
            return
        end
    
        -- ����ѭ������
        if visited[t] then
            print(string.rep("  ", indent) .. "table (ѭ������)")
            return
        end
        visited[t] = true
    
        local indent_str = string.rep("  ", indent)
        print(indent_str .. "{")  -- ��ʼ��ӡ table
    
        -- ���� table �еļ�ֵ��
        for k, v in pairs(t) do
            -- ������ĸ�ʽ
            local key_str
            if type(k) == "string" and k:match("^[%a_][%a%d_]*$") then
                -- �Ϸ���ʶ������ĸ/�»��߿�ͷ��ֱ����Ϊ��
                key_str = k .. " = "
            else
                -- �������͵ļ��� [��] ��ʾ�������֡������ַ��ַ�����
                key_str = "[" .. tostring(k) .. "] = "
            end
        
            -- ��ӡ����ֵ��ֵ�ݹ鴦��
            io.write(indent_str .. "  " .. key_str)
            self:print_table(v, indent + 1, visited)
        end
    
        print(indent_str .. "}")  -- ������ӡ table
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

-- ��̬��������ȡ����ʵ��
function ngldata.getInstance()
    if not ngldata._instance then
        ngldata._instance = new()
    end
    return ngldata._instance
end

return ngldata