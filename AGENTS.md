# AGENTS.md

本文件为在此仓库中工作的 AI 编码代理提供**项目特定的操作约束**。
它不重复通用介绍——架构总览见 [`README.md`](README.md)，新增传感器的详细教程见 [`docs/二次开发指导-新增传感器.md`](docs/二次开发指导-新增传感器.md)。

**基线**：分支 `develop_fang`，固件版本 V2.2.9。文中行号以 commit `a43890e` 为准。

---

## 0. 先读这一页：五条硬性约束

这五条是踩过坑之后总结的，违反任一条都会造成**不可用或不可逆**的后果。

| # | 约束 | 违反后果 |
|---|---|---|
| 1 | **无法在本机编译验证** —— 不要声称代码"已验证/已编译" | 交付未经编译的代码被当成已验证 |
| 2 | **写文件前必须确认目标文件编码** | GBK 文件的中文注释被永久摧毁 |
| 3 | **改动 `setting_t` 结构必须同步 `SETTING_FIRSTRUN_JUDGE`** | 老设备升级后读到错位配置 |
| 4 | **新增传感器必须改齐 6 处必改项 + `Sersor_Number`** | 搜不到设备 / 数值恒为 0 / 链接失败 |
| 5 | **不要改 `code/Drivers/`、`code/Middlewares/`** | 覆盖 ST 官方 HAL，无法回退 |

---

## 1. 项目一句话说明

STM32L152VE 裸机（**无 RTOS**）固件，便携式多参数水质分析仪主机。
通过 RS485/Modbus RTU 连接并自动识别 30+ 种数字传感器探头，负责测量、显示、校准、报警、数据记录与上位机通信。

**技术栈**：C99/C11 · STM32L1xx HAL · Keil MDK-ARM · ARM Compiler 6 (armclang V6.14) · `-O0` · 无第三方库 · 裸机前后台架构。

---

## 2. ⚠️ 编码：本项目最大的陷阱

**源码编码不统一。** 实测分布（`Core/` + `gui/` + `hardware/` + `logic/` 共 132 个 `.c`/`.h`）：

| 编码 | 文件数 | 分布 |
|---|---|---|
| **UTF-8** | 54 | `hardware/` **全部**；`gui/` 的 `.c` 与 `option.h`/`interfacial.h`/`generate_page.h`/`gui.h`；`Core/Inc/main.h`、`Core/Src/usart.c`/`spi.c`；`logic/` 的 `logic.c`、`dissolved_oxygen.c`、`setting.c`、`log.c`、`algorith.c`、`calculate.c`；**以及全部近期新增文件**（`DO_HaiFa_DY56.c` 等） |
| **GBK** | 33 | `Core/Src/main.c`；`gui/GUI_BASIC.h`、`GUI_StockC.h`、`label.h`、`loadbit.h`、`nano_option.h`（5 个）；**27 个早期 `logic/` 传感器驱动**：`BGA_shenghui.c`、`COD_DC18.c`、`COD_DC18.h`、`COD_shenghui.c`、`Chl_shenghui.c`、`DL312.c`、`DO59.c`、`DO_HaiFa_DY12.c`、`DO_shenghui.c`、`DX01.c`、`DY05.c`、`DZ09.c`、`EC_DE26.c`、`EC_DE40.c`、`EC_shenghui.c`、`MLSS_Tianjian.c`、`MLSS_lanchang.c`、`NH3N_DN02.c`、`NH3N_shenghui.c`、`ORP_DR31.c`、`OiW_Guohong.c`、`OiW_Guohong.h`、`OiW_yushan.c`、`TDS_DT49.c`、`Tur_shenghui.c`、`pH_DpH07.c`、`pH_shenghui.c` |
| 纯 ASCII | 44 | 无中文注释的文件（无风险） |
| 注释已损坏 | 1 | `hardware/data_processing.h` |

### 2.1 为什么这是陷阱（已实测验证）

