// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"

#include "stdlib.h"
#include "stdio.h"
//#include "hsp3struct.h"

#pragma warning(disable : 4996)

#define MAX_HSPVM 16

typedef BOOL(CALLBACK* HSP3FUNC)(int, int, int, int);
typedef BOOL(CALLBACK* HSP3FUNC2)(int, int, int, void*);
HINSTANCE h_hspimp[MAX_HSPVM];
HSP3FUNC hspini[MAX_HSPVM];
HSP3FUNC hspbye[MAX_HSPVM];
HSP3FUNC hspexec[MAX_HSPVM];
HSP3FUNC2 hspprm[MAX_HSPVM];

typedef struct HSPThreadctx {
    UINT32 argini[4];
    UINT32 argprm[512][4];
};

bool ishspvmused[MAX_HSPVM];
char fname4hsp3imp[256] = "hsp3imp.dll";

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        for (int cnt = 0; cnt < MAX_HSPVM; cnt++) {
            h_hspimp[cnt] = LoadLibraryA(fname4hsp3imp);
            if (h_hspimp[cnt] != NULL) {
                hspini[cnt] = (HSP3FUNC)GetProcAddress(h_hspimp[cnt], "_hspini@16");
                hspbye[cnt] = (HSP3FUNC)GetProcAddress(h_hspimp[cnt], "_hspbye@16");
                hspexec[cnt] = (HSP3FUNC)GetProcAddress(h_hspimp[cnt], "_hspexec@16");
                hspprm[cnt] = (HSP3FUNC2)GetProcAddress(h_hspimp[cnt], "_hspprm@16");
            }
            sprintf(fname4hsp3imp, "hsp3imp%d.dll", (cnt + 1));
        }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void execthread(HSPThreadctx* prm_0) { int tmpvmid = -1; for (int cnt = 0; cnt < MAX_HSPVM; cnt++) { if (ishspvmused[cnt] == false) { tmpvmid = cnt; break; } } if (tmpvmid == -1) { return; } ishspvmused[tmpvmid] = true; for (int cnt = 0; cnt < 512; cnt++) { if (prm_0->argprm[cnt][0] != 0) { hspprm[tmpvmid](cnt, prm_0->argprm[cnt][1], prm_0->argprm[cnt][2], (void*)(prm_0->argprm[cnt][3])); } } hspini[tmpvmid](prm_0->argini[0], prm_0->argini[1], prm_0->argini[2], prm_0->argini[3]); hspexec[tmpvmid](0, 0, 0, 0); hspbye[tmpvmid](0, 0, 0, 0); ishspvmused[tmpvmid] = false; }

extern "C" __declspec(dllexport) void CreateHSPThread(HSPThreadctx * prm_0) { CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)execthread,(LPVOID)prm_0,0,0); }
extern "C" __declspec(dllexport) HSPThreadctx * AllocateHSPThreadCTX() { return (HSPThreadctx*)malloc(sizeof(HSPThreadctx)); }
