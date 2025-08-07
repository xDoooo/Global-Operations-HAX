#include "game_logic.h"

#include<Windows.h>
#include<iostream>
#include<string>
#include<vector>
#include<psapi.h>
#include<atomic>
#include<thread>
//#include<shlwapi.h>

bool game_logic::ReadFromMemory(LPCVOID read_from, DWORD* read_to) {

    return  ReadProcessMemory(Global_Operations_Process, read_from, read_to, sizeof(DWORD), NULL);
}

bool game_logic::WriteToMemory(LPVOID write_to, DWORD* write_from) {

    return WriteProcessMemory(Global_Operations_Process, write_to, write_from, sizeof(DWORD), NULL);
}

DWORD game_logic::GetModuleBaseAdress(HANDLE the_process, const std::wstring& module_name) {

    //count of bytes
    DWORD cb{ };

    //get the number of bytes required to store all module handles 
    EnumProcessModules(the_process, NULL, 0, &cb);

    //how many modules in process
    DWORD modules_count{ cb / sizeof(HMODULE) };

    std::vector<HMODULE> hModule(modules_count);//use dynamic allocation memory here coz its flexible size (we dont know the numbers of modules

    if (EnumProcessModules(the_process, hModule.data(), modules_count * sizeof(HMODULE), &cb)) {

        for (unsigned int i = 0; i < modules_count; i++) {

            TCHAR module_names[MAX_PATH];

            if (GetModuleFileNameEx(the_process, hModule[i], module_names, sizeof(module_names) / sizeof(TCHAR))) {

                //get file name only 
                std::wstring file_name{ PathFindFileName(module_names) };

                if (module_name == file_name) {

                    return (DWORD)hModule[i];
                }
            }
        }
    }

    // If the module was not found, print an error
    std::cerr << "Error: Module not found,Connect with the owner." << std::endl;
    return 0;

}

void game_logic::GetTheGameInfo() {

    // Get the process ID
    GetWindowThreadProcessId(Global_Operations_Window, &Global_Operations_id);
    //Open the process with all access rights
    Global_Operations_Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Global_Operations_id);
    //get base adress of DLLs
    cshell_dll = GetModuleBaseAdress(Global_Operations_Process, L"cshell.dll");
    //get iCheck0 value
    ReadFromMemory((LPCVOID)(cshell_dll + 0x109270), &iCheck0);

}

void game_logic::ReadMoneysMemory() {

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));//wait until everything is fully loaded

    // Read memory to get initial values
    ReadFromMemory((LPCVOID)0x536B90, &money0);
    ReadFromMemory((LPCVOID)0x536B90, &money1);

    // Start applying offsets from this point
    money0 += 0xEE4;
    money1 += 0x12CC;
    ReadFromMemory((LPCVOID)money0, &money0);
    ReadFromMemory((LPCVOID)money1, &money1);

    money0 += 0x181C4;
    money1 += 0x40;
    ReadFromMemory((LPCVOID)money0, &money0);
    ReadFromMemory((LPCVOID)money1, &money1);

    money0 += 0xB8;
    money1 += 0x610;
    ReadFromMemory((LPCVOID)money1, &money1);

    money1 += 0x3AC;
    ReadFromMemory((LPCVOID)money1, &money1);

    money1 += 0x12C;

    read_moneys_memory_once = false;

}

void game_logic::UnlimitedMoney() {

    //temporary values
    DWORD unlimited_money0{ 999999 }, unlimited_money1{ 999999 };

    //save old values of moneys
    if (save_old_money) {

        ReadFromMemory((LPCVOID)money0, &old_money0);
        ReadFromMemory((LPCVOID)money1, &old_money1);

        save_old_money = false;
    }

    // Write the updated values back to memory
    WriteToMemory((LPVOID)money0, &unlimited_money0);
    WriteToMemory((LPVOID)money1, &unlimited_money1);
}

void game_logic::TheCheat() {

    while (stopThread0) {

        // Find the game window (and check if game is running or not )
        if ((Global_Operations_Window = FindWindow(NULL, L"Global Operations")) != NULL) {

            GetTheGameInfo();

            //turn off All cheats if user are not inside game room, and read moneys memory again(coz pointer change each time load inside new game room)
            if (iCheck0 == 0) {

                F1_On_off = false;
                F1_current_state = false;
                F1_last_state = false;

                read_moneys_memory_once = true;
                save_old_money = true;
            }

            //check if we are inside a game room
            if (iCheck0 > 0) {

                //Read moneys memory one time
                if (read_moneys_memory_once) {
                    ReadMoneysMemory();
                }

                //Chose a Cheat
                //F1
                F1_current_state = GetAsyncKeyState(VK_F1) & 0x8000;
                if (F1_current_state && !F1_last_state) F1_On_off = !F1_On_off;
                //F2


            // F1
                {
                    //this is what heppened when u turn on F1
                    if (F1_On_off) UnlimitedMoney();

                    //this is what heppened when u turn off F1
                    if (!F1_current_state && F1_last_state) {

                        save_old_money = true;//if off F1 cheat , we need to save old money values again (so set this to True)

                        //set money to the old values
                        WriteToMemory((LPVOID)money0, &old_money0);
                        WriteToMemory((LPVOID)money1, &old_money1);
                    }
                }

                // Update the cheats HotKeys last state for next iteration (so we know if it was flase or true)
                F1_last_state = F1_current_state;

            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
