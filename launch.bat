@echo off
title Deadlock Defender — IPC Engine
wt wsl bash "/mnt/d/CSE323  project/deadlock-defender/run.sh"
if %errorlevel% neq 0 (
    wsl bash "/mnt/d/CSE323  project/deadlock-defender/run.sh"
)
