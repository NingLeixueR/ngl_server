local nlog = {}
local instance = nil

-- Ĭ������
local config = {
    log_file = nil,
    console_output = true,
    buffer_size = 0,
    log_buffer = {}
}

-- д����־���ļ�
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

-- ��¼��־
local function log(message)
    -- ���������̨
    if config.console_output then
        print(message)
    end
    
    -- ��ӵ�������
    table.insert(config.log_buffer, message)
    
    -- ����������ﵽ������д���ļ�
    if #config.log_buffer >= config.buffer_size then
        flush_logs()
    end
end

-- ������־�ļ�
function nlog:set_log_file(file_path)
    config.log_file = file_path
end

-- ����/���ÿ���̨���
function nlog:set_console_output(enable)
    config.console_output = enable
end

-- ���û�������С
function nlog:set_buffer_size(size)
    config.buffer_size = size
end

-- ����ˢ�»�����
function nlog:flush()
    flush_logs()
end

-- ��־����
function nlog:write(msg)
    log(msg)
end

-- ��ȡ����ʵ��
local function get_instance(log_file)
    if not instance then
        instance = setmetatable({}, { __index = nlog })
        if log_file then
            instance:set_log_file(log_file)
        end
        
        -- ע���˳�������
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