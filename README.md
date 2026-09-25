# PortableOxygen B580

基于 **STM32L152VE** 的便携式多参数水质分析仪（主机/手持终端）固件工程。

设备通过 RS485（Modbus RTU）连接并自动识别多种数字传感器探头，完成测量、显示、校准、报警、数据记录与上位机通信。

- 当前软件版本：**V2.2.9**（`code/gui/interfacial.c` → `software_version[]`）
- 支持机型：**B580** / **D702**（出厂模式可选，存于 W25QXX `0x000800`）
- 编译环境：Keil MDK-ARM（ARM Compiler 6 / armclang）
- **源码编码不统一**：UTF-8（54 个）、GBK（33 个）、纯 ASCII（44 个），另有 1 个文件注释已损坏。
  详见 [AGENTS.md](AGENTS.md) 第 2 节或本文 §9 注意事项 1。

---

## 1. 仓库结构

```text
PortableOxygen B580/
├── code/                       # 固件工程（Keil MDK-ARM）
│   ├── Core/                   # CubeMX 生成：HAL 初始化、时钟、中断向量
│   │   ├── Inc/                # main.h（引脚/Flash地址定义）、各外设句柄声明
│   │   └── Src/                # main.c、gpio.c、adc.c、dma.c、spi.c、tim.c、usart.c、iwdg.c、it.c
│   ├── Drivers/                # STM32L1xx HAL + CMSIS（第三方，勿改）
│   ├── gui/                    # 显示与界面层（9 个 .c / 10 个 .h，约 2.2 万行）
│   ├── hardware/               # 硬件驱动与通信协议解析层（14 个 .c / 14 个 .h）
│   ├── logic/                  # 业务逻辑层（32 个 .c / 32 个 .h，约 1.9 万行）
│   ├── MDK-ARM/                # Keil 工程文件、启动文件、编译产物
│   │   ├── PortableOxygen.uvprojx/.uvoptx      # Keil 工程（目标名 PortableOxygen）
│   │   └── PortableOxygen/                     # 编译输出（.axf/.hex/.map/...）
│   ├── PortableOxygen.ioc      # STM32CubeMX 工程配置
│   ├── a.ini                   # Keil 调试脚本（打印 GPS_RX_BUF 到 MyValues.log）
│   └── .workbuddy/memory/      # 开发过程记录（历史改动决策备忘）
├── docs/                       # 文档目录（当前为空）
└── README.md                   # 本文件
```

应用层代码规模约 **5.3 万行**（Core + gui + hardware + logic，不含 HAL/CMSIS）。

---

## 2. 硬件平台

### 2.1 主控与时钟

| 项目 | 配置 |
|---|---|
| MCU | STM32L152VE（Cortex-M3，LQFP100，512 KB Flash / 80 KB SRAM） |
| CubeMX 记录型号 | STM32L152V8TxA（`Mcu.Name=STM32L152V(8-B)TxA`） |
| 编译宏 | `USE_HAL_DRIVER`, `STM32L152xE` |
| 时钟源 | HSE 8 MHz → PLL（PLLMUL=8 / PLLDIV=2）→ **SYSCLK 32 MHz**，Flash Latency 1 |
| 辅助时钟 | HSI 16 MHz、LSI（IWDG 用） |
| 系统节拍 | TIM2：PSC=31、ARR=999 → **1 ms** 定时中断 |
| 独立看门狗 | IWDG：Prescaler=32、Reload=4095（约 3.5 s 超时），在 TIM2 中断中喂狗 |

### 2.2 外设资源分配

| 外设 | 用途 | 说明 |
|---|---|---|
| USART3 | **RS485 传感器总线** | 9600 8N1，PC10=TX / PC11=RX / PC12=DE，DMA1_Ch3 接收 + 空闲中断 |
| USART2 | GPS 模块 | 9600，PD5=TX / PD6=RX，DMA1_Ch6，PA11 为 GPS 供电使能 |
| USART1 | CH340E（USB 转串口，接上位机） | 115200，PA9=TX / PA10=RX，DMA1_Ch5 |
| SPI1 | W25QXX Flash + BMP280/SPA06-003（共用） | Flash CS=PD7（WP=PE0），气压计 CS=PB8 |
| 软件 I2C | HYM8563 RTC | PB6=SCL / PB7=SDA（`soft_iic.c` 位操作模拟） |
| ADC | 电池电压检测 | ADC1_CHANNEL_23（BAT_AD = PE8），12 位，单次转换 |
| LCD | UC1698U 单色 160×160 | 8 位并口（PD8~PD15 数据），见引脚表 |
| TIM2 | 系统 1 ms 节拍 | 所有定时标志的时基 |
| IWDG | 看门狗 | 由 TIM2 中断内 `feed_dog()` 喂狗 |

