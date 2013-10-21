//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : getsi
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _NTQSI_H_
#define _NTQSI_H_

#ifdef WIN32
#include <windows.h>

#define STATUS_SUCCESS               ((NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH  ((NTSTATUS)0xC0000004L)


#define SystemProcessorPerformanceInformation	8
#define SystemProcessInformation				5


typedef LONG NTSTATUS;

typedef struct _LSA_UNICODE_STRING
{
	USHORT  Length;
	USHORT  MaximumLength;
	PWSTR   Buffer;
}LSA_UNICODE_STRING,*PLSA_UNICODE_STRING;

typedef LSA_UNICODE_STRING UNICODE_STRING, *PUNICODE_STRING;

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
}CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

typedef LONG KPRIORITY;

typedef struct _VM_COUNTERS
{
	ULONG PeakVirtualSize;
	ULONG VirtualSize;
	ULONG PageFaultCount;
	ULONG PeakWorkingSetSize;
	ULONG WorkingSetSize;
	ULONG QuotaPeakPagedPoolUsage;
	ULONG QuotaPagedPoolUsage;
	ULONG QuotaPeakNonPagedPoolUsage;
	ULONG QuotaNonPagedPoolUsage;
	ULONG PagefileUsage;
	ULONG PeakPagefileUsage;
}VM_COUNTERS,*PVM_COUNTERS;

#ifndef  WIN32
typedef struct _IO_COUNTERS
{
	LARGE_INTEGER ReadOperationCount;
	LARGE_INTEGER WriteOperationCount;
	LARGE_INTEGER OtherOperationCount;
	LARGE_INTEGER ReadTransferCount;
	LARGE_INTEGER WriteTransferCount;
	LARGE_INTEGER OtherTransferCount;
}IO_COUNTERS,*PIO_COUNTERS;
#endif

typedef enum _THREAD_STATE
{
	StateInitialized,
	StateReady,
	StateRunning,
	StateStandby,
	StateTerminated,
	StateWait,
	StateTransition,
	StateUnknown
}THREAD_STATE;

typedef enum _KWAIT_REASON
{
	Executive,
	FreePage,
	PageIn,
	PoolAllocation,
	DelayExecution,
	Suspended,
	UserRequest,
	WrExecutive,
	WrFreePage,
	WrPageIn,
	WrPoolAllocation,
	WrDelayExecution,
	WrSuspended,
	WrUserRequest,
	WrEventPair,
	WrQueue,
	WrLpcReceive,
	WrLpcReply,
	WrVertualMemory,
	WrPageOut,
	WrRendezvous,
	Spare2,
	Spare3,
	Spare4,
	Spare5,
	Spare6,
	WrKernel
}KWAIT_REASON;

typedef struct _SYSTEM_THREADS
{
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG         WaitTime;
	PVOID         StartAddress;
	CLIENT_ID     ClientId;
	KPRIORITY     Priority;
	KPRIORITY     BasePriority;
	ULONG         ContextSwitchCount;
	THREAD_STATE  State;
	KWAIT_REASON  WaitReason;
}SYSTEM_THREADS,*PSYSTEM_THREADS;

typedef struct _SYSTEM_PROCESSES
{
	ULONG          NextEntryDelta;
	ULONG          ThreadCount;
	ULONG          Reserved1[6];
	LARGE_INTEGER  CreateTime;
	LARGE_INTEGER  UserTime;
	LARGE_INTEGER  KernelTime;
	UNICODE_STRING ProcessName;
	KPRIORITY      BasePriority;
	ULONG          ProcessId;
	ULONG          InheritedFromProcessId;
	ULONG          HandleCount;
	ULONG          Reserved2[2];
	VM_COUNTERS    VmCounters;
	IO_COUNTERS    IoCounters;
	SYSTEM_THREADS Threads[1];
}SYSTEM_PROCESSES,*PSYSTEM_PROCESSES;

typedef struct
_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
	LARGE_INTEGER IdleTime;
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER Reserved1[2];
	ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;


typedef ULONG (__stdcall * NtQuerySystemInformation)(
		ULONG SystemInformationClass,
		PVOID SystemInformation,
		ULONG SystemInformationLength,
		PULONG ReturnLength);

#endif

#endif
