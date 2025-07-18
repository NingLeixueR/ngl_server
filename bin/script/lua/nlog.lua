local nlog = {}
local instance = nil

-- 默认配置
local config = {
    log_file = nil,
    console_output = true,
    buffer_size = 0,
    log_buffer = {}
}

-- 写入日志到文件
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

-- 记录日志
local function log(message)
    -- 输出到控制台
    if config.console_output then
        print(message)
    end
    
    -- 添加到缓冲区
    table.insert(config.log_buffer, message)
    
    -- 如果缓冲区达到限制则写入文件
    if #config.log_buffer >= config.buffer_size then
        flush_logs()
    end
end

-- 设置日志文件
function nlog:set_log_file(file_path)
    config.log_file = file_path
end

-- 启用/禁用控制台输出
function nlog:set_console_output(enable)
    config.console_output = enable
end

-- 设置缓冲区大小
function nlog:set_buffer_size(size)
    config.buffer_size = size
end

-- 立即刷新缓冲区
function nlog:flush()
    flush_logs()
end

-- 日志方法
function nlog:write(msg)
    log(msg)
end

-- 获取单例实例
local function get_instance(log_file)
    if not instance then
        instance = setmetatable({}, { __index = nlog })
        if log_file then
            instance:set_log_file(log_file)
        end
        
        -- 注册退出处理函数
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