> 气压传感器采用运行时判型：`bmp280_Init()` 先探 SPA06-003（ID 寄存器 0x0D），
> 失败再判 BMP280（0xD0 == 0x58），结果由 `bmp280_GetType()` 返回 `BARO_SPA06/BARO_BMP280/BARO_NONE`。
> 两者 pin-to-pin 兼容，共用同一根片选线。

### 2.3 引脚分配（摘自 `Core/Inc/main.h`）

| 功能 | 引脚 | 功能 | 引脚 |
|---|---|---|---|
| LED | PA2 | 蜂鸣器 | PA6 |
| 关机控制 BAT_OFF | PE7 | 电池 AD | PE8 (ADC_IN23) |
| USB 插入检测 | PC4 | LCD 背光 | PA15 |
| GPS 供电使能 | PA11 | LCD CS0 | PA8 |
| 按键 BLU | PE9 | 按键 UP | PE10 |
| 按键 DOWN | PE11 | 按键 OK | PE12 |
| 按键 CAL | PE13 | 按键 ESC | PE14 |
| 按键 MENU | PE15 | 按键 OFF | PA7 |
| 硬件版本 VER0 | PB12 | 硬件版本 VER1 | PB13 |
| LCD D0~D7 | PD15~PD8 | LCD RST | PC6 |
| LCD WR | PC7 | LCD RD | PC8 |
| LCD A0 | PC9 | RS485 DE | PC12 |
| RS485 TX/RX | PC10 / PC11 | CH340E TX/RX | PA9 / PA10 |
| GPS TX/RX | PD5 / PD6 | Flash CS | PD7 |
| Flash WP | PE0 | RTC SCL/SDA | PB6 / PB7 |
| 气压计 CS | PB8 | USB 检测 | PC4 |

硬件版本由 PB12/PB13 两位电阻编码读取，映射为 `VER_A ~ VER_D`（`update_HWVersion()`），在「仪表信息」页显示。

---

## 3. 软件架构

### 3.1 分层

```text
        ┌──────────────────────────────────────────────┐
        │  gui/     界面与交互层                        │
        │  interfacial.c  页面调度 / 按键 / 弹窗 / 报警  │
        │  generate_page.c 各页面生成（约 5400 行）      │
        │  label / option / nano_option  三种链表控件    │
        │  gui.h(GUI_BASIC) / font_lib / loadbit / lcd_drive │
        ├──────────────────────────────────────────────┤
        │  logic/   业务逻辑层                          │
        │  logic.c         主循环 main_loop()           │
        │  dissolved_oxygen.c 设备链表 + DO 业务（1500+ 行）│
        │  setting.c       参数配置 + Flash 读写         │
        │  log.c           历史数据记录 / 导出            │
        │  algorith.c/calculate.c  滤波与队列算法         │
        │  <各型号传感器>.c  各探头 Modbus 组帧实现       │
        ├──────────────────────────────────────────────┤
        │  hardware/ 驱动与协议解析层                    │
        │  data_processing.c  接收分发 + 各型号解析状态机 │
        │  rs485.c / rs485.h  RS485 收发框架与设备 ID 表  │
        │  rtc / gps / ch340e / key / battery / bee /    │
        │  bmp280 / spa06 / spi_flash / soft_iic / lcd   │
        ├──────────────────────────────────────────────┤
        │  Core/ + Drivers/  HAL 初始化与中断（CubeMX）   │
        └──────────────────────────────────────────────┘
```

### 3.2 模块职责一览

