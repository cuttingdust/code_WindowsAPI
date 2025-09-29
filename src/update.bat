@echo off
cmake -S . -B build -A Win32
timeout 2
::pause