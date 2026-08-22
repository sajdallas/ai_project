# STM32F407 内存优化：把 FreeRTOS 堆搬到 CCM RAM

## 背景

STM32F407 有 192KB 系统 SRAM，其中 112KB 是普通 SRAM，16KB 留给启动代码初始化，还有 64KB 是 **CCM（Core Coupled Memory）**。

CCM 的特点：
- 地址范围：`0x10000000` ~ `0x1000FFFF`（64KB）
- **只和 CPU 直连，DMA 无法访问**
- 跟 CPU 同频，零等待存取，比普通 SRAM 更快
- 适合放：FreeRTOS 堆、内核数据、临界区代码

我们的项目 FreeRTOS 堆（ucHeap）40KB，放在普通 SRAM 里占了 35.7%，导致内部 SRAM 占用高达 93%。把它搬到 CCM 就能释放这 40KB。

---

## 方案一（失败）：`__attribute__((at(0x...)))`

直接在 `heap_4.c` 里写：

```c
static uint8_t ucHeap[40960] __attribute__((at(0x10000000)));
```

**为什么失败：屏幕不亮。**

原因：CCM RAM 不在 MDK 默认的 scatter 文件里，启动代码 **不会对 CCM 做清零操作**。ucHeap 里全是随机垃圾值，FreeRTOS 初始化堆时把所有内存块当成已分配，后续 `pvPortMalloc` 全部失败，系统崩溃。

---

## 方案二（成功）：scatter 文件 + 关闭 mem3 池

### 步骤 1：scatter 文件加 CCM 区域

在 `ai_project.sct` 最后加：

```
RW_CCM 0x10000000 0x00010000  {
    heap_4.o (+RW +ZI)
}
```

这样 MDK 启动代码（`__main` / `__scatterload`）会自动把 `heap_4.o` 的 RW 和 ZI 数据加载到 CCM，并**执行清零**。

### 步骤 2：关掉 malloc 的 mem3 池

`malloc.c` 里有一个 mem3 池，也用了 `__attribute__((at(0X10000000)))` 占着 CCM 起始地址。如果不关掉，链接器发现 CCM 地址冲突，会回退把 `heap_4.o` 放进普通 SRAM——白干。

所以：
- `malloc.h`：`MEM3_MAX_SIZE = 0`
- `malloc.c`：注释掉 `mem3base` 和 `mem3mapbase`，mallco_dev 里填 NULL

---

## 核心原理

### MDK 链接器（armlink）的工作流程

1. **模块加载**：编译器把每个 `.c` 编译成 `.o`，每个 `.o` 包含多个 section（`.text`、`.data`、`.bss` 等）
2. **scatter 文件**：告诉链接器各个 section 放哪里
3. **非 `.ANY` 区域优先**：先处理明确指定了匹配规则的 region（如 `heap_4.o (+RW +ZI)`）
4. **`.ANY` 区域兜底**：用 `.ANY` 的 region 最后处理，把剩余 section 放进去

### scatter 文件语法

```
LR_IROM1 0x08000000 0x00080000  {     // Load Region：整个固件
  ER_IROM1 0x08000000 0x00080000  {   // Execution Region：代码+常量
   *.o (RESET, +First)                // 中断向量表放最前面
   *(InRoot$$Sections)                // 库函数需要的根段
   .ANY (+RO)                         // 所有只读数据
   .ANY (+XO)                         // 所有只执行代码
  }
  RW_IRAM1 0x20000000 0x0001C000  {   // 内部 SRAM 第一块 (112KB)
   .ANY (+RW +ZI)                     // 剩余的可读写+零初始化数据
  }
  RW_IRAM2 0x2001C000 0x00004000  {   // 内部 SRAM 第二块 (16KB)
   .ANY (+RW +ZI)
  }
  RW_CCM 0x10000000 0x00010000  {     // CCM (64KB) —— 新增
   heap_4.o (+RW +ZI)                 // 只放 FreeRTOS 堆
  }
}
```

### 关键：`__attribute__((at(0x...)))` vs scatter 文件

| 方式 | 优点 | 缺点 |
|------|------|------|
| `__attribute__((at(0x...)))` | 简单，一行代码 | 绕过 scatter 文件，启动代码不初始化，手动管理容易出错 |
| scatter 文件 | 启动代码自动清零，链接器统一管理 | 语法稍复杂，需注意地址冲突 |

**教训**：能用 scatter 文件就别用 `at` 属性，后者绕过了 ARM 启动代码的初始化流程，数据不会自动清零。

### 为什么启动代码不对 `at` 区域清零？

MDK 的启动流程：
1. `__main` → `__scatterload` 读取 scatter 文件描述的每个 region
2. 对每个 region 的 RW 段：从 Flash 复制到 RAM
3. 对每个 region 的 ZI 段：填零

`__attribute__((at(0x...)))` 是**编译器级别的绝对地址**，不经过 scatter 文件，启动代码不知道它的存在，自然不会清零。

---

## 内存占用对比

| 区域 | 之前 | 之后 |
|------|------|------|
| 内部 SRAM (RW_IRAM1) | 91.9% | ~56% |
| CCM (RW_CCM) | 空闲 | ucHeap 40KB |
| 内部 SRAM (RW_IRAM2) | 43.6% | 不变 |

---

## 后续优化方向

1. **LVGL 显示缓冲搬到外部 SRAM**：`buf1[240*20]` 和 `buf2[240*20]` 共 19.2KB，移到 `0x68000000` 外部 SRAM
2. **ESP8266 接收缓冲搬到外部 SRAM**：`esp.rx_buf[4096]` + `esp_raw[4096]` = 8KB
3. **freertos.c 里的 json[8192] 搬到外部 SRAM**：8KB

**注意**：外部 SRAM 通过 FSMC 访问，虽然 DMA 也能用，但速度比内部 SRAM 慢。只适合放**不频繁访问**的大缓冲。

---

## 相关文件

- [ai_project.sct](file:///d:/STMPRO/ai_project/MDK-ARM/ai_project/ai_project.sct) — scatter 链接脚本
- [heap_4.c](file:///d:/STMPRO/ai_project/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c) — FreeRTOS heap_4 实现
- [malloc.c](file:///d:/STMPRO/ai_project/MALLOC/malloc.c) — 内存管理
- [malloc.h](file:///d:/STMPRO/ai_project/MALLOC/malloc.h) — 内存池配置