| 文件 | 职责 |
|---|---|
| `Core/Src/main.c` | `HAL_Init` → 时钟 → 外设初始化 → `hardware_init()` → USB 充电显示 → `while(1) main_loop()` |
| `logic/logic.c` | `hardware_init()`（外设启动顺序）、`software_init()`（开机 logo/状态栏）、`main_loop()` 主循环、设备插拔处理、报警闪烁 |
| `hardware/rs485.c/.h` | RS485 收发、设备计数、断连判定、循环发送与重发、所有传感器 Modbus ID 宏、`DOProbe_t` 设备结构体定义 |
| `hardware/data_processing.c` | 串口中断回调、CRC 校验后按 `rx_buf[0]` 分发到各型号解析函数、**每个型号一个发送/接收状态机** |
| `logic/dissolved_oxygen.c` | 设备链表（`DO_AddProbe` 按 Modbus ID 生成节点）、当前设备指针、气压/盐度自动下发、数值滤波与锁定 |
| `logic/setting.c/.h` | 全部可配置参数的结构体 `setting_t`、Flash 持久化、各项 getter/setter |
| `logic/log.c` | 历史数据写入/读取/删除、通过 CH340E 导出记录 |
| `gui/interfacial.c` | 页面枚举 `PAGE_NUM` 调度、按键逻辑（上/下/确定/取消/校准/菜单）、弹窗、报警提示、传感器搜索时序 |
| `gui/generate_page.c` | 每个界面构建标签与选项链表；按传感器型号/厂商决定校准菜单 |
| `gui/label.c / option.c / nano_option.c` | 显示链表控件：只读标签 / 可选中大选项 / 可编辑数值与子项 |
| `hardware/heart.c` | 1 ms 中断内的定时标志生成（`main_tim`/`key_tim`/`functions_tim`）、按键计时状态机 |
| `hardware/lcd_drive.c` | UC1698U 显存（160×160）与刷新 |

### 3.3 启动流程

```text
main()
 ├─ HAL_Init() / SystemClock_Config()            32 MHz
 ├─ MX_GPIO/ADC/SPI1/TIM2/IWDG_Init()
 ├─ hardware_init()
 │   ├─ TIM2 1ms 中断启动、SPI1 使能
 │   ├─ MX_DMA_Init() → USART3/2/1（**DMA 必须先于串口初始化**，否则只能收到最后 1 字节）
 │   ├─ rs485_usart.init(&huart3) / gps_usart.init(&huart2) / ch340e_usart.init(&huart1)
 │   ├─ bmp280.init(&hspi1)              自动判型 BMP280 / SPA06-003
 │   ├─ HYM8563_init()                    RTC
 │   └─ lcdware_init()                    读 Flash 设置 → GUI 初始化 → 设置起始刷新行
 ├─ USB_Battery_Show()                   插 USB 时进入充电显示循环（可关机）

while(1) main_loop()
     └─ software_init() 在开机/唤醒后初始化界面：logo、状态栏、进入 PAGE_0_START
```

### 3.4 主循环与中断调度

**中断侧（1 ms，TIM2 → `HAL_TIM_PeriodElapsedCallback`）**

```c
feed_dog();        // 喂狗
main_tim();        // 产生 Lcd/Rtc/Bmp/Bat/GPS/Interfacial/Warning/Test 等周期标志
key_tim();         // 按键消抖与长按计时（短按 10ms / 长按 3000ms / 超长按 6000ms）
bee_handle();      // 蜂鸣器节奏
functions_tim();   // 自动关机计时等
logo_tim();
rs485_TimHandle(); // 每 600 ms（RS485_CIRCULAR_TIM）置「需要发送」→ 触发轮询读数
```

串口空闲中断：`USARTx_IRQHandler` → `HAL_UART_IDLE_HANDLER()` → 各模块 `*_IDLECallBack()`
（RS485/GPS/CH340E 均用「DMA 接收 + 空闲中断」，回调里 `HAL_UART_AbortReceive()` 后重新挂载 DMA）。

**主循环侧（`logic.c: main_loop()`）**顺序为：

1. RTC 标志 → 刷新时间、闪 LED
2. 气压标志（约 2 s）→ 读气压/温度；按间隔保存历史数据（`Save_Data()`）
3. GPS 标志 / 电池标志（约 1.5 s）→ 解析 GPS、刷新电量图标，低电自动关机计数
4. `key_scan()` 按键扫描
5. 界面刷新标志（250 ms）→ `interfacial_refresh()` 重建当前页显示
6. `btn_func()` 按键事件派发
7. 弹窗销毁、报警图标闪烁
8. 屏幕刷新标志（TIM_REFRESHLCD）→ `GUI_UpdateDisplay()` 把显存推到 LCD
9. `ch340_DataHandle()` 上位机指令处理
10. `rs485_DataHandle()` 传感器应答解析（CRC 校验后按 ID 分发）
11. 断连标志 → `logic_DeviceDestory()` 摘除设备并重生成界面
12. 当前设备为溶解氧时 → `DO_UpdatePressSal()` 把仪表气压/盐度变化下发到探头
13. 需要发送 → `rs485_SendBuf()`
14. `RTC_AutoShut()` 自动关机

