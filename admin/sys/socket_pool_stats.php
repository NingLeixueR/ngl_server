<?php
require_once dirname(__FILE__) . '/../auth.php'; check_action(1502);
gm_log('Socket缓冲池状态', 'server=' . $_POST['server']);
include '../socketbyte.php';

$so = new socketbyte();
if($so->connectServer($_POST['server']) == false)
{
    echo "connect err!!!";
    return;
}

$arr = array(
    'actor_name' => 'ACTOR_GM',
    'operator' => 'socket_pool_stats',
	'servertype' => $_POST['servertype'],
    'data' => array()
);

$json = json_encode($arr);
$so->send($json);
$results = $so->wait_response_all(count($_POST['servertype']));

// Custom rendering for socket pool stats
echo '<html><head><meta charset="UTF-8"><title>Socket缓冲池状态</title>';
echo '<link rel="stylesheet" href="../style.css">';
echo '<style>
.bucket-table { margin-top: 20px; }
.bucket-table th { background-color: #4CAF50; color: white; padding: 10px; }
.bucket-table td { padding: 8px; text-align: right; }
.bucket-table tr:nth-child(even) { background-color: #f2f2f2; }
.warning { color: #ff9800; font-weight: bold; }
.danger { color: #f44336; font-weight: bold; }
.summary-table { margin-bottom: 30px; }
</style></head><body>';
echo '<h2>Socket缓冲池状态 <a href="socket_pool_stats_html.php">返回</a></h2>';

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

    if (is_array($data))
    {
        // Display summary stats
        echo '<h4>总体统计</h4>';
        echo '<table class="summary-table"><tr><th>指标</th><th>值</th></tr>';
		
        $summary_keys = array('alloc_count', 'free_count', 'cache_hit', 'cache_miss', 'hit_rate', 'large_alloc', 'alloc_fail');
        $labels = array(
            'alloc_count' => '总分配次数',
            'free_count' => '总释放次数',
            'cache_hit' => '缓存命中',
            'cache_miss' => '缓存未命中',
            'hit_rate' => '命中率 (%)',
            'large_alloc' => '超大分配',
            'alloc_fail' => '分配失败'
        );
        foreach ($summary_keys as $key)
        {
            if (isset($data[$key]))
            {
                $label = isset($labels[$key]) ? $labels[$key] : $key;
                $value = $data[$key];
                if ($key === 'hit_rate') $value = number_format($value, 2);
                echo '<tr><td>' . htmlspecialchars($label) . '</td><td>' . htmlspecialchars($value) . '</td></tr>';
            }
        }
        echo '</table>';

        // Display bucket stats
        if (isset($data['buckets']) && is_array($data['buckets']) && !empty($data['buckets']))
        {
            echo '<h4>桶统计 (按大小)</h4>';
            echo '<table class="bucket-table">';
            echo '<tr>';
            echo '<th>桶大小 (字节)</th>';
            echo '<th>配置容量</th>';
            echo '<th>当前分配数</th>';
            echo '<th>峰值分配数</th>';
            echo '<th>当前缓存数</th>';
            echo '<th>利用率</th>';
            echo '<th>状态</th>';
            echo '</tr>';

            foreach ($data['buckets'] as $bucket)
            {
                $size = $bucket['bucket_size'];
                $capacity = $bucket['bucket_capacity'];
                $allocated = $bucket['current_allocated'];
                $peak = $bucket['peak_allocated'];
                $cached = $bucket['current_cached'];

                // Calculate utilization based on peak vs capacity
                $utilization = $capacity > 0 ? ($peak / $capacity * 100) : 0;
                $util_str = number_format($utilization, 1) . '%';

                // Determine status
                $status = '正常';
                $status_class = '';
                if ($allocated > $capacity * 2)
                {
                    $status = '超载';
                    $status_class = 'danger';
                }
                else if ($allocated > $capacity)
                {
                    $status = '高负载';
                    $status_class = 'warning';
                }

                // Highlight if cached is much larger than allocated (potential trim candidate)
                $cache_ratio = $allocated > 0 ? ($cached / $allocated) : 0;
                if ($cache_ratio > 3)
                {
                    $cached = '<span class="warning">' . $cached . ' (过多)</span>';
                }

                echo '<tr>';
                echo '<td>' . $size . '</td>';
                echo '<td>' . $capacity . '</td>';
                echo '<td>' . $allocated . '</td>';
                echo '<td>' . $peak . '</td>';
                echo '<td>' . $cached . '</td>';
                echo '<td>' . $util_str . '</td>';
                echo '<td class="' . $status_class . '">' . $status . '</td>';
                echo '</tr>';
            }
            echo '</table>';
        }
    }
    else
    {
        echo '<pre>' . htmlspecialchars(json_encode($resp, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT)) . '</pre>';
    }
    echo '</div>';
}
echo '</body></html>';
?>