`read` / `edit` / `write` 工具链**按 UTF-8 解码**。对 GBK 文件：

1. 读取时，每个 GBK 中文字节无法解码 → 变成 `U+FFFD`（``）；
2. 写入时，把内存中这份**已损坏**的字符串以 UTF-8 序列化；
3. 结果：**该文件全部中文注释永久丢失**，且丢失发生在写入瞬间，无法撤销。

**实测数据**（本仓库内做的对照实验）：

```text
操作：对 code/logic/ORP_DR31.c（GBK，3328 字节）做一次仅涉及 ASCII 区域的 edit
结果：文件大小 3328 → 3525 字节
      GBK 汉字 0 个（原有 88 个汉字全部变成 U+FFFD）
      文件不再是合法 GBK，变成合法 UTF-8
      → 即使 oldText 只包含纯 ASCII，整个文件仍会被重新序列化，中文全毁
```

**关键结论：不能通过"只改 ASCII 区域"来规避。任何一次写入都会重编码整个文件。**

### 2.2 正确处置流程

**新建文件**（推荐路径）：直接写 **UTF-8**。这是项目实际演进方向——所有近期新增文件都是 UTF-8。无需询问。

**编辑已有文件**：先判定编码，再决定。

```bash
# 判定单个文件编码（UTF-8 能解码 → UTF-8；解码失败 → GBK）
python -c "
import sys
b = open(sys.argv[1], 'rb').read()
try:
    b.decode('utf-8'); print('UTF-8  (可安全编辑)')
except UnicodeDecodeError:
    print('GBK    (禁止直接编辑，需先转换)')
" code/logic/ORP_DR31.c
```

若为 **GBK**，按以下顺序处理，**不要自作主张静默转换**：

```bash
# 1) 转为 UTF-8（无损、可逆；iconv 是可靠工具）
iconv -f GBK -t UTF-8 code/logic/ORP_DR31.c > /tmp/x.c && mv /tmp/x.c code/logic/ORP_DR31.c

# 2) 让编码转换成为一次独立的、可审查的改动
git add code/logic/ORP_DR31.c && git commit -m "编码统一：ORP_DR31.c GBK -> UTF-8"

# 3) 之后再正常编辑内容
```

> **务必把"编码转换"与"逻辑修改"分成两个 commit。** 混在一起会让 code review 无法分辨语义变更。

**向用户报告**：如果任务要求修改 GBK 文件，应在动手前说明"该文件是 GBK，直接编辑会摧毁中文注释，建议先转 UTF-8"，而不是默默转换。

### 2.3 不要动的两件事

- **不要"修复" `hardware/data_processing.h`**：它的中文注释已被双重编码损坏为 `//¿ÕÏÐÖÐ¶Ï`（原本是 `//空闲中断`）。这只是可读性问题，**不影响编译**。修复它属于无关变更。
- **不要在 GBK 文件的 `//` 注释里新增中文**：GBK 双字节字符的次字节可能是 `0x5C`（`\`），在行尾会被编译器当作**行续接**，吞掉下一行代码。当前仓库 33 个 GBK 文件已检查过，**无此风险**，但新增中文可能引入。

---

## 3. 构建与验证：**本机无法编译**

这是本仓库最重要的工作流事实。

### 3.1 现状

| 项 | 值 |
|---|---|
| 构建系统 | **Keil µVision（MDK-ARM）**，GUI 工具，无 Makefile / CMake |
| 工具链 | ARM Compiler 6（armclang V6.14），路径不在 PATH 中 |
| 工程文件 | `code/MDK-ARM/PortableOxygen.uvprojx` |
| 输出 | `code/MDK-ARM/PortableOxygen/`（`.axf`/`.hex`/`.map`） |

**代理无法在命令行编译本项目。** `armclang`/`armlink`/`UV4.exe` 通常不可用，也没有可脚本化的构建入口。

### 3.2 因此必须遵守

- ❌ **禁止**声称"已编译通过"、"已验证"、"测试通过"。
- ❌ **禁止**把语法正确性视为已验证——HAL 头文件依赖、宏展开、`enum` 数值依赖都可能出错。
- ✅ 代码写完后，**明确告知用户需要在 Keil 中 Rebuild 验证**。
- ✅ 可以做的是**静态自查**：括号配对、`#include` 是否需要新增、函数是否已声明、新 `.c` 是否已加入 `.uvprojx`、符号是否重名。
- ✅ 若仅是分析/文档/编码转换类任务，说明不需要编译。