> 这是**裸机（无 RTOS）**架构：定时中断只置标志，重活全部在 `main_loop()` 里轮询完成。

---

## 4. 关键机制详解

### 4.1 RS485 通信框架

`rs485_t` 结构体（`hardware/rs485.h`）承载一次通信的全部状态：

```c
typedef struct {
    uint8_t tx_flag :1;            // 已发送，等待回传
    uint8_t rx_flag :1;            // 已收到一帧
    uint8_t rs485_circular_sent:1; // 循环发送中
    uint8_t rs485_need_sent:1;     // 需要发送一次
    uint8_t rs485_is_disconnect:1; // 设备断开
    uint8_t device_count;          // 已连接设备数
    uint8_t resend_count;          // 重发计数
    uint8_t tx_size, rx_size;
    uint8_t rx_buf[64], tx_buf[64];
    rs485_sent_type sent_type;     // 本次指令的语义（状态机依据）
    void (*init)(UART_HandleTypeDef*);
    UART_HandleTypeDef *huart;
} rs485_t;
```

要点：

- **组帧**：各型号文件负责填 `tx_buf[]`，最后调 `SetCrc(buf, len)` 追加 CRC16 并写 `tx_size`，再 `rs485_SetSentType(语义)`。
- **发送**：主循环中 `rs485_SendBuf()` 拉高 DE → `HAL_UART_Transmit` → 拉低 DE。
- **重发与断连**：每次发送 `resend_count++`，达到 `RESEND_MAX = 4` 即判定设备断开（`rs485_SetIsDisconnect()`），
  搜索模式（`DO_SendType_GetModbusId`）不受此限制。
- **循环发送**：TIM2 每 600 ms 置 `need_sent`，主循环据此轮询发送读数指令；`rs485_circular_sent` 用于区分「轮询」与「一次性写操作」。
- **设备搜索**：`rs485_Search_Sensor()`（位于 `gui/interfacial.c`）按序号轮询各候选 ID 的 `GetModbusId()` 指令，
  收到有效应答即 `DO_AddProbe(id)` 挂载节点。582/702 机型、单/双探头场景走不同分支。

### 4.2 数据解析分发（`hardware/data_processing.c`）

```c
void rs485_DataHandle(void)
{
    if (!rs485_GetRxFlag()) return;
    rs485_ClearRxFlag();
    if (rs485_usart.rx_size < 5) return;
    if (!CheckCrc(rs485_usart.rx_buf, rs485_usart.rx_size)) return;   // CRC 校验

    switch (rs485_usart.rx_buf[0]) {          // 第 1 字节 = 从站地址
        case DO_HF_DY56_ModbusID:  DO_HF_DY56_DataHandle();  break;
        case DO_HF_DY12_ModbusID:  DO_HF_DY12_DataHandle();  break;
        case pH_shenghui_ModbusID: pH_shenghui_DataHandle(); break;
        /* ... 共 30+ 个型号 ... */
        default: break;
    }
}
```

每个 `*_DataHandle()` 内部是一个**基于 `rs485_GetSentType()` 的状态机**，典型流程：

```text
GetModbusId 应答 → SetSN_SH()/GetSHWVersion() → 读盐度 → 读气压 → 读温度偏移 → DO_SetIsInit()
                          ↓
循环发送 GetValue → 解析 mg/L、%、温度 → 滑动平均 filter → 写显示字符串 xxx_arr[] → 数值锁定检查
                          ↓
写操作：Set_Cmd_open(写使能) → 写目标寄存器 → Set_Cmd_close(写禁止) → 成功弹窗 + 恢复循环读取
```

以海发 DY56 为例的校准三步（官方协议）：

```text
写使能 open ：06 00 28 5A 5A
零位校正     ：06 00 01 00 03     满氧校正：06 00 01 00 04
温度偏移     ：06 00 19 xx xx     （×100，int16）
写禁止 close：06 00 28 00 00
恢复厂家设置 ：06 00 00 FE FE
```

**注意**：写指令在串口日志中出现两次属正常现象——第一次是传感器对 0x06 的**回显帧**，不是超时重发。

### 4.3 传感器设备管理（comA / comB / cur）

`hardware/rs485.h` 中 `DOProbe_t` 是**所有传感器类型共用的"设备"结构体**
（溶解氧的字段命名被复用到 pH/浊度/电导率等，`float_u DOmgl`、`temperature_arr` 等对其它类型表示各自的主测量值）。

