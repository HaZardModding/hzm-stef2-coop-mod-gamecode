@echo off
cd code\linux\
bash -c "make -j4 build_release"
pause

bash -c "make build_release"
pause