### 3.3 可供参考的既有构建产物

`code/MDK-ARM/PortableOxygen/PortableOxygen.map` 与 `.build_log.htm` 是**上一次成功构建**的产物，可用于：

- 确认某符号是否真的被编入（`.map` 中查符号）；
- 参考当前体积基线：ROM 361 968 B / RAM 40 640 B；
- 查看既有告警基线（当前仅 1 条：`GUI_StockC.h` 中 `typedef 'TCOLOR'` 重复定义，可忽略）。

> macOS / Linux 上**没有**可用的验证路径。不要尝试用 `gcc`/`clang` "顺便编译一下"——缺少 HAL 头文件与启动文件，必然失败且无意义。

---

## 4. 代码约定

### 4.1 命名

| 对象 | 约定 | 示例 |
|---|---|---|
| 驱动 API | `<型号>_rs485_<动作>` | `DO_HaiFa_DY56_rs485_GetValue` |
| 应答解析 | `<型号>_UpdateTemp2DO`（**全项目统一名，即使是 pH/浊度也这么叫**） | `ORP_DR31_UpdateTemp2DO` |
| 标志/数值访问 | `Set_<型号>_Xxx_Flag` / `Get_<型号>_Set_Xxx` / `Set_<型号>_Xxx_Value` | `Set_DY56_Sal_Flag` |
| 设备状态 | `DO_SetIsInit` / `DO_SetIsGetedValue` | —— |
| 全局配置 | `setting_GetXxx` / `setting_SetXxx` | `setting_GetSalinity` |

沿用既有命名，**不要引入新风格**。`UpdateTemp2DO` 这种历史遗留名字虽然不合理，但改了会造成大面积不一致。

### 4.2 模块变量一律 `static`

驱动文件里所有非 API 的模块级变量**必须加 `static`**。从同类型文件克隆时尤其危险：

```c
/* ❌ 与 DO_HaiFa_DY12.c 的同名符号冲突 → multiple definition */
uint8_t Sal_Flag = 0, Press_Flag = 0;

/* ✅ */
static uint8_t Sal_Flag = 0, Press_Flag = 0;
```

### 4.3 不要使用 `STATIC` 宏

`code/Core/Inc/main.h` 中：

```c
#define DEBUG 1
#if DEBUG
#define STATIC          /* ← DEBUG=1 时展开为空，变量变成全局符号 */
#else
#define STATIC static
#endif
```

新增代码请**直接用 `static` 关键字**，不要用 `STATIC` 宏。

### 4.4 格式

- 缩进：**制表符 Tab**（源文件混用 Tab/空格，跟随所在文件的既有风格）。
- 行尾：CRLF（部分新文件是 LF，跟随所在文件）。
- 大括号：Allman 风格（函数与 `switch` 的 `{` 另起一行）。
- 中文注释：可以，但见第 2 节编码约束。
- **不要对既有文件做仅格式化的改动**（重排、改缩进、增删空行）——会产生巨大 diff 淹没语义变更。

### 4.5 指针与空值防御

驱动 API 的既有约定是**首行做空指针检查**，请沿用：

```c
void DO_HaiFa_DY56_rs485_GetValue(PtrToDOProbe ptd)
{
    if(ptd == NULL) return;
    ...
}
```

### 4.6 双探头指针选择（最容易写错的一行）

