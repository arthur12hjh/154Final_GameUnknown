// 사용할 명령어   사용할 옵션			원본 파일이 있는 위치			사본을 저장할 위치

xcopy			/y/s	.\Engine\Public\*.*							.\EngineSDK\Inc\

xcopy			/y		.\Engine\Bin\Debug\Engine.dll					.\Effect_Editor\Bin\Debug\
xcopy			/y		.\Engine\Bin\Debug\Engine.lib					.\EngineSDK\Lib\Debug\

xcopy			/y		.\Engine\Bin\Release\Engine.dll					.\Effect_Editor\Bin\Release\
xcopy			/y		.\Engine\Bin\Release\Engine.lib					.\EngineSDK\Lib\Release\

xcopy			/y		.\Engine\ThirdPartyLib\Debug\*.lib				.\EngineSDK\Lib\Debug\
xcopy			/y		.\Engine\ThirdPartyLib\Release\*.lib			.\EngineSDK\Lib\Release\

xcopy			/y		.\Engine\Bin\ShaderFiles\*.*				.\Effect_Editor\Bin\ShaderFiles\
xcopy			/y/s		.\Engine\Bin\EngineResource\*.*					.\Effect_Editor\Bin\EngineResource\