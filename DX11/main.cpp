/*
- WinMain
    - SystemClass
        - ApplicationClass: 렌더링
            - D3DClass: D3D 기반 하드웨어 (모니터, GPU, ...) 조작
            - ColorShaderClass: Shader 관련 기능 담당
        - InputClass: 입력 대응
*/

#include "main.h"
#include "systemclass.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    SystemClass* System;
    bool result;

    System = new SystemClass;
    result = System->Initialize();
    if (result)
    {
        System->Run();
    }

    System->Shutdown();
    delete System;
    System = 0;
    return 0;
}