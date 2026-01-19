@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion
title 智能着色器编译器 (终极稳定版)

echo.
echo ========================================
echo   智能着色器编译器 - 终极稳定版
echo   支持命令行参数: 1=GLES, 2=Vulkan, 3=OpenGL
echo ========================================
echo.

:: ========== 1. 初始化检查 ==========
if not defined VULKAN_SDK (
    echo [错误] 未找到 VULKAN_SDK 环境变量!
    echo 请安装 Vulkan SDK 或设置 VULKAN_SDK 环境变量
    pause
    exit /b 1
)

set "COMPILER=%VULKAN_SDK%\Bin\glslangValidator.exe"
if not exist "%COMPILER%" (
    echo [错误] 找不到编译器: %COMPILER%
    pause
    exit /b 1
)

:: ========== 2. 平台参数处理 (核心修复) ==========
set "ARGS_PLATFORM=%~1" & REM 去掉包裹的引号

:: 如果提供了参数，直接使用；否则显示菜单
if not "%ARGS_PLATFORM%"=="" (
    set "PLATFORM=%ARGS_PLATFORM%"
    goto :SETUP_PLATFORM
)

:: 无参数时显示菜单
echo 请选择目标平台:
echo   1. OpenGL ES / WebGL (GLSL ES 300)
echo   2. Vulkan (GLSL 450 - SPIR-V)
echo   3. Desktop OpenGL (GLSL 330)
echo.
set /p "PLATFORM=请输入选项 (1/2/3): "
if "%PLATFORM%"=="" (
    echo [错误] 未选择平台
    pause
    exit /b 1
)

:SETUP_PLATFORM
:: 验证平台参数
if "%PLATFORM%"=="1" (
    set "PLATFORM_NAME=GLES"
    set "HEADER_FILE=header_gles.glsl"
    set "TARGET_ENV=opengl"
    set "COMPILE_FLAG=-G"
    set "OUTPUT_SUFFIX=.gles"
) else if "%PLATFORM%"=="2" (
    set "PLATFORM_NAME=Vulkan"
    set "HEADER_FILE=header_vulkan.glsl"
    set "TARGET_ENV=vulkan1.0"
    set "COMPILE_FLAG=-V"
    set "OUTPUT_SUFFIX=.spv"
) else if "%PLATFORM%"=="3" (
    set "PLATFORM_NAME=OpenGL"
    set "HEADER_FILE=header_opengl.glsl"
    set "TARGET_ENV=opengl"
    set "COMPILE_FLAG=-G"
    set "OUTPUT_SUFFIX=.gl"
) else (
    echo [错误] 无效的平台选项: %PLATFORM%
    ::特殊含义的符号要加上小尖尖，这是格式化字符串的常规问题
    REM 特殊含义的符号要加上小尖尖，这是格式化字符串的常规问题
    echo 请使用: 1 ^(GLES^), 2 ^(Vulkan^), 或 3 ^(OpenGL^) & REM特殊含义的符号要加上小尖尖，这是格式化字符串的常规问题
    pause
    exit /b 1
)

echo.
echo [信息] 平台: !PLATFORM_NAME!
echo [信息] 使用Header: !HEADER_FILE!
echo [信息] 编译模式: !COMPILE_FLAG!
echo.

:: ========== 3. 确保Header文件存在 ==========
if not exist "!HEADER_FILE!" (
    echo [信息] 创建默认Header文件: !HEADER_FILE!
    
    if "!PLATFORM!"=="1" (
        echo #version 300 es > "!HEADER_FILE!"
        echo precision mediump float; >> "!HEADER_FILE!"
        echo #define IN in >> "!HEADER_FILE!"
        echo #define OUT out >> "!HEADER_FILE!"
    ) else if "!PLATFORM!"=="2" (
        echo #version 450 > "!HEADER_FILE!"
        echo #extension GL_ARB_separate_shader_objects : enable >> "!HEADER_FILE!"
        echo #define IN layout^(location = 0^) in >> "!HEADER_FILE!"
        echo #define OUT layout^(location = 0^) out >> "!HEADER_FILE!"
    ) else if "!PLATFORM!"=="3" (
        echo #version 330 core > "!HEADER_FILE!"
        echo #define IN layout^(location = 0^) in >> "!HEADER_FILE!"
        echo #define OUT out >> "!HEADER_FILE!"
    )
    echo.
)

