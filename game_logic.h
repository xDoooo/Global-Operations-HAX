#pragma once

#include<Windows.h>
#include<iostream>
#include<string>
#include<vector>
#include<psapi.h>
#include<atomic>
#include<thread>
#include<shlwapi.h>

namespace game_logic {

	//Global Operations Process ID nad Window
    inline HANDLE Global_Operations_Process = 0;
    inline DWORD Global_Operations_id = 0;
    inline HWND Global_Operations_Window = 0;

    //Dlls
    inline DWORD cshell_dll;

    //StopThread values
    inline std::atomic<bool> stopThread0(true);

    //read the memory once each time u open new game room
    inline bool read_moneys_memory_once{ true };

	//we need this to restort old money values before active it the cheat
    inline bool save_old_money{ true };

	//old values
    inline DWORD old_money0{ 0 }, old_money1{ 0 };

    inline DWORD money0 = 0;
    inline DWORD money1 = 0;

    //some check values
    inline DWORD iCheck0;

    //F1
    inline bool F1_On_off{ false };
    inline bool F1_current_state{ false };
    inline bool F1_last_state{ false };

    //F2
    inline bool F2_On_off{ false };
    inline bool F2_current_state{ false };
    inline bool F2_last_state{ false };

	bool ReadFromMemory(LPCVOID read_from, DWORD* read_to);

	bool WriteToMemory(LPVOID write_to, DWORD* write_from);

	DWORD GetModuleBaseAdress(HANDLE the_process, const std::wstring& module_name);

    void GetTheGameInfo();

    void ReadMoneysMemory();

    void UnlimitedMoney();

    void TheCheat();
}
