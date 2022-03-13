@echo off
cd code\linux\
bash -c "make -j4 build_release"
pause
test
