@echo off
cd code/linux/
rd /s /q releasei386
rd /s /q debugi386
cd ../
cd dlls
rd  /s /q Debug
cd game
rd  /s /q Debug
cd ../../
cd Executable
rd  /s /q Debug
cd ../
rd /s /q ipch
cd Executable
rd /s /q Release
cd ../
cd dlls
cd game
rd /s /q Release
cd ../../
del coop-EF2.sdf
del EF2-game.sdf
del coop-EF2-game.sdf

del /s /q /f *.sbr
del /s /q /f *.o
echo Cleanup COMPLETE
pause>NUL