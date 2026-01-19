@echo off
chcp 65001 >nul
title 清理 SPIR-V 文件

echo.
echo 正在清理 SPIR-V 文件...
echo.

set COUNT=0
for %%f in (*.spv) do (
    echo 删除: %%f
    del "%%f"
    set /a COUNT+=1
)

echo.
if %COUNT% equ 0 (
    echo 没有找到 .spv 文件
) else (
    echo 已删除 %COUNT% 个 .spv 文件
)

echo.
pause