:: ========== 4. 主编译流程 ==========
set "SUCCESS=0"
set "FAIL=0"
set "SKIP=0"

echo 开始编译着色器...
echo.

:: 定义需要处理的着色器类型
set "SHADER_TYPES=vs fs gs cs"
for %%t in (%SHADER_TYPES%) do (
    if exist "*.%%t" (
        for %%f in (*.%%t) do (
            call :COMPILE_SHADER "%%f" "%%t"
        )
    )
)

:: ========== 5. 输出结果 ==========
echo.
echo ========================================
echo 编译完成 [!PLATFORM_NAME!平台]
echo ========================================
echo 成功: !SUCCESS!
echo 跳过: !SKIP!
echo 失败: !FAIL!
echo.

if !FAIL! gtr 0 (
    echo [警告] 有编译失败的文件，请检查错误信息
) else if !SUCCESS! gtr 0 (
    echo [成功] 所有着色器编译完成!
) else (
    echo [信息] 没有需要编译的着色器
)

echo.
echo 输出文件带后缀: !OUTPUT_SUFFIX!
echo 快速启动: %~n0 1 ^(GLES^) ^| 2 ^(Vulkan^) ^| 3 ^(OpenGL^)
pause
exit /b 0

:: ========== 编译子程序 ==========
:COMPILE_SHADER
set "INPUT_FILE=%~1"
set "SHADER_TYPE=%~2"

:: 根据shader类型确定阶段(stage)
if "%SHADER_TYPE%"=="vs" set "STAGE=vert"
if "%SHADER_TYPE%"=="fs" set "STAGE=frag"
if "%SHADER_TYPE%"=="gs" set "STAGE=geom"
if "%SHADER_TYPE%"=="cs" set "STAGE=comp"

set "BASE_NAME=%~n1"
set "OUTPUT_FILE=!BASE_NAME!_!PLATFORM_NAME!!OUTPUT_SUFFIX!"
set "TEMP_FILE=!BASE_NAME!_temp.!STAGE!"

:: 智能时间戳检查：是否需要编译
set "NEED_COMPILE=0"
if not exist "!OUTPUT_FILE!" (
    set "NEED_COMPILE=1"
) else (
    for %%I in ("!INPUT_FILE!") do set "INPUT_TIME=%%~tI"
    for %%I in ("!OUTPUT_FILE!") do set "OUTPUT_TIME=%%~tI"
    for %%I in ("!HEADER_FILE!") do set "HEADER_TIME=%%~tI"
    
    :: 比较文件时间
    if "!INPUT_TIME!" gtr "!OUTPUT_TIME!" set "NEED_COMPILE=1"
    if "!HEADER_TIME!" gtr "!OUTPUT_TIME!" set "NEED_COMPILE=1"
)

if !NEED_COMPILE! equ 0 (
    echo   跳过: %~nx1
    set /a SKIP+=1
    goto :EOF
)

echo   编译: %~nx1 -^> !OUTPUT_FILE!

:: 合并Header和着色器源码
(
    type "!HEADER_FILE!"
    echo.
    type "!INPUT_FILE!"
) > "!TEMP_FILE!"

:: 执行编译命令
"%COMPILER%" !COMPILE_FLAG! -S !STAGE! "!TEMP_FILE!" --target-env !TARGET_ENV! -o "!OUTPUT_FILE!"

if errorlevel 1 (
    echo     [失败] 编译错误
    set /a FAIL+=1
    
    :: 显示前几行错误信息
    echo     建议检查: !TEMP_FILE! 文件内容
) else (
    echo     [成功]
    set /a SUCCESS+=1
)

:: 清理临时文件
if exist "!TEMP_FILE!" del "!TEMP_FILE!" >nul 2>nul
goto :EOF