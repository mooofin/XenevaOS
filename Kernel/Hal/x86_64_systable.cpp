/**
* BSD 2-Clause License
*
* Copyright (c) 2022-2023, Manas Kamal Choudhury
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
**/

#include <stdint.h>
#include <_null.h>
#include <aucon.h>
#include <Hal/x86_64_sched.h>
#include <Hal/x86_64_hal.h>
#include <loader.h>
#include <Mm/vmmngr.h>
#include <process.h>
#include <Hal/serial.h>
#include <Sync/mutex.h>
#include <Hal/x86_64_signal.h>
#include <Serv/sysserv.h>
#include <ftmngr.h>
#include <Fs/tty.h>
#include <Fs/pipe.h>
#include <Mm/mmap.h>
#include <net/socket.h>
#include <Fs/vdisk.h>

/* Syscall function format */
typedef int64_t(*syscall_func) (int64_t param1, int64_t param2, int64_t param3, int64_t
	param4, int64_t param5, int64_t param6);


/*
 * KePringMsg -- this system call uses kernel console output
 * to pring msg directly from process
 */
uint64_t KePrintMsg(int64_t param1, int64_t param2, int64_t param3, int64_t
	param4, int64_t param5, int64_t param6) {
	char* text = (char*)param1;
	AuTextOut("%s\n",text);
	return 0;
}

/*
 * null_call -- 0th call is null call
 */
uint64_t null_call(int64_t param1, int64_t param2, int64_t param3, int64_t
	param4, int64_t param5, int64_t param6) {
	return 0;
}

/* syscall entries */
static void* syscalls[AURORA_MAX_SYSCALL] = {
	(void*)null_call,        //0
	(void*)SeTextOut,       //1
	(void*)PauseThread,      //2
	(void*)GetThreadID,      //3
	(void*)GetProcessID,     //4
	(void*)ProcessExit,      //5
	(void*)ProcessWaitForTermination, //6
	(void*)CreateProcess,    //7
	(void*)ProcessLoadExec,  //8
	(void*)CreateSharedMem,  //9
	(void*)ObtainSharedMem,  //10
	(void*)UnmapSharedMem,   //11
	(void*)OpenFile,         //12
	(void*)CreateMemMapping, //13
	(void*)UnmapMemMapping,  //14
	(void*)GetProcessHeapMem, //15
	(void*)ReadFile,         //16
	(void*)WriteFile,        //17
	(void*)CreateDir,        //18
	(void*)RemoveFile,       //19
	(void*)CloseFile,        //20
	(void*)FileIoControl,    //21
	(void*)FileStat,         //22
	(void*)ProcessSleep,     //23
	(void*)SignalReturn,     //24
	(void*)SetSignal,        //25
	(void*)GetSystemTimerTick, //26
	(void*)AuFTMngrGetFontID, //27
	(void*)AuFTMngrGetNumFonts, //28
	(void*)AuFTMngrGetFontSize, //29
	(void*)MemMapDirty, //30
	(void*)AuTTYCreate, //31
	(void*)CreateUserThread, //32
	(void*)SetFileToProcess, //33
	(void*)ProcessHeapUnmap, //34
	(void*)SendSignal, //35
	(void*)GetCurrentTime, //36
	(void*)OpenDir, //37
	(void*)ReadDir, //38
	(void*)CreateTimer, //39
	(void*)StartTimer, //40
	(void*)StopTimer,  //41
	(void*)DestroyTimer, //42
	(void*)ProcessGetFileDesc, //43
	(void*)FileSetOffset, //44
	(void*)GetTimeOfDay, //45
	(void*)AuCreateSocket, //46
	(void*)NetConnect, //47
	(void*)NetSend, //48
	(void*)NetReceive, //49
	(void*)AuSocketSetOpt, //50
	(void*)NetBind, //51
	(void*)NetAccept, //52
	(void*)NetListen, //53
	(void*)AuCreatePipe, //54
	(void*)AuGetVDiskInfo, //55
	(void*)AuGetVDiskPartitionInfo, //56
	(void*)GetEnvironmenBlock, //57
};

//! System Call Handler Functions
//! @param a -- arg1 passed in r12 register
//! @param b -- arg2 passed in r13 register
//! @param c -- arg3 passed in r14 register
//! @param d -- arg4 passed in r15 register
extern "C" int64_t x64_syscall_handler(int a) {
	x64_cli();
	AuThread* current_thr = AuGetCurrentThread();
	uint64_t ret_code = 0;

	if (a < 0 || a >= AURORA_MAX_SYSCALL)
		return -1;
	
	syscall_func func = (syscall_func)syscalls[a];
	if (!func)
		return 0;

	ret_code = func(current_thr->syscall_param.param1, current_thr->syscall_param.param2, current_thr->syscall_param.param3,
			current_thr->syscall_param.param4, current_thr->syscall_param.param5, current_thr->syscall_param.param6);

	return ret_code;
}