`get_COMADo()` **永远**返回 A 通道指针，与"这个应答来自哪个探头"无关。
第二个探头挂在 `comB_DO`。**统一使用**：

```c
get_COMADo()->modbus_id == XX_ModbusID ? get_COMADo() : get_COMBDo()
```

写错会导致 **B 通道探头数据/SN 显示空白，A 通道显示错误数据**。不要自创简写。

---

## 5. 分层职责边界（严格遵守）

违反边界是本项目最主要的腐化来源。

| 层 | 目录 | **只负责** | **禁止** |
|---|---|---|---|
| 驱动 | `logic/<型号>.c` | 组帧 + 解析字节 | 调 `rs485_SendBuf()`、碰 `interfacial_*`、写业务流程 |
| 状态机 | `hardware/data_processing.c` | 唯一的"收到什么之后发什么"决策点 | 组帧细节、数值滤波、响应按键 |
| 设备 | `logic/dissolved_oxygen.c` | 设备链表、类型归类、气压/盐度自动下发 | 组帧 |
| 界面 | `gui/interfacial.c` | 按键 → 写 `setting_*` + 置标志 + 发写使能 | 直接发参数写指令、解析 `rx_buf` |
| 显示 | `gui/generate_page.c` | 只读显示 | 参与任何协议 |
| 配置 | `logic/setting.c/.h` | 持久化 getter/setter | 带协议逻辑 |

**核心模式**：界面层**不直接**发写指令，只置「标志 + 数值」并发**写使能**；
真正的写寄存器由状态机在 `DO_SendType_Start` 的应答分支里完成。原因：Modbus 半双工、单总线，
`rs485_usart` 只有一份 `tx_buf`，界面直接发会被 600 ms 循环读数顶掉。

---

## 6. 新增传感器：必改项速查

完整教程（含可复制模板）见 [`docs/二次开发指导-新增传感器.md`](docs/二次开发指导-新增传感器.md)。此处仅列**不可遗漏的清单**：

| # | 文件 | 改什么 | 漏掉的后果 |
|---|---|---|---|
| 1 | `hardware/rs485.h:20-63` | `#define XX_ModbusID <n>`（**n ≤ 99，且不得与已有 ID 冲突**） | 编译报未定义 |
| 2 | `logic/XX.c` + `logic/XX.h` | 新建驱动（UTF-8） | —— |
| 3 | `hardware/data_processing.c:1-40` | `#include "XX.h"` | 编译报未定义 |
| 4 | `hardware/data_processing.c:3463` | `rs485_DataHandle()` 的 `switch` 中注册 `case XX_ModbusID:` | **能搜到设备但数值恒为 0** |
| 5 | `logic/dissolved_oxygen.c:184` | `DO_AddProbe()` 中归入某个 `case` | 设备名为空、类型异常 |
| 6 | `code/MDK-ARM/PortableOxygen.uvprojx` | `logic` 组加入 `XX.c` | 链接失败 |
| 7 | `gui/interfacial.c:8019` | `#define Sersor_Number 28` → 递增 | **新传感器永远搜不到** |

### 6.1 按功能选配的插入点

| 功能 | 位置（基线行号） |
|---|---|
| 搜索轮询（**共 2 处独立副本**） | `gui/interfacial.c:8461`、`:8705` |
| 读数 case（**共 3 处**） | `gui/interfacial.c:8038`（双探头 A）、`:8172`（双探头 B）、`:8318`（单探头） |
| 盐度下发 | `gui/interfacial.c:3474` `PAGE_3_SALT` |
| 气压下发 | `gui/interfacial.c:3420` `PAGE_3_PRESSURE` |
| 温度校准 | `gui/interfacial.c:4469` `PAGE_5_TEMP` |
| 斜率/满氧校准 | `gui/interfacial.c:4907` `PAGE_5_DO_ONE_First` |
| 零点校准 | `gui/interfacial.c:5098` `PAGE_5_DO_TWO_FIRST` |
| 恢复出厂 | `gui/interfacial.c:3172` `PAGE_1_RESETCAL` |
| 校准菜单显示 | `gui/generate_page.c:1364`、`:1369` |
| 气压/盐度自动下发（仅 DO 类） | `logic/dissolved_oxygen.c:1434`、`:1472` |

