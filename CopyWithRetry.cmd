@echo off
REM Usage: CopyWithRetry.cmd "source_file" "target_directory"
REM Retries copying up to 10 times with a 1s delay (handles transient sharing violations)

if "%~1"=="" (
  echo ERROR: source file not specified.
  exit /b 4
)
if "%~2"=="" (
  echo ERROR: target directory not specified.
  exit /b 4
)

setlocal enabledelayedexpansion
set "SRC=%~1"
set "DST=%~2"
set /a MAX_ATTEMPTS=10
set /a ATTEMPT=0

REM Ensure target directory exists
if not exist "%DST%" (
  mkdir "%DST%" 2>nul
  if errorlevel 1 (
    echo ERROR: Failed to create target directory "%DST%".
    exit /b 4
  )
)

:TRY_COPY
set /a ATTEMPT+=1
echo Copy attempt !ATTEMPT!/%MAX_ATTEMPTS%: "%SRC%" -> "%DST%"
xcopy /Y /Q "%SRC%" "%DST%" >nul 2>&1
if %errorlevel%==0 (
  echo Copy succeeded.
  exit /b 0
)

REM If copy failed, wait a bit and retry (likely file-in-use / sharing violation)
echo Copy failed (error %errorlevel%). Waiting 1s before retry...
timeout /t 1 /nobreak >nul

if !ATTEMPT! GEQ !MAX_ATTEMPTS! (
  echo ERROR: Failed to copy after %MAX_ATTEMPTS% attempts. The target file may be locked (e.g. Unity Editor has the DLL loaded).
  exit /b 4
)
goto :TRY_COPY