三个设备上下文：

| 变量 | 含义 |
|---|---|
| `comA_DO.DO_list` | 第 1 个被识别的设备 |
| `comB_DO.DO_list` | 第 2 个被识别的设备 |
| `cur_DO.DO_list` | 当前正在显示的设备（上下键切换） |

`DO_AddProbe(id)` 依据 Modbus ID 推断 `SENSOR_TYPE` 与显示名，并按是否已有 comA 决定挂到 A 还是 B。
`Save_Data()`、`logic_DeviceDestory()` 中均需在 A/B 之间来回切换指针（见 `logic.c`）。

### 4.4 界面系统

界面由三套链表组成，全部**动态分配**（`malloc`）：

| 控件 | 结构体 | 用途 |
|---|---|---|
| 可选中大项 | `node_option` | 菜单项，可跳转 `next_page`，可挂子项 `son_option` |
| 子项/数值 | `node_nano` | 时间各位数字、校准数值、保存按钮等 |
| 只读显示 | `struct_label` | 数值标签、单位、图标，由 `xxx_arr[]` 驱动刷新 |

- `PAGE_NUM`（`gui/gui.h`）枚举了 100+ 个页面（一级菜单 → 五级校准页）。
- `interfacial_SetPage(page, isBack)`：一个巨型 `switch`，为每个页面调用对应 `generate_*()` 构建链表，
  并注册按键回调（`btnUpDownFunc_register` / `btnOkEscFunc_register`）。
- `interfacial_refresh()`：按当前页重新写入显存（状态栏、标题、标签、选项高亮）。
- 显示区域为 160×160 点阵，`set_StartPoint(row)` 支持局部刷新以减少闪烁。
- 弹窗：`generate_MessageBox(MESSAGE_SETTING/SAVELOG/SUCCESSFUL/DELETE/tixing/Close, ...)`。
- 报警：`PAGE_0_START` 上报警图标按 `TIM_WARNING`（1306 ms）闪烁。

### 4.5 参数设置与 Flash 存储布局

配置结构体 `setting_t`（`logic/setting.h`）包含：语言、开机 logo、屏幕对比度、电池类型、仪表型号、
按键音/报警音、自动关机、自动锁定（分传感器类型）、报警门限（分类型高低门限）、滑动平均开关与次数、
盐度/气压补偿、温度单位、盐度单位、GPS 开关、间隔保存时间等。

存储介质：**W25QXX SPI Flash**，地址布局（`Core/Inc/main.h`）：

| 地址 | 内容 |
|---|---|
| `0x000000` | 首次运行标志（`SETTING_FIRSTRUN_JUDGE = 0xE0`，版本维护用） |
| `0x000100` | 恢复出厂时使用中文/英文 |
| `0x000200` | 各类记录条数 |
| `0x000400` | 开机 logo 定制 |
| `0x000500` | 是否带 GPS |
| `0x000600` | 屏幕对比度 |
| `0x000700` | 电池类型（0=锂电 1=干电池） |
| `0x000800` | 仪表型号（0=D702 1=B580） |
| `0x000900` | `setting_t` 整体配置区（`SettingToFlash()` 直接整块写入） |
| `0x100000` 起 | 历史数据区，**每种传感器类型一块，每类 `LOG_MAX_COUNT = 5000` 条，每条按 256 字节对齐**（`index << 8`） |

> 修改 `setting_t` 字段或结构大小时务必同步提升 `SETTING_FIRSTRUN_JUDGE`，否则老设备读到的
> 是错位的旧结构。

### 4.6 历史数据（日志）

- `log_t` 记录：设备类型字符串、日期时间、探头 SN、GPS 经纬度 + 南北/东西，以及 20+ 个测量量（`log_FloatData_t`）。
- `log_SaveData(SENSOR_TYPE)`：按类型写入对应地址块；双探头时先存 A 再存 B。
- 触发方式：主界面下按「间隔保存时间」（`setting_GetAutoIntervalTime()`）自动存（气压标志 2 s 节拍累加），
  或手动保存。
- 导出：`log_SendBytes()` / `log_SendCount()` 通过 **CH340E（USART1）** 把记录传给上位机。
- CH340E 同时承担**上位机配置通道**：`0xFE` 开头的帧可恢复出厂、设置时间、切换定制 logo、清 Flash 等。

### 4.7 算法与数值处理

