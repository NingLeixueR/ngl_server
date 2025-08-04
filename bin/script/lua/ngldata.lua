local cjson = require("cjson")
local logger = require("nlog").get_instance("./log/lua/ngldata"..os.date("%Y_%m_%d__%H_%M_%S")..".json") -- ��ȡ��־ʵ��

local ngldata = {}

-- ˽�й��캯��
local function new()
    local instance = 
    {
        data = {},              -- ����
        edit = {},              -- �����Ƿ������༭
        change = {},            -- ��Щ���ݱ��ı�
        del = {},               -- ��Щ���ݱ�ɾ��
        data_source = {},       -- ������Դ��db,csv,dbnsp  ���������Դdbnsp,�޸����ݺ��Զ�����auto_save����


        sysdata = {},
    }

    local readonlyMT = {
        __newindex = function()
            error("Cannot modify readonly table", 2)
        end,
        __metatable = false  -- ��ֹ��ȡ���޸�Ԫ��
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
			logger:write(indent_str .. "table (ѭ������)")
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

    function instance:push_sysdata(asysjson)
        self.sysdata = self:json_decode(asysjson)
    end

    function instance:data_push(aname, asource, ajson, aedit)
        logger:write("instance:push_data("..adbname..","..adatajson..(aedit and "true)" or "false)"))
        if not ajson or ajson == "" then
            logger:write("Error: ajson is nil or empty")
            return
        end
        
        local parsedData = self:json_decode(ajson)
        
        if not self.data[aname] then
            self.data[aname] = {}
            self.edit[aname] = aedit
            self.change[aname] = {}
            self.del[aname] = {}
        end

        for k,v in pairs(parsedData) do
            for k1,v1 in pairs(v) do
                self.data[aname][k1] = {
                    parsed_data = v1
                }
            end
	    end

        
        self.data_source[aname] = asource
        
        logger:write("##"..aname.."##")
        self:print_table(parsedData)
        logger:write("####")
    end

    function instance:get(aname, adataid)
       logger:write("instance:get("..aname..","..adataid..")")
       if self.edit[aname] == false then
            return nil;
       end

       if self.data[aname] and self.data[aname][adataid] then
            self.change[aname][adataid] = true
            return self.data[aname][adataid]["parsed_data"]
       end
       return nil
    end

    function instance:getconst(aname, adataid)
        logger:write("instance:getconst("..aname..","..adataid..")")
        if self.data[aname] and self.data[aname][adataid] then
            local data = self.data[aname][adataid] and self.data[aname][adataid]["parsed_data"] or nil
            if data ~= nil then
                self:print_table(data)
                return setmetatable({}, {
                    __index = data,
                    __newindex = readonlyMT.__newindex,
                    __metatable = readonlyMT.__metatable,
                    __pairs = function(t)
                        return pairs(data)
                    end
                })
            end
        end
        return nil
    end

    -- parm iscpp �Ƿ�cpp����
    function instance:data_del(aname, adataid, iscpp)
        logger:write("instance:data_del("..aname..","..adataid)
        if self.data[aname][adataid] then
            self.data[aname][adataid] = nil
        end
        if self.change[aname][adataid] then
            self.change[aname][adataid] = nil
        end
        if iscpp == nil then
            self.del[aname][adataid] = true
        else
            self.del[aname][adataid] = nil
        end
    end

    -- ��ȡ���б仯����
    function instance:data_checkout(aname)
        logger:write("instance:check_outdata("..aname..")")
        if self.edit[aname] == false then
            return false, ""
        end
        ret = {}
        self:print_table(self.change[aname])
        ret[aname] = {}
        retbool = false
        if self.data[aname] then
            for k,v in pairs(self.change[aname]) do
                if v then
                    ret[aname][k] = self.data[aname][k]["parsed_data"]
                    retbool = true
		        end
            end
            self.change[aname] = {}
            if retbool then
                return true, self:json_encode(ret)
            end
        end
        return false, ""
    end

	-- ����ָ���仯����
    function instance:data_checkoutbyid(aname, adataid)
        logger:write("instance:data_checkoutbyid("..aname..", "..adataid..")")
        if self.edit[aname] == false then
            return false, ""
        end
        ret = {}
        self:print_table(self.change[aname])
        
        ret[aname] = {}
        retbool = false
        if self.change[aname] then
            if self.change[aname][adataid] then
			    ret[aname][adataid] = self.data[aname][adataid]["parsed_data"]
			    self.change[aname][adataid] = nil
			    retbool = true
            end
        end
        if retbool then
			return true, self:json_encode(ret)
        end
        return false, ""
    end

    function instance:data_checkdel(aname)
        logger:write("instance:data_checkdel("..aname..")")
        if self.edit[aname] == false then
            return false, ""
        end
        ret = {}
        ret[aname] = {}
        retbool = false
        if self.del[aname] then
            for k,v in pairs(self.del[aname]) do
                if v then
                    table.insert(ret[aname], k)
                    retbool = true
	            end
            end
            self.del[aname] = {}
            if retbool then
                return true, self:json_encode(ret)
            end
        end
        return false, ""
    end

    function instance:data_checkdelbyid(aname, adataid)
        logger:write("instance:data_checkdelbyid("..aname..", "..adataid..")")
        if self.edit[aname] == false then
            return false, ""
        end
        ret = {}
        ret[aname] = {}
        retbool = false
        if self.del[aname][adataid] then
            table.insert(ret[aname], k)
            retbool = true
        end
        self.del[aname][adataid] = nil
        if retbool then
            return true, self:json_encode(ret)
        end
        return false, ""
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