> **改完务必自检**：`grep -n "Sersor_Number" gui/interfacial.c` 的数量值，
> 必须**等于** `grep -c "CircularSent_Count ==" gui/interfacial.c`。两者不一致就是搜索漏项。

---

## 7. 高危改动区域

改动前请先向用户确认，并说明影响面。

| 区域 | 为什么危险 |
|---|---|
| `setting_t`（`logic/setting.h`） | 结构变更必须同步提升 `SETTING_FIRSTRUN_JUDGE`（`Core/Inc/main.h`），否则已出货设备读到错位配置 |
| Flash 地址定义（`Core/Inc/main.h`） | 改动会使既有设备的配置与历史数据失效；历史记录每条按 **256 字节对齐**（`index << 8`），单类型 5000 条 |
| `SENSOR_TYPE` 枚举（`hardware/rs485.h:67-87`） | `TYPE_NONE` 是哨兵值，追加类型会影响数值依赖。**优先复用现有类型** |
| `HAL_UART_IDLE_HANDLER` / `*_IDLECallBack` | RS485 空闲中断里先 `HAL_UART_AbortReceive()` 再重启 DMA，时序敏感 |
| `MX_DMA_Init()` 调用顺序（`logic.c`） | **必须早于** `MX_USARTx_UART_Init()`，否则串口只能收到最后一个字节 |
| `DO_UpdatePressSal()`（`dissolved_oxygen.c:1416`） | 与传感器写流程互相影响，改错会造成"参数自动被写回"的循环 |
| 主循环顺序（`logic.c: main_loop()`） | 前后顺序有隐式依赖（如"先刷界面再更新数据"） |

---

## 8. 已知设计缺陷（不要顺手"修复"）

这些是**已知**问题，修复它们属于**无关变更**，会扩大 diff 并可能引入回归。除非用户明确要求，否则保持原样并只在需要时提示。

| 位置 | 问题 |
|---|---|
| `logic/dissolved_oxygen.c:895` `DO_SetSN()` | `ptd->SN[1] == 'L' && ptd->SN[1] == '0'` 恒为假 → `manufacturer` 对多数探头是**未初始化垃圾值**。因此校准菜单的厂商分支必须**额外**判断 `modbus_id` |
| `hardware/rs485.c:18` | `STATIC uint8_t is_change_senesor` 因 `DEBUG 1` 实际是全局符号 |
| `hardware/data_processing.h` | 中文注释已被双重编码损坏（`//¿ÕÏÐÖÐ¶Ï`），仅影响可读性 |
| `logic/setting.c` | `setting_GetIsSelect_*` 系列函数**无任何调用者**（死配置） |
| `logic/DO_HaiFa_DY56.c` | `DO_HaiFa_DY56_rs485_Set_Break`、`Start_Temp_Cal`、`Temp_Cal_Pending` 等无调用者（死代码，DY56 协议调整后遗留） |
| `logic/MLSS_lanchang.c` | 已加入 Keil 工程，但**未接入** `DO_AddProbe()` 与 `data_processing.c` 分发表，实际不可用 |
| `logic/dissolved_oxygen.c:1514` `DO_ValueCheckFirst()` | 直接 `return 1`，不做事 |
| `hardware/data_processing.h` | 与 `do_haifa_dy12.h` 等头文件存在重复声明 |

---

## 9. 反复踩过的坑（协议层）

新增/修改传感器驱动时逐条对照。

1. **写指令在串口日志中出现的两次是传感器的"回显帧"，不是超时重发。**
   判据：回显内容与请求帧完全一致；真重发会在 `RS485_CIRCULAR_TIM`（600 ms）之后出现。
   不要据此去改 `RESEND_MAX` 或重发逻辑。