| 文件 | 功能 |
|---|---|
| `logic/algorith.c` | `filter_t` 环形滑动平均：`filter_init/inset2arr/get_avg/clear`，每类传感器独立队列（`queue_domgl`/`queue_dopercent`/`queue_temp`） |
| `logic/calculate.c` | `LiQueue` 链式队列求平均、`float_format()` 保留两位小数 |
| `dissolved_oxygen.c` | `DO_ValueCheckFirst()` 首值处理、`CheckValueLock()` 数值稳定自动锁定（`SHAKE_TIMES=3`、`SAME_TIMES=3`）、`DO_zero_buf_mgl/percent()` 显示缓冲清零 |

### 4.8 校准体系

校准入口为 `generate_Cal()`（`gui/generate_page.c`），先按入口页取到目标探头，再把 **厂商（`manufacturer`）**
传入 `generate_cal_DO()` 决定校准菜单：

- **海发（hyphive）** 系列（DY12 / DY56 / DY05 / DO59）：零点 / 满氧 / 温度偏移（写使能三步式）
- **昇辉（或其它）** 系列：零点 / 斜率 / K、B 两点式
- pH：标液组管理（4.01 / 6.86 / 9.18）
- COD：零点 / 斜率 / 浊度零点与斜率 / 补偿系数 / 内部校准
- 氨氮：两点 + pH 三点；浊度：三点；电导率：单点/零点

「恢复出厂」分两级：仪表级（`PAGE_3_RESERT`，清配置）与传感器级（`PAGE_1_RESETCAL`，清探头校准系数）。

---

## 5. 支持的传感器型号与 Modbus ID

定义于 `hardware/rs485.h`，分发表在 `hardware/data_processing.c`。

| 类型 | 驱动文件 | Modbus ID |
|---|---|---|
| 溶解氧 | `DO_shenghui.c` | 90 |
| 溶解氧 | `DO_HaiFa_DY12.c` | 33 |
| 溶解氧 | `DO_HaiFa_DY56.c` | 81 |
| 溶解氧 | `DY05.c` | 7 |
| 溶解氧 | `DO59.c` | 6 |
| 溶解氧 | （`dissolved_oxygen.c` 内）HF1012 | 254（**已软弃用**，ID 让位给 DY56，代码保留为死代码） |
| pH | `pH_DpH07.c` | 1 |
| pH | `pH_shenghui.c` | 92 |
| 浊度 | `Tur_shenghui.c` | 91 |
| 浊度 | `DZ09.c` | 13 |
| 电导率 | `EC_shenghui.c` | 93 |
| 电导率 | `EC_DE26.c` | 52 |
| 电导率 | `EC_DE40.c` | 3 |
| 盐度 | `EC_DE40.c`（DS46 分支） | 85 |
| TDS | `TDS_DT49.c` | 86 |
| ORP | `ORP_DR31.c` | 9 |
| 氨氮 | `NH3N_DN02.c` | 34 |
| 氨氮 | `NH3N_shenghui.c` | 94 |
| 叶绿素 | `Chl_shenghui.c` | 96 |
| 蓝绿藻 | `BGA_shenghui.c` | 97 |
| COD | `COD_DC18.c` | 10 |
| COD | `COD_DC18.c`（DC17 分支） | 74 |
| COD | `COD_shenghui.c` | 95 |
| 水中油 | `OiW_yushan.c` | 78 |
| 水中油 | `OiW_yushan.c`（DA511） | 42 |
| 水中油 | `OiW_Guohong.c` | 29 |
| MLSS 污泥浓度 | `MLSS_Tianjian.c` | 77 |
| 悬浮物 TSS | `DX01.c` | 5 |
| 余氯 CL | `DL312.c` | 58 |
| MLSS（蓝创） | `MLSS_lanchang.c` | 83（已在工程中，但未接入 `DO_AddProbe` 与分发表） |

---

## 6. 新增一款传感器的改造清单

按现有约定，接入一个新探头需要动以下 8 处（以新增「XX」型号、ID = `NN` 为例）：

1. **`hardware/rs485.h`**：新增 ID 宏 `#define XX_ModbusID NN`；若属新类型，追加 `SENSOR_TYPE` 枚举。
2. **`logic/XX.c` / `logic/XX.h`**：参考同类型已有型号（如 `DO_HaiFa_DY56.c`）实现组帧函数
   （`XX_rs485_GetModbusId` / `GetValue` / `SetXxx` …），每个函数末尾 `SetCrc()` + `rs485_SetSentType()`。
