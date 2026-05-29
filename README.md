<<<<<<< HEAD
# FlashBoot - USB启动盘制作工具

一款基于 Qt 6 的 USB 启动盘制作工具，支持 UEFI/BIOS 双模式启动。

## 功能特性

- ❌ USB设备自动检测与管理
- ✅ ISO镜像解析（支持 Windows、Linux 等多种镜像）
- ✅ GPT/MBR 分区方案
- ✅ UEFI + BIOS 双模式支持
- ✅ 实时进度显示
- ✅ 详细日志记录
- ✅ 深色/浅色主题切换
- ❓ 危险操作二次确认

## 系统要求

- Windows 10/11 (64位)
- Qt 6.5+ (推荐 6.11.1)
- CMake 3.20+
- MinGW-w64 11+ 或 MSVC 2019+

## 编译步骤

### 方法一：使用 Qt Creator（推荐）

1. **打开项目**
   - 启动 Qt Creator
   - 点击「文件」→「打开文件或项目」
   - 选择 `FlashBoot/CMakeLists.txt`
2. **配置构建套件**
   - 在「构建套件」选择界面，选择您安装的 Qt 版本（如 Qt 6.11.1 MinGW 64-bit）
   - 点击「配置项目」
3. **构建项目**
   - 点击左下角的「构建」按钮（锤子图标）
   - 或使用快捷键 `Ctrl+B`
4. **运行程序**
   - 构建成功后，点击「运行」按钮（绿色三角形图标）

### 方法二：使用命令行

#### 1. 设置环境变量

```powershell
# 设置 Qt 环境变量（根据您的安装路径调整）
$env:Qt6_DIR = "F:\QTt\6.11.1\mingw_64"
$env:Path += ";F:\QTt\6.11.1\mingw_64\bin"

# 设置 CMake 路径（如果未添加到系统 PATH）
$env:Path += ";C:\Program Files\CMake\bin"
```

#### 2. 创建构建目录

```powershell
cd FlashBoot
mkdir -p build
cd build
```

#### 3. 配置项目

**使用 MinGW（推荐）：**

```powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="F:/QTt/6.11.1/mingw_64/lib/cmake"
```

**使用 MSVC：**

```powershell
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="F:/QTt/6.11.1/msvc2019_64/lib/cmake"
```

#### 4. 编译项目

```powershell
# MinGW
cmake --build . --config Release -j4

# MSVC
cmake --build . --config Release
```

#### 5. 部署依赖

```powershell
# MinGW
F:\QTt\6.11.1\mingw_64\bin\windeployqt.exe FlashBoot.exe

# MSVC
F:\QTt\6.11.1\msvc2019_64\bin\windeployqt.exe Release\FlashBoot.exe
```

#### 6. 运行程序

```powershell
# MinGW
.\FlashBoot.exe

# MSVC
.\Release\FlashBoot.exe
```

## 项目结构

```
FlashBoot/
├── CMakeLists.txt          # CMake 配置文件
├── cmake/                  # CMake 模块
│   └── QtDeploy.cmake
├── resources/              # 资源文件
│   ├── icons/              # SVG 图标
│   ├── styles/             # 主题样式
│   └── translations/       # 翻译文件
└── src/                    # 源代码
    ├── core/               # 业务逻辑层
    │   └── models/         # 数据模型
    ├── hal/                # 硬件抽象层
    ├── ui/                 # UI层
    │   └── widgets/        # 自定义控件
    ├── utils/              # 工具类
    └── main.cpp            # 程序入口
```

## 目录说明

| 目录                  | 说明                   |
| ------------------- | -------------------- |
| `src/core/`         | USB设备管理、ISO解析、格式化逻辑  |
| `src/hal/`          | Windows API 封装（磁盘操作） |
| `src/ui/`           | 主窗口、面板、对话框等UI组件      |
| `src/utils/`        | 日志、主题、哈希计算等工具类       |
| `resources/icons/`  | 应用程序图标资源             |
| `resources/styles/` | 浅色/深色主题样式表           |

## 常见问题

### Q1: CMake 找不到 Qt

**错误信息：**

```
Could not find a package configuration file provided by "Qt6"
```

**解决方案：**
确保设置了正确的 `CMAKE_PREFIX_PATH`，例如：

```powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="F:/QTt/6.11.1/mingw_64/lib/cmake"
```

### Q2: 程序启动后立即退出

**原因：** 缺少 Qt 运行时库

**解决方案：** 使用 `windeployqt` 部署依赖：

```powershell
windeployqt.exe FlashBoot.exe
```

### Q3: 编译时提示缺少某些头文件

**原因：** 可能缺少某些 Qt 模块

**解决方案：** 在 Qt 安装器中确保安装了以下组件：

- Qt 6.11.1 -> MinGW 64-bit
- Qt 6.11.1 -> Qt Network
- Qt 6.11.1 -> Qt SVG

## 许可证

MIT License

## 作者

FlashBoot Development Team
=======
# FlashBoot
>>>>>>> 2ac2e8d94ab719b3a4dd6cfae392d3e811225e38
