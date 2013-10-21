//////////////////////////////////////////////////////////////////////////
//
//		I don't quite remember whose .h it is.
//		Maybe zzzEVAzzz's wrote this code.
//		If not, it's mine :)
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RING0_H_
#define _RING0_H_


///////////////////////
// Include and Macro //
///////////////////////

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Ntsecapi.h>
#include <Aclapi.h>

#pragma comment (lib,"ntdll.lib")    // Win DDK
#pragma comment (lib,"Kernel32.lib")
#pragma comment (lib,"Advapi32.lib")

typedef enum _SECTION_INHERIT {
	ViewShare = 1,
	ViewUnmap = 2,
} SECTION_INHERIT;

typedef struct _OBJECT_ATTRIBUTES {
	DWORD Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	DWORD Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _SYSTEM_MODULE_INFORMATION {
	DWORD Reserved[2];
	PVOID Base;
	DWORD Size;
	DWORD Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define STATUS_SUCCESS              0x00000000
#define STATUS_UNSUCCESSFUL         0xC0000001
#define STATUS_NOT_IMPLEMENTED      0xC0000002
#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004
#define STATUS_INVALID_PARAMETER    0xC000000D
#define STATUS_ACCESS_DENIED        0xC0000022
#define STATUS_BUFFER_TOO_SMALL     0xC0000023
#define OBJ_KERNEL_HANDLE           0x00000200
#define SystemModuleInformation     11

#define InitializeObjectAttributes( p, n, a, r, s ) { \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES );        \
	(p)->RootDirectory = r;                           \
	(p)->Attributes = a;                              \
	(p)->ObjectName = n;                              \
	(p)->SecurityDescriptor = s;                      \
	(p)->SecurityQualityOfService = NULL;             \
}



////////////////
// Native API //
////////////////

#ifdef __cplusplus
extern "C"{
#endif
	NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation( DWORD,
		PVOID,
		DWORD,
		PDWORD );

	NTSYSAPI NTSTATUS NTAPI ZwMapViewOfSection(   IN HANDLE SectionHandle,
		IN HANDLE ProcessHandle,
		IN OUT PVOID *BaseAddress,
		IN DWORD ZeroBits,
		IN DWORD CommitSize,
		IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
		IN OUT PDWORD ViewSize,
		IN SECTION_INHERIT InheritDisposition,
		IN DWORD AllocationType,
		IN DWORD Protect );

	NTSYSAPI NTSTATUS NTAPI ZwUnmapViewOfSection( IN HANDLE ProcessHandle, 
		IN PVOID BaseAddress );

	NTSYSAPI VOID NTAPI		RtlInitUnicodeString( PUNICODE_STRING DestinationString,
		PCWSTR SourceString );

	NTSYSAPI NTSTATUS NTAPI	ZwOpenSection( OUT PHANDLE SectionHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes );

	NTSYSAPI NTSTATUS NTAPI	ZwClose( IN HANDLE Handle );

	NTSYSAPI NTSTATUS NTAPI	NtVdmControl( IN DWORD ControlCode,
		IN PVOID ControlData );



	/////////
	// pfn //
	/////////

	typedef NTSTATUS (NTAPI *PFNNTVDMCONTROL)(	IN DWORD ControlCode,
		IN PVOID ControlData );

	typedef LARGE_INTEGER (NTAPI* PFNMMGETPHYSICALADDRESS)( void *BaseAddress );

	typedef BOOL (WINAPI *PCREATEPROCESSINTERNALW)( DWORD reserved1,
		LPCWSTR lpApplicationName,
		LPWSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCWSTR lpCurrentDirectory,
		LPSTARTUPINFOW lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation,
		DWORD reserved2 );
#ifdef __cplusplus
}
#endif



////////////////////////////////////
// Function needed to enter ring0 //
////////////////////////////////////

LARGE_INTEGER	MyMmGetPhysicalAddress(PVOID);
NTSTATUS		Ring0Code(DWORD, DWORD);
PVOID			GetModuleBase(PCSTR);
HANDLE			OpenPhysicalMemory();
PVOID			MapPhysicalMemory(HANDLE, DWORD, DWORD);


#endif // _RING0_H_