3. **`hardware/data_processing.c`**：新增 `void XX_DataHandle(void)`，并在 `rs485_DataHandle()` 的
   `switch (rs485_usart.rx_buf[0])` 中注册 `case XX_ModbusID:`。
4. **`logic/dissolved_oxygen.c`**：`DO_AddProbe()` 中新增 `case XX_ModbusID:`，设置显示名（`snprintf(p->name,...)`）、
   `add_Type`，并按需初始化滑动平均队列（`filter_init`）。
5. **`gui/interfacial.c`**：
   - `rs485_Search_Sensor()` 各分支（单探头 / 双探头 / 702 机型）新增轮询与读数 `case`；
   - 各设置/校准页的按键处理分支新增 `case XX_ModbusID:`。
6. **`gui/generate_page.c`**：校准菜单显示（零点/两点 vs 零点/满氧）、单位、参数页等处新增分支。
7. **`MDK-ARM/PortableOxygen.uvprojx`**：把 `XX.c` 加入 `logic` 分组。
8. **协议存档**：把寄存器表与帧格式记录到 `code/.workbuddy/memory/`（现有约定）。

### 踩坑清单（来自既往开发记录）

- **克隆型号文件时必须给模块内的全局变量加 `static`**。否则会与源文件产生 `multiple definition` 链接错误
  （DY56 从 DY12 克隆时 `Sal_Flag`、`Press_Flag`、`Zero_Flag`、`Full_Flag`、`SalValue` 等 8 个符号重复定义）。
- **注意 `main.h` 中的 `DEBUG` 宏**：`DEBUG 1` 时 `STATIC` 被展开为空，`hardware/rs485.c` 里的
  `STATIC uint8_t is_change_senesor` 会变成**全局符号**。发布前应把 `DEBUG` 置 0 或改用真正的 `static`。
- **双探头场景下写 SN 必须按通道判断**：曾出现 `SetSN_SH(get_COMADo())` 硬编码 A 通道，导致挂在 comB 的
  探头 SN / 软硬件版本显示空白。应使用与 `UpdateTemp2DO` 一致的三元表达式。
- **「恢复出厂」必须同步本地 `setting` 缓存**：只写传感器寄存器会导致主循环 `DO_UpdatePressSal()`
  检测到「本地旧值 ≠ 探头新值」而把旧参数自动写回，表现为「出厂流程发两遍」且参数没变。
- **断连重连要清业务标志**：恢复出厂等长流程被超时打断时，残留的标志会在下次写操作时误触发，
  应在 `GetModbusId` 应答（重连点）统一清零。
- **写指令串口上出现两次是传感器回显**，不是重发，勿据此改进协议。

---

## 7. 编译与烧录

### 7.1 Keil MDK 工程

| 项目 | 值 |
|---|---|
| 工程文件 | `code/MDK-ARM/PortableOxygen.uvprojx` |
| Target | `PortableOxygen` |
| 器件 | STM32L152VE（Keil.STM32L1xx_DFP.1.2.0） |
| 工具链 | ARM Compiler 6（ARMCLANG V6.14，`uAC6=1`），优化等级 `-O0` |
| 语言标准 | C99 / C11（勾选 `uC99`） |
| 预处理宏 | `USE_HAL_DRIVER,STM32L152xE` |
| 头文件路径 | `../Core/Inc;../Drivers/STM32L1xx_HAL_Driver/Inc;[...]/Inc/Legacy;../Drivers/CMSIS/Device/ST/STM32L1xx/Include;../Drivers/CMSIS/Include;../gui;../hardware;../logic` |
| 输出目录 | `code/MDK-ARM/PortableOxygen/` |
| 当前体积 | ROM 361 968 B（353.5 KB）/ RAM 40 640 B（39.7 KB）（Flash 512 KB / RAM 80 KB） |
| 当前告警 | 1 条：`GUI_StockC.h` 中 `typedef 'TCOLOR'` 重复定义（C11 特性，可忽略） |

编译步骤：用 Keil µVision 打开 `.uvprojx` → Rebuild → 产物 `PortableOxygen.hex` / `.bin` / `.axf`。

### 7.2 烧录

- 调试器配置见 `code/MDK-ARM/PortableOxygen.uvoptx` 与 `DebugConfig/*.dbgconf`，支持 **J-Link** 与 ST-Link。
- 工程根目录已按团队约定保留固件产物（`.axf` / `.bin` / `.hex`）入库，见 `code/.gitignore` 白名单。

### 7.3 目录约定与 `.gitignore`

