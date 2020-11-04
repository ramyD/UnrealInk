rem Build the plugin
set UE4DIR="C:\Program Files\Epic Games\UE_4.25"
call %UE4DIR%\Engine\Build\BatchFiles\RunUAT.bat BuildPlugin -Plugin="%CD%\UnrealInk.uplugin" -Package="%CD%\Build\UnrealInk" -Rocket

robocopy "%CD%\ThirdParty" "%CD%\Build\UnrealInk\ThirdParty" /E
rem rd /s /q "./Build/UnrealInk/Intermediate"

robocopy "%CD%\Build\UnrealInk" %UE4DIR%\Engine\Plugins\ /E
pause
