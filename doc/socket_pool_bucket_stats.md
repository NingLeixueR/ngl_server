# Socket Pool Bucket Statistics Enhancement

## 概述

增强了socket buffer pool的统计系统，现在可以追踪每个桶的详细分配情况，并改进了3x释放逻辑。

## 主要改进

### 1. 新增每桶统计指标

为每个桶（bucket）增加了三个关键指标：

```cpp
struct bucket_stats
{
    std::atomic<uint64_t> m_current_allocated{0};  // 当前在外部持有的buffer数量
    std::atomic<uint64_t> m_peak_allocated{0};     // 历史峰值分配数量
    std::atomic<uint64_t> m_current_cached{0};     // 当前在池中缓存的数量
};
```

#### 指标说明

- **current_allocated**: 当前分配出去但尚未释放的buffer数量（外部持有）
  - 在 `malloc_private()` 成功分配时 +1
  - 在 `free_private()` 释放时 -1
  - 反映真实的内存使用情况

- **peak_allocated**: `current_allocated` 的历史最高值
  - 使用 CAS 循环更新，确保线程安全
  - 用于评估桶容量配置是否合理

- **current_cached**: 当前在本地池中缓存的buffer数量
  - 在 `drain_pending()` 时更新
  - 反映池的回收效率

### 2. 改进的3x释放逻辑

**旧逻辑问题**：
```cpp
// 旧代码：只比较 cached 和配置的 m_counts
size_t trigger = static_cast<size_t>(m_counts[i]) * 3;
if (bucket.size() > trigger) { /* trim */ }
```
- 只统计了池中缓存的数量
- 没有考虑外部持有的数量
- 无法反映真实的内存压力

**新逻辑**：
```cpp
// 新代码：比较 cached 和 current_allocated
uint64_t allocated = m_bucket_stats[i].m_current_allocated.load(std::memory_order_relaxed);
size_t trigger = static_cast<size_t>(allocated) * 3;
if (bucket.size() > trigger && allocated > 0)
{
    size_t target = static_cast<size_t>(allocated) * 2;
    // trim to 2x
}
```

**改进点**：
- 基于**当前实际分配数**（`current_allocated`）而不是静态配置
- 当缓存数量超过当前分配数的3倍时才触发释放
- 释放到2倍，保持合理的缓存余量
- 避免在低负载时过度释放，在高负载时及时释放

### 3. PHP管理界面增强

新的统计页面显示：

#### 总体统计
- 总分配次数
- 总释放次数
- 缓存命中率
- 超大分配次数
- 分配失败次数

#### 桶统计表格
每个桶显示：
- **桶大小**：64, 128, 256, ... 字节
- **配置容量**：100, 80, 60, ... 
- **当前分配数**：外部持有的buffer数量
- **峰值分配数**：历史最高分配数
- **当前缓存数**：池中缓存的数量
- **利用率**：`peak_allocated / capacity * 100%`
- **状态**：
  - 正常：`allocated <= capacity`
  - 高负载：`capacity < allocated <= 2 * capacity`（黄色警告）
  - 超载：`allocated > 2 * capacity`（红色危险）

#### 异常提示
- 当 `cached / allocated > 3` 时，标记为"过多"（黄色），表示可能触发trim

## 使用场景

### 1. 容量规划
通过 `peak_allocated` 和 `capacity` 对比，判断桶容量配置是否合理：
```
如果 peak_allocated 远小于 capacity：容量过大，可以减小
如果 peak_allocated 接近或超过 capacity：容量不足，需要增加
```

### 2. 内存泄漏检测
监控 `current_allocated`：
```
如果持续增长不回落：可能存在内存泄漏（buffer未释放）
如果 alloc_count - free_count 与 current_allocated 不匹配：统计异常
```

### 3. 缓存效率评估
对比 `current_cached` 和 `current_allocated`：
```
cached / allocated 比例：
  < 1: 缓存不足，可能频繁分配
  1-2: 正常范围
  2-3: 缓存充足
  > 3: 缓存过多，会触发trim
```

## 实现细节

### 线程安全
- 所有统计字段使用 `std::atomic`
- `current_allocated` 使用 `fetch_add` / `fetch_sub`
- `peak_allocated` 使用 CAS 循环更新
- `current_cached` 在持有 `m_pending_mutex` 时更新

### 性能影响
- 每次 `malloc` 增加一次原子递增和一次 CAS 循环（仅在更新峰值时）
- 每次 `free` 增加一次原子递减
- 开销极小，对性能影响可忽略

### 内存开销
- 每个桶增加 24 字节（3个 `uint64_t`）
- 12个桶共增加 288 字节
- 相比整个池的内存占用，可忽略不计

## API变更

### C++ API
```cpp
// 新增接口
static const std::array<bucket_stats, 12>& socket_pool::get_bucket_stats();
static const std::array<int32_t, 12>& socket_pool::get_bucket_bytes();
static const std::array<int32_t, 12>& socket_pool::get_bucket_counts();
```

### GM协议
```cpp
struct bucket_info
{
    int32_t bucket_size;           // 桶大小（字节）
    int32_t bucket_capacity;       // 配置容量
    uint64_t current_allocated;    // 当前分配数
    uint64_t peak_allocated;       // 峰值分配数
    uint64_t current_cached;       // 当前缓存数
};

struct socket_pool_stats
{
    // ... 原有字段 ...
    std::vector<bucket_info> buckets;  // 新增：桶统计数组
};
```

## 文件变更清单

1. **public/cpp/tools/serialize/socket_pool.h**
   - 新增 `bucket_stats` 结构体
   - 新增 `m_bucket_stats` 成员数组
   - 新增 `track_alloc()` / `track_free()` 方法
   - 修改 `drain_pending()` 的trim逻辑
   - 新增 `get_bucket_stats()` / `get_bucket_bytes()` / `get_bucket_counts()` 访问器

2. **public/cpp/actor/actor_logic/actor_gmclient/actor_gmclient.h**
   - 新增 `bucket_info` 结构体
   - 修改 `socket_pool_stats` 结构体，增加 `buckets` 字段

3. **public/cpp/actor/actor_logic/actor_gmclient/actor_gmclient.cpp**
   - 修改 `get_socket_pool_stats()` 实现，填充桶统计数据

4. **admin/sys/socket_pool_stats.php**
   - 完全重写，增加桶统计表格显示
   - 增加状态颜色标记（正常/高负载/超载）
   - 增加缓存过多警告

## 测试建议

1. **功能测试**
   - 分配不同大小的buffer，验证统计正确性
   - 跨线程释放，验证统计一致性
   - 触发trim，验证释放逻辑

2. **压力测试**
   - 高并发分配/释放，验证原子操作正确性
   - 长时间运行，验证峰值统计准确性

3. **界面测试**
   - 访问PHP管理页面，验证数据显示正确
   - 模拟高负载/超载场景，验证状态标记

## 未来改进方向

1. **自适应容量调整**
   - 根据 `peak_allocated` 动态调整 `m_counts`
   - 在运行时优化内存使用

2. **历史趋势记录**
   - 记录峰值出现的时间戳
   - 绘制分配趋势图表

3. **告警机制**
   - 当某个桶超载时发送告警
   - 当缓存命中率过低时发送告警
