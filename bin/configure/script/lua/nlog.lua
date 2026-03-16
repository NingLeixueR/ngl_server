-- File overview: Defines Lua script logic for nlog.

local nlog = {}
local instance = nil

-- Defaultconfig
local config = {
    log_file = nil,
    console_output = true,
    buffer_size = 0,
    log_buffer = {}
}

-- Writelogtofile
local function flush_logs()
    if not config.log_file or #config.log_buffer == 0 then return end
    
    local file = io.open(config.log_file, "a")
    if file then
        for _, line in ipairs(config.log_buffer) do
            file:write(line, "\n")
        end
        file:close()
        config.log_buffer = {}
    end
end

-- Recordlog
local function log(message)
    -- Outputtoconsole
    if config.console_output then
        print(message)
    end
    
    -- Addtobuffer
    table.insert(config.log_buffer, message)
    
    -- Ifbuffer to writefile
    if #config.log_buffer >= config.buffer_size then
        flush_logs()
    end
end

-- Setlogfile
function nlog:set_log_file(file_path)
    config.log_file = file_path
end

-- / Consoleoutput
function nlog:set_console_output(enable)
    config.console_output = enable
end

-- Setbuffer
function nlog:set_buffer_size(size)
    config.buffer_size = size
end

-- Flush immediatelybuffer
function nlog:flush()
    flush_logs()
end

-- Log
function nlog:write(msg)
    log(msg)
end

-- Getsingletoninstance
local function get_instance(log_file)
    if not instance then
        instance = setmetatable({}, { __index = nlog })
        if log_file then
            instance:set_log_file(log_file)
        end
        
        -- Registerexithandler
        local old_exit = os.exit
        os.exit = function(...)
            instance:flush()
            old_exit(...)
        end
    end
    return instance
end

return {
    get_instance = get_instance
}    