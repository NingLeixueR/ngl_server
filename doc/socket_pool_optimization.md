# Socket Buffer Pool 设计文档

## 概述

`socket_pool` 是一个面向网络 I/O 场景的线程本地内存池，位于 `tools/serialize/socket_pool.h`。

核心设计：每个线程拥有独立的 bucket 缓存，同线程分配/释放零竞争；跨线程释放通过 pending queue 异步归还，仅在归还时加锁。无全局池、无 shared_mutex、无定时回收。

## 架构

```
Thread A (slot 0)                Thread B (slot 1)
┌─────────────────┐              ┌─────────────────┐
│  thread_cache    │              │  thread_cache    │
│  ┌─────────────┐ │              │  ┌─────────────┐ │
│  │ local_pool   │ │   cross-    │  │ local_pool   │ │
│  │ [0] 64B     │ │   thread    │  │ [0] 64B     │ │
│  │ [1] 128B    │ │   free()    │  │ [1] 128B    │ │
│  │ ...         │ │ ──────────► │  │ ...         │ │
│  │ [11] 128KB  │ │  pending    │  │ [11] 128KB  │ │
│  └─────────────┘ │  queue      │  └─────────────┘ │
│  ┌─────────────┐ │  (mutex)    │  ┌─────────────┐ │
│  │ m_pending   │ │              │  │ m_pending   │ │
│  └─────────────┘ │              │  └─────────────┘ │
└─────────────────┘              └─────────────────┘
         │                                │
         └──── s_slots[128] 全局槽位表 ────┘
               (atomic 指针, O(1) 查找)
```

## Bucket 布局

12 个 bucket，64B 起步，2 倍递增，编译期生成（`static constexpr`）：

| Bucket | 大小 | 预分配上限 | Trim 阈值 (2x) |
|--------|------|-----------|----------------|
| 0 | 64B | 100 | 200 |
| 1 | 128B | 80 | 160 |
| 2 | 256B | 60 | 120 |
| 3 | 512B | 50 | 100 |
| 4 | 1KB | 40 | 80 |
| 5 | 2KB | 30 | 60 |
| 6 | 4KB | 25 | 50 |
| 7 | 8KB | 20 | 40 |
| 8 | 16KB | 15 | 30 |
| 9 | 32KB | 10 | 20 |
| 10 | 64KB | 5 | 10 |
| 11 | 128KB | 3 | 6 |

超过 128KB 的请求直接 `new[]`，释放时直接 `delete[]`，不进入池。

## 关键机制

### block_head

每个 buffer 前置 8 字节头部：

```
| m_magic (4B) | m_index (2B) | m_slot (2B) | user data ... |
```

- `m_magic = 0x534F434B`：校验合法性，`free()` 时 magic 不匹配会触发 `assert`
- `m_index`：bucket 索引，-1 表示超大分配
- `m_slot`：分配线程的槽位号，`free()` 据此 O(1) 定位 owner

### 线程槽位注册

每个线程首次使用时通过 CAS 注册到 `s_slots[128]`：

```cpp
int16_t expected = s_slot_next.load(std::memory_order_relaxed);
while (expected < enum_max_threads)
{
    if (s_slot_next.compare_exchange_weak(expected, expected + 1, ...))
    {
        m_slot = expected;
        s_slots[m_slot].store(&m_cache, std::memory_order_release);
        return;
    }
}
// 超过 128 线程：m_slot 保持 -1，退化为无池化模式
```

CAS 保证 `s_slot_next` 不会超过 128，避免 `int16_t` 溢出导致数组越界。

### O(1) Bucket 查找

利用 bucket 尺寸为 2 的幂次的特性，用 `std::bit_width` 替代 `std::lower_bound`：

```cpp
constexpr int base_shift = std::countr_zero(static_cast<uint32_t>(TINITBYTES)); // 6
int shift = std::bit_width(static_cast<uint32_t>(abytes - 1));
int16_t bucket = shift - base_shift;
```

编译期约束 `TINITBYTES` 必须为 2 的幂（`static_assert`）。

### 分配路径 (malloc_private)

```
abytes <= 0 → nullptr
    ↓
O(1) 定位 bucket（超大 → 直接 new）
    ↓
drain_pending（atomic flag 快速跳过）
    ↓
local_pool 有缓存 → 命中返回（cache_hit++）
    ↓
new[] 分配新 buffer（cache_miss++）
```

### 释放路径 (free_private)

```
nullptr → 跳过
    ↓
read_head 校验 magic（失败 → assert）
    ↓
超大分配 (m_index < 0) → delete[]
    ↓
同线程 (m_slot >= 0 && m_slot == tl.m_slot) → push 到 local_pool
    ↓
跨线程 → lock owner->m_pending_mutex → push + 设置 m_has_pending
    ↓
owner 已死或 slot 无效 → delete[]
```

### drain_pending 优化

每次 `malloc` 前检查 pending queue。通过 `atomic<bool> m_has_pending` 标记避免无谓加锁：

```cpp
if (!acache.m_has_pending.load(std::memory_order_relaxed))
    return;  // 无 pending，跳过加锁

std::lock_guard lk(acache.m_pending_mutex);
ltmp.swap(acache.m_pending);
acache.m_has_pending.store(false, std::memory_order_relaxed);
```

drain 后对超过 2x 配置上限的 bucket 执行 trim（`delete[]` 多余 buffer）。

## 接口

```cpp
// 静态单例接口
char* socket_pool::malloc(int abytes);
void  socket_pool::free(char* abuff);
void  socket_pool::print_stats();
const pool_stats& socket_pool::get_stats();
```

通过 `bpool` 桥接兼容 `pack` 体系：

```cpp
// pack.cpp
char* bpool::malloc(int32_t alen) { return socket_pool::malloc(alen); }
void  bpool::free(char* ap, int32_t) { socket_pool::free(ap); }
```

## 监控

`pool_stats` 提供 6 个原子计数器，所有 load 使用 `relaxed` 序且只读取一次，避免 TOCTOU：

```cpp
socket_pool::print_stats();
// SocketPool Stats: alloc=50000 free=50000 hit_rate=96.30% large=12 fail=0
```

运维后台通过 GM 命令 `socket_pool_stats` 远程查询。

## 内存开销

- 每个 buffer 额外 8 字节头部（`block_head`）
- 每线程缓存：12 个 `vector<char*>`（按需增长）+ 1 个 pending vector + 1 个 mutex + 1 个 atomic flag
- 全局：`s_slots[128]` 原子指针数组 = 1KB
- 统计：6 个 `atomic<uint64_t>` = 48 字节
- `m_bytes` 为 `static constexpr`，不占实例空间

## 测试覆盖

| 测试用例 | 场景 |
|---------|------|
| BasicAllocation | 基本分配/释放 |
| MultipleAllocations | 多种尺寸分配 |
| LargeAllocation | 超大分配 (200KB) |
| CrossThreadFree | 跨线程释放 + pending drain |
| CrossThreadBatch | 批量跨线程释放 (32 buffers) |
| CrossThreadContentPreserved | 跨线程释放前内容完整性校验 |
| SlotOverflow | 140 线程并发，验证超 128 槽位不崩溃 |
| BucketTrim | 250 buffers 跨线程释放后 trim 验证 |
| ConcurrentStress | 8 线程 × 1000 次混合尺寸，验证 alloc == free |
| NetbuffPoolZeroLengthReturnsNull | malloc(0) 返回 nullptr |

## 设计约束

1. **线程生命周期 = 进程生命周期**：`thread_cache` 不析构，`s_slots` 中的指针始终有效。这是框架层面的保证，非通用场景不适用
2. **线程数上限 128**：超出后退化为直接 `new/delete`，无池化收益
3. **TINITBYTES 必须为 2 的幂**：编译期 `static_assert` 强制约束，支撑 O(1) bucket 查找