2. **重连点必须清零所有业务标志。** 重连点 = `case DO_SendType_GetModbusId` 的应答分支。
   写流程可能被"重发 4 次判为断连"打断，残留标志会在下次任意写操作时误触发
   （现象：恢复出厂做一半拔线，重插后改盐度却触发了恢复出厂）。
3. **恢复出厂必须同步本地 `setting` 缓存。** 只写传感器寄存器会导致主循环
   `DO_UpdatePressSal()` 检测到「本地旧值 ≠ 探头新值」而把旧参数自动写回
   （现象：出厂流程发两遍、参数没变）。
4. **写成功后必须回填本地缓存的校准值。** DY56 第一次温度校准正常、第二次算错的根因，
   就是 `SetTemp` 应答里没回填 `NH4_Vol`（温度偏移量缓存）。
5. **`SetCrc(buf, len)` 的 `len` 是"含 2 字节 CRC 的整帧长度"**，即 8；写 6 会覆盖数据字节。
6. **`rs485_DataHandle()` 丢弃 `rx_size < 5` 的帧**（`data_processing.c:3469`）。
7. **`close_circle()` 只关循环发送，不清 `need_sent`。**
8. **每个 case 内必须二次校验应答**（`rx_buf[1]` 功能码 + `rx_buf[2]` 字节数/寄存器高字节），
   因为 `rs485_GetSentType()` 只表示"我在等什么"，不能保证收到的就是那个应答。
9. **`snprintf` 的 size 参数不得超过数组容量**：`DOmgl_arr`/`DOpercent_arr` 是 7，`temperature_arr` 是 6，
   `name` 是 8，`HWV`/`SWV` 是 5（4 字符 + `'\0'`），`SN` 实用 12 字符（`SN[12]='\0'`）。
10. **`p->name` 只有 8 字节且传感器信息页只显示两位地址 → Modbus ID 必须 ≤ 99。**

---

## 10. 文档与记录约定

- **架构/机制变更** → 更新 [`README.md`](README.md) 对应章节。
- **新增传感器** → 更新 [`docs/二次开发指导-新增传感器.md`](docs/二次开发指导-新增传感器.md) 的速查表（行号、清单）。
- **协议调研与设计决策** → **必须**追加到 `code/.workbuddy/memory/<YYYY-MM-DD>.md`。
  这是团队约定：`README.md` 里的踩坑清单正是靠这些记录才没有在多次返工中丢失设计意图。
  记录内容应包含：需求、最终语义、关键代码位置（文件:行号）、方案取舍理由、待确认点。

---

## 11. 版本控制

| 项 | 值 |
|---|---|
| 主分支 | `master` |
| 当前开发分支 | `develop_fang` |
| 远端 | `origin` → `https://github.com/wcf8822/wcf1314.git` |
| 提交信息风格 | `<版本号/日期> <改动摘要>` |

示例（取自真实历史）：

```text
d7620be 2026-08-27 v2.2.7
e70a579 修改81地址的通信协议为海发自己的协议 2026-09-20
a43890e 迁移代码至code目录，完善.gitignore规则
```

### 11.1 `.gitignore` 有白名单，固件产物需要入库

`code/.gitignore` 忽略所有 Keil 中间产物，但**显式保留**：

```gitignore
!*.uvprojx   !*.h   !*.c   !*.ioc   !*.axf   !*.bin   !*.hex
```

因此构建产物（`.axf`/`.bin`/`.hex`）是**有意入库**的团队约定，不要建议删除或加入忽略列表。
注意：`.uvoptx`、`.uvguix.*`、`JLinkLog.txt`、`DebugConfig/` 均被忽略。

### 11.2 不要提交的临时文件

- 编码转换的中间文件（用 `/tmp`）。
- 诊断脚本、临时 dump（本次会话中曾产生 `_detect.py`/`_enc.txt`/`_enctest/`，均已清理）。
- `code/PortableOxygen.map - 快捷方式.lnk` 已被忽略，勿动。