`code/.gitignore` 忽略全部 Keil 中间产物（`*.o`、`*.d`、`*.crf`、`*.map`、`*.lst`、`*.sct`、`*.uvguix*`、`RTE/`、`Objects/`、`Listings/`、`DebugConfig/` 等）
与 CubeMX 的 `.mxproject`，但**显式白名单保留** `*.uvprojx`、`*.h`、`*.c`、`*.ioc`、`*.axf`、`*.bin`、`*.hex`。

---

## 8. 定制与版本

| 定制项 | 存储 / 代码位置 |
|---|---|
| 开机 Logo（恒蓝 HENGLAN / 鲁恒 LUHENG / 中兴 ZHONGXING / 力和 LIHE） | Flash `0x000400`；图片数组在 `gui/loadbit.c`，绘制于 `logic.c: software_init()` |
| 仪表型号 B580 / D702 | Flash `0x000800`；`setting_Get_Type()` 返回 0=702、1=580，影响搜索策略与显示 |
| 软件版本字符串 | `gui/interfacial.c` → `const uint8_t software_version[] = "V2.2.9";` |
| 硬件版本 VER_A~VER_D | PB12/PB13 硬件电阻编码，`update_HWVersion()` |
| 首次运行标志 / 配置结构版本 | `main.h` → `SETTING_FIRSTRUN_JUDGE = 0xE0`（**结构变更时必须更新**） |

---

## 9. 注意事项

1. **源码编码不统一（重要）**。全项目实测分布：

   | 编码 | 文件数 | 主要分布 |
   |---|---|---|
   | UTF-8 | 54 | `hardware/` 全部、`gui/` 的 .c、`Core/Inc/main.h`、`logic/` 的 5 个核心文件（`logic.c`、`dissolved_oxygen.c`、`setting.c`、`log.c`、`algorith.c`、`calculate.c`）、以及**全部近期新增文件**（如 `logic/DO_HaiFa_DY56.c`） |
   | GBK | 33 | `Core/Src/main.c`、5 个 `gui/*.h`、28 个 `logic/` 早期传感器驱动（含 `COD_DC18.h`、`OiW_Guohong.h`） |
   | 纯 ASCII | 44 | 无中文注释的文件 |
   | 注释已损坏 | 1 | `hardware/data_processing.h`（中文被双重编码为 `//¿ÕÏÐÖÐ¶Ï`，仅影响可读性，不影响编译） |

   **编辑任何文件前必须先确认其编码**。用 UTF-8 解码 GBK 文件会把每个中文字节替换成 ``(U+FFFD)
   并重新序列化为 UTF-8，**该文件全部中文注释将永久丢失**（实测：编辑一个 142 行的 GBK 驱动文件后，
   88 个汉字全部变成替换字符）。新增文件统一用 **UTF-8**——这已是本项目的实际演进方向。
   详细处置流程见 [AGENTS.md](AGENTS.md) 第 2 节。
2. **裸机 + 动态内存**：界面链表与设备节点使用 `malloc`，无内存池与碎片回收机制；
   `interfacial_ClearLabel()`、`CurInterfacial_Destory()` 的调用时机直接影响长期运行稳定性。
3. **DMA 初始化顺序**：`MX_DMA_Init()` 必须在 `MX_USARTx_UART_Init()` 之前，否则串口只能收到最后一个字节。
4. **RS485 空闲中断里会 `HAL_UART_AbortReceive()` 再重启 DMA**，这段逻辑改动风险高。
5. **`setting_t` 结构体变更需同步 `SETTING_FIRSTRUN_JUDGE`**，否则老设备升级后会读到错位配置。
6. **历史数据区每条记录按 256 字节对齐**（`index << 8`），单类型容量固定 5000 条。
7. **`DEBUG` 宏影响 `STATIC` 语义**（见 §6 踩坑清单），发布版本请确认取值。
8. `code/PortableOxygen.map - 快捷方式.lnk` 是一个 Windows 快捷方式文件，已被 `.gitignore` 忽略。
9. `docs/` 目录当前为空，可在此补充协议文档、硬件资料与测试报告。

---

## 10. 分支与提交约定

- 主分支：`master`；当前开发分支：`develop_fang`
- 远端：`origin` → `https://github.com/wcf8822/wcf1314.git`
- 提交信息风格：`<版本号/日期> <改动摘要>`，例如
  `d7620be 2026-08-27 v2.2.7`、`e70a579 修改81地址的通信协议为海发自己的协议 2026-09-20`
