# IPC —— 基于 RK3566 的嵌入式 IP 摄像机
## 项目简介
本项目是一个运行在 **Rockchip RK3566** 平台上的嵌入式 IP 摄像机（IPC）应用程序。  
它通过 **V4L2 (Video4Linux2)** 接口从摄像头采集原始视频帧，使用 **x264** 库将其编码为 H.264，再借助 **ZLMediaKit** 框架将视频流推送到 **RTSP** 服务器，实现远程实时观看。
```
摄像头 (V4L2)
    │
    ▼  YUYV 原始帧
格式转换 (YUYV → YUV420P)
    │
    ▼  YUV420P 帧
H.264 编码 (x264)
    │
    ▼  H.264 NAL 单元
RTSP 推流 (ZLMediaKit)
    │
    ▼
rtsp://<设备IP>:554/live/test
```
---
## 目录结构
```
IPC/
├── include/            # 头文件
│   ├── common.h        # 公共常量（分辨率、帧率等）
│   ├── get_yuv.h       # V4L2 视频采集接口
│   ├── yuyv2yuv420p.h  # 格式转换接口
│   ├── yuv420p2h264.h  # H.264 编码接口
│   ├── push264.h       # RTSP 推流接口
│   ├── x264.h          # x264 库头文件
│   └── mk_*.h          # ZLMediaKit 库头文件
├── src/                # 源文件
│   ├── get_yuv.c       # V4L2 视频采集实现
│   ├── yuyv2yuv420p.c  # YUYV 转 YUV420P 实现
│   ├── yuv420p2h264.c  # H.264 编码实现
│   └── push264.c       # RTSP 推流实现
├── main.c              # 程序入口
├── Makefile            # 构建配置
└── build/              # 编译产物（自动生成）
```
---
## 功能特性
- 通过 V4L2 接口采集摄像头视频（设备节点：`/dev/video9`）
- YUYV 格式转换为 YUV420P 平面格式
- 基于 x264 对视频帧进行 H.264 编码
- 通过 ZLMediaKit 建立 RTSP 服务器并推流
- 默认视频参数：分辨率 `640×480`，帧率 `30 FPS`
- 编译目标架构：ARM aarch64（RK3566）
---
## 依赖项
| 依赖库 | 说明 |
|--------|------|
| **x264** | H.264 视频编码库 |
| **ZLMediaKit** (`libmk_api`, `libZLToolKit`) | RTSP 媒体服务器框架 |
| **Linux V4L2** | 内核摄像头驱动接口（系统自带） |
**库文件路径（Makefile 中配置）：**
```
/home/book/rk3566/IPcamera/rk3566-x264/lib          # x264 库
/home/book/rk3566/IPcamera/ZLMediaKit/release/linux/Release  # ZLMediaKit 库
/home/book/rk3566/IPcamera/ZLMediaKit/build/3rdpart/ZLToolKit/lib
```
> 如需在不同环境下编译，请修改 `Makefile` 中对应的 `-L` 路径。
---
## 编译
### 前提条件
- 安装 aarch64 交叉编译工具链：`aarch64-buildroot-linux-gnu-gcc`
- 已编译好 x264 和 ZLMediaKit 的 aarch64 版本库文件
### 编译命令
```bash
make
```
编译成功后会在当前目录生成可执行文件 `IPC`（aarch64 ELF 格式）。
### 清理构建产物
```bash
make clean
```
### 自定义交叉编译器
```bash
make CROSS_COMPILE=<你的交叉编译器前缀>
```
---
## 配置
在运行前，请根据实际环境修改以下配置：
| 配置项 | 文件 | 默认值 | 说明 |
|--------|------|--------|------|
| 视频分辨率 | `include/common.h` | `640×480` | 修改 `WIDTH` / `HEIGHT` |
| 帧率 | `include/common.h` | `30 FPS` | 修改 `FPS` |
| 摄像头设备 | `src/get_yuv.c` | `/dev/video9` | 修改对应 `open()` 路径 |
| RTSP 服务器地址 | `include/push264.h` | `192.168.137.12:554` | 修改 `URL` 宏 |
---
## 运行
将编译好的 `IPC` 可执行文件传输到目标设备（RK3566 板卡），然后运行：
```bash
./IPC
```
程序启动后，可通过以下 RTSP 地址访问视频流：
```
rtsp://<设备IP>:554/live/test
```
推荐使用 **VLC** 或 **ffplay** 播放：
```bash
# 使用 ffplay 播放
ffplay rtsp://<设备IP>:554/live/test
# 使用 VLC 播放
vlc rtsp://<设备IP>:554/live/test
```
---
## 开发环境
| 项目 | 信息 |
|------|------|
| 目标平台 | Rockchip RK3566 |
| 操作系统 | Linux (buildroot) |
| 编程语言 | C (gnu11) |
| 交叉编译器 | `aarch64-buildroot-linux-gnu-gcc` |
| 构建工具 | GNU Make |
---