---

## 12. 快速定位命令集

```bash
cd code

# 查某型号在何处出现（把 DY56 换成目标型号）
grep -rn "DO_HF_DY56_ModbusID" --include=*.c --include=*.h .

# 查所有已占用 Modbus ID（新增前必做，避免冲突）
grep -n "_ModbusID\|_Modbus " hardware/rs485.h

# ★ 搜索漏项自检：两个数必须相等
grep -n "Sersor_Number" gui/interfacial.c
grep -c "CircularSent_Count ==" gui/interfacial.c

# 查新文件是否已加入 Keil 工程
grep -n "XX.c" MDK-ARM/PortableOxygen.uvprojx

# 查编码分布（避免误编辑 GBK 文件）
python -c "
import glob
for d in ['Core/Inc','Core/Src','gui','hardware','logic']:
    for p in sorted(glob.glob(d+'/*.c')+glob.glob(d+'/*.h')):
        b=open(p,'rb').read()
        try:
            b.decode('ascii'); continue
        except UnicodeDecodeError: pass
        try: b.decode('utf-8')
        except UnicodeDecodeError: print('GBK  ->', p)
"

# 查是否已有 U+FFFD 损坏（若有，说明曾被工具破坏）
grep -rlP '\xef\xbf\xbd' --include=*.c --include=*.h . 2>/dev/null || echo "(无损坏)"

# 查符号是否真的进了上次构建
grep -n "XX_DataHandle" MDK-ARM/PortableOxygen/PortableOxygen.map
```

---

## 13. 交付前自检

- [ ] 未声称"已编译/已验证"；已提示用户需在 Keil 中 Rebuild
- [ ] 新建文件为 **UTF-8**；未编辑任何 **GBK** 文件（或已先转换并独立提交）
- [ ] 新 `.c` 已加入 `MDK-ARM/PortableOxygen.uvprojx`
- [ ] 新增/修改的模块变量带 `static`
- [ ] 未使用 `STATIC` 宏
- [ ] 驱动 API 有空指针检查
- [ ] 双探头指针用统一三元表达式
- [ ] 新增传感器：6 处必改项 + `Sersor_Number` 全部就位，且自检两个计数相等
- [ ] 涉及 `setting_t` 时已同步 `SETTING_FIRSTRUN_JUDGE`
- [ ] 未做无关的格式化/重排改动
- [ ] 未"顺手修复"第 8 节的已知缺陷
- [ ] 协议或架构决策已记入 `code/.workbuddy/memory/`
- [ ] 相关文档（`README.md` / `docs/`）已同步
- [ ] 无临时/调试文件残留

---

## 14. 与用户沟通的注意事项

- **中文回复**：仓库文档与提交信息均为中文，用户以中文提问。
- **不要过度承诺**：本项目无法编译验证，请如实说明"代码已改完，但未编译验证"。
- **编码问题必须主动说明**：涉及 GBK 文件时，先报告再动手，不要静默转换。
- **改动面要交代清楚**：本项目的改动往往横跨 6–8 个文件，请列出清单，便于用户按 §12 的检索命令复核。
- **行号会漂移**：文中行号基于 commit `a43890e`。若代码已变动，用 §12 的命令重新定位，不要盲信行号。

---

## 参考

| 文档 | 内容 |
|---|---|
| [`README.md`](README.md) | 工程总览、硬件平台、外设/引脚分配、Flash 布局、Modbus ID 全表 |
| [`docs/二次开发指导-新增传感器.md`](docs/二次开发指导-新增传感器.md) | 新增传感器完整教程：14 项协议确认清单、7 步流程、可复制模板、3 类范例、15 条坑 |
| `code/.workbuddy/memory/2026-09-20.md` | DY56 协议迁移的完整决策记录（最有价值的历史上下文） |
