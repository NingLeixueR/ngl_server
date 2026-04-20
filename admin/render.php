<?php
function render_response($results, $title, $back_url)
{
    echo '<html><head><meta charset="UTF-8"><title>' . htmlspecialchars($title) . '</title>';
    echo '<link rel="stylesheet" href="../style.css"></head><body>';
    echo '<h2>' . htmlspecialchars($title) . ' <a href="' . htmlspecialchars($back_url) . '">返回</a></h2>';

    if (empty($results))
    {
        echo '<p class="fail">未收到服务器响应</p>';
        echo '</body></html>';
        return;
    }

    foreach ($results as $resp)
    {
        echo '<div class="card">';
        if (isset($resp['server_name']))
        {
            echo '<h3>' . htmlspecialchars($resp['server_name']) . '</h3>';
        }

        $data = isset($resp['data']) ? $resp['data'] : $resp;

        if (is_bool($data))
        {
            echo '<p>' . ($data ? '<span class="ok">操作成功</span>' : '<span class="fail">操作失败</span>') . '</p>';
        }
        else if (is_string($data) || is_numeric($data))
        {
            echo '<p>' . htmlspecialchars($data) . '</p>';
        }
        else if (is_array($data))
        {
            render_data_table($data);
        }
        else
        {
            echo '<pre>' . htmlspecialchars(json_encode($resp, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT)) . '</pre>';
        }
        echo '</div>';
    }
    echo '</body></html>';
}

function render_data_table($data)
{
    if (empty($data))
    {
        echo '<p>无数据</p>';
        return;
    }

    // Check if it's a list of objects (indexed array of arrays)
    if (isset($data[0]) && is_array($data[0]))
    {
        $keys = array_keys($data[0]);
        echo '<table><tr>';
        foreach ($keys as $k) echo '<th>' . htmlspecialchars($k) . '</th>';
        echo '</tr>';
        foreach ($data as $row)
        {
            echo '<tr>';
            foreach ($keys as $k)
            {
                $v = isset($row[$k]) ? $row[$k] : '';
                if (is_array($v)) $v = json_encode($v, JSON_UNESCAPED_UNICODE);
                echo '<td>' . htmlspecialchars($v) . '</td>';
            }
            echo '</tr>';
        }
        echo '</table>';
    }
    else
    {
        // Key-value pairs
        echo '<table><tr><th>字段</th><th>值</th></tr>';
        foreach ($data as $k => $v)
        {
            if (is_array($v)) $v = json_encode($v, JSON_UNESCAPED_UNICODE);
            echo '<tr><td>' . htmlspecialchars($k) . '</td><td>' . htmlspecialchars($v) . '</td></tr>';
        }
        echo '</table>';
    }
}
?>
