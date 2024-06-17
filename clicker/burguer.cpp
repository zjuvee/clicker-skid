

/*

the antisandbox/antidebugging part was fully coded by tosted

it is a pretty bad bypass, if you want you can improve it or adapt it to your needs

*/

#include <fileapi.h>
#include <pch.hpp>
#include <WinBase.h>
#include <stdlib.h>
#include "xorstr.hpp"
#include <TlHelp32.h>
#include <vmaware.hpp>
#include <winioctl.h>
#include <winternl.h>
#include "burguer.hpp"

// check if blacklisted process is running
bool SecurityChecks::isProcessRunning(const std::wstring& processName) {
#if (MSVC)
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return false;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return false;
    }

    do {
#ifdef UNICODE
        if (std::wstring(pe32.szExeFile) == processName) {
#else
        if (std::string(pe32.szExeFile) == std::string(processName.begin(), processName.end())) {
#endif
            CloseHandle(hProcessSnap);
            return true;
        }
        } while (Process32Next(hProcessSnap, &pe32));

        CloseHandle(hProcessSnap);
        return false;
#else
    return false;
#endif
    }

// close the process
void SecurityChecks::terminateProcessByName(const std::wstring & processName) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return;
    }

    do {
#ifdef UNICODE
        if (std::wstring(pe32.szExeFile) == processName) {
#else
        if (std::string(pe32.szExeFile) == std::string(processName.begin(), processName.end())) {
#endif
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (hProcess != NULL) {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }
        } while (Process32Next(hProcessSnap, &pe32));

        CloseHandle(hProcessSnap);
    }


// check if the debugger is running
bool SecurityChecks::isDebuggerPresent() {
#if (MSVC)
    return IsDebuggerPresent();
#else
    return false;
#endif
}

// check if the remote debugger is running
bool SecurityChecks::checkRemoteDebuggerPresent() {
#if (MSVC)
    BOOL isDebuggerPresent = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebuggerPresent);
    return isDebuggerPresent == TRUE;
#else
    return false;
#endif
}

// check if the system is runniung in triage
bool SecurityChecks::isUSBConnected() {
    DWORD drives = GetLogicalDrives();
    for (int i = 4; i < 26; ++i) {
        if (drives & (1 << i)) {
            wchar_t driveLetter[] = { L'A' + i, L':', L'\\', L'\0' };
            if (GetDriveType(driveLetter) == DRIVE_REMOVABLE) {
                return true;
            }
        }
    }
    return false;
}

// check if the system is runniung in triage
bool SecurityChecks::isDriveConnectedToF() {
    DWORD drives = GetLogicalDrives();
    for (int i = 0; i < 26; ++i) {
        if (drives & (1 << i)) {
            wchar_t driveLetter[] = { L'A' + i, L':', L'\\', L'\0' };
            if (GetDriveType(driveLetter) == DRIVE_FIXED) {
                if (driveLetter[0] == 'F') {
                    return true;
                }
            }
        }
    }
    return false;
}

// checks system registry keys
bool SecurityChecks::registryKeyExists(const std::wstring& keyName) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyName.c_str(), 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }
    return false;
}

// check if the CPUID string matches
bool SecurityChecks::checkCPUIDStrings() {
    std::string cpuidString;

#if defined(_MSC_VER)
    int info[4];
    __cpuid(info, 0x40000000);
    cpuidString += std::string(reinterpret_cast<const char*>(&info[1]), 4);
    cpuidString += std::string(reinterpret_cast<const char*>(&info[3]), 4);
    cpuidString += std::string(reinterpret_cast<const char*>(&info[2]), 4);
#elif defined(__GNUC__)
    unsigned int eax, ebx, ecx, edx;
    __get_cpuid(0x40000000, &eax, &ebx, &ecx, &edx);
    cpuidString += std::string(reinterpret_cast<const char*>(&ebx), 4);
    cpuidString += std::string(reinterpret_cast<const char*>(&edx), 4);
    cpuidString += std::string(reinterpret_cast<const char*>(&ecx), 4);
#endif


    if (cpuidString.find(xorstr_("Microsoft Hv")) != std::string::npos ||
        cpuidString.find(xorstr_("KVMKVMKVM")) != std::string::npos ||
        cpuidString.find(xorstr_("prl hyperv")) != std::string::npos ||
        cpuidString.find(xorstr_("VBoxVBoxVBox")) != std::string::npos ||
        cpuidString.find(xorstr_("VMwareVMware")) != std::string::npos ||
        cpuidString.find(xorstr_("XenVMMXenVMM")) != std::string::npos) {

        return true;
    }


    return false;
}

// check virtual disk names
bool SecurityChecks::isVirtualDiskName(const std::wstring& diskName) {

    if (diskName.find(xorstr_(L"QEMU")) != std::wstring::npos ||
        diskName.find(xorstr_(L"VBOX")) != std::wstring::npos ||
        diskName.find(xorstr_(L"VIRTUAL HD")) != std::wstring::npos ||
        diskName.find(xorstr_(L"VMWare")) != std::wstring::npos) {
        return true;
    }

    return false;
}

// fast execution detection, hybrid analisis, virustotal and tria.ge techniques
bool SecurityChecks::detectFastExecution() {
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return duration < 1000;
}

// enum more vm process
BOOL CALLBACK EnumWindowsProc(HWND hWindow, LPARAM parameter)
{
    WCHAR windowTitle[1024];
    GetWindowTextW(hWindow, windowTitle, sizeof(windowTitle));
    CharUpperW(windowTitle);
    if (wcsstr(windowTitle, xorstr_(L"SYSINTERNALS"))) *(PBOOL)parameter = true;
    return true;
}

bool SecurityChecks::ntQueryInformationProcess() {
#if (MSVC)
    typedef NTSTATUS(WINAPI* NtQueryInformationProcess)(HANDLE, UINT, PVOID, ULONG, PULONG);
    NtQueryInformationProcess ntQueryInfo = (NtQueryInformationProcess)GetProcAddress(GetModuleHandleA(xorstr_("ntdll.dll")), xorstr_("NtQueryInformationProcess"));

    if (ntQueryInfo) {
        DWORD isBeingDebugged = 0;
        ntQueryInfo(GetCurrentProcess(), 7, &isBeingDebugged, sizeof(DWORD), NULL);
        return isBeingDebugged != 0;
    }
    return false;
#else
    return false;
#endif
}

// enumarate number of processors
bool SecurityChecks::detectEmulation() {
#if (MSVC)
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    return sysInfo.dwNumberOfProcessors < 2;
#else
    return false;
#endif
}

// check if the MORE sandbox process are running
bool SecurityChecks::checkSandboxProcesses() {
    std::vector<std::wstring> sandboxProcesses = {
        (xorstr_(L"vboxservice.exe")),
        (xorstr_(L"vmsrvc.exe"))
    };

    for (const auto& process : sandboxProcesses) {
        if (isProcessRunning(process)) {
            return true;
        }
    }
    return false;
}

// check blacklisted mac adresses, you can add your own
bool SecurityChecks::checkMacAddress() {
    const std::string macPrefixes[] = {
            xorstr_("05:17:5D:75:D5:54"),
            xorstr_("00:03:47:63:8b:de"),
            xorstr_("00:0c:29:05:d8:6e"),
            xorstr_("00:0c:29:2c:c1:21"),
            xorstr_("00:0c:29:52:52:50"),
            xorstr_("00:0d:3a:d2:4f:1f"),
            xorstr_("00:15:5d:00:00:1d"),
            xorstr_("00:15:5d:00:00:a4"),
            xorstr_("00:15:5d:00:00:b3"),
            xorstr_("00:15:5d:00:00:c3"),
            xorstr_("00:15:5d:00:00:f3"),
            xorstr_("00:15:5d:00:01:81"),
            xorstr_("00:15:5d:00:02:26"),
            xorstr_("00:15:5d:00:05:8d"),
            xorstr_("00:15:5d:00:05:d5"),
            xorstr_("00:15:5d:00:06:43"),
            xorstr_("00:15:5d:00:07:34"),
            xorstr_("00:15:5d:00:1a:b9"),
            xorstr_("00:15:5d:00:1c:9a"),
            xorstr_("00:15:5d:13:66:ca"),
            xorstr_("00:15:5d:13:6d:0c"),
            xorstr_("00:15:5d:1e:01:c8"),
            xorstr_("00:15:5d:23:4c:a3"),
            xorstr_("00:15:5d:23:4c:ad"),
            xorstr_("00:15:5d:b6:e0:cc"),
            xorstr_("00:1b:21:13:15:20"),
            xorstr_("00:1b:21:13:21:26"),
            xorstr_("00:1b:21:13:26:44"),
            xorstr_("00:1b:21:13:32:20"),
            xorstr_("00:1b:21:13:32:51"),
            xorstr_("00:1b:21:13:33:55"),
            xorstr_("00:23:cd:ff:94:f0"),
            xorstr_("00:25:90:36:65:0c"),
            xorstr_("00:25:90:36:65:38"),
            xorstr_("00:25:90:36:f0:3b"),
            xorstr_("00:25:90:65:39:e4"),
            xorstr_("00:50:56:97:a1:f8"),
            xorstr_("00:50:56:97:ec:f2"),
            xorstr_("00:50:56:97:f6:c8"),
            xorstr_("00:50:56:a0:06:8d"),
            xorstr_("00:50:56:a0:38:06"),
            xorstr_("00:50:56:a0:39:18"),
            xorstr_("00:50:56:a0:45:03"),
            xorstr_("00:50:56:a0:59:10"),
            xorstr_("00:50:56:a0:61:aa"),
            xorstr_("00:50:56:a0:6d:86"),
            xorstr_("00:50:56:a0:84:88"),
            xorstr_("00:50:56:a0:af:75"),
            xorstr_("00:50:56:a0:cd:a8"),
            xorstr_("00:50:56:a0:d0:fa"),
            xorstr_("00:50:56:a0:d7:38"),
            xorstr_("00:50:56:a0:dd:00"),
            xorstr_("00:50:56:ae:5d:ea"),
            xorstr_("00:50:56:ae:6f:54"),
            xorstr_("00:50:56:ae:b2:b0"),
            xorstr_("00:50:56:ae:e5:d5"),
            xorstr_("00:50:56:b3:05:b4"),
            xorstr_("00:50:56:b3:09:9e"),
            xorstr_("00:50:56:b3:14:59"),
            xorstr_("00:50:56:b3:21:29"),
            xorstr_("00:50:56:b3:38:68"),
            xorstr_("00:50:56:b3:38:88"),
            xorstr_("00:50:56:b3:3b:a6"),
            xorstr_("00:50:56:b3:42:33"),
            xorstr_("00:50:56:b3:4c:bf"),
            xorstr_("00:50:56:b3:50:de"),
            xorstr_("00:50:56:b3:91:c8"),
            xorstr_("00:50:56:b3:94:cb"),
            xorstr_("00:50:56:b3:9e:9e"),
            xorstr_("00:50:56:b3:a9:36"),
            xorstr_("00:50:56:b3:d0:a7"),
            xorstr_("00:50:56:b3:dd:03"),
            xorstr_("00:50:56:b3:ea:ee"),
            xorstr_("00:50:56:b3:ee:e1"),
            xorstr_("00:50:56:b3:f6:57"),
            xorstr_("00:50:56:b3:fa:23"),
            xorstr_("00:e0:4c:42:c7:cb"),
            xorstr_("00:e0:4c:44:76:54"),
            xorstr_("00:e0:4c:46:cf:01"),
            xorstr_("00:e0:4c:4b:4a:40"),
            xorstr_("00:e0:4c:56:42:97"),
            xorstr_("00:e0:4c:7b:7b:86"),
            xorstr_("00:e0:4c:94:1f:20"),
            xorstr_("00:e0:4c:b3:5a:2a"),
            xorstr_("00:e0:4c:b8:7a:58"),
            xorstr_("00:e0:4c:cb:62:08"),
            xorstr_("00:e0:4c:d6:86:77"),
            xorstr_("06:75:91:59:3e:02"),
            xorstr_("08:00:27:3a:28:73"),
            xorstr_("08:00:27:45:13:10"),
            xorstr_("12:1b:9e:3c:a6:2c"),
            xorstr_("12:8a:5c:2a:65:d1"),
            xorstr_("12:f8:87:ab:13:ec"),
            xorstr_("16:ef:22:04:af:76"),
            xorstr_("1a:6c:62:60:3b:f4"),
            xorstr_("1c:99:57:1c:ad:e4"),
            xorstr_("1e:6c:34:93:68:64"),
            xorstr_("2e:62:e8:47:14:49"),
            xorstr_("2e:b8:24:4d:f7:de"),
            xorstr_("32:11:4d:d0:4a:9e"),
            xorstr_("3c:ec:ef:43:fe:de"),
            xorstr_("3c:ec:ef:44:00:d0"),
            xorstr_("3c:ec:ef:44:01:0c"),
            xorstr_("3c:ec:ef:44:01:aa"),
            xorstr_("3e:1c:a1:40:b7:5f"),
            xorstr_("3e:53:81:b7:01:13"),
            xorstr_("3e:c1:fd:f1:bf:71"),
            xorstr_("42:01:0a:8a:00:22"),
            xorstr_("42:01:0a:8a:00:33"),
            xorstr_("42:01:0a:8e:00:22"),
            xorstr_("42:01:0a:96:00:22"),
            xorstr_("42:01:0a:96:00:33"),
            xorstr_("42:85:07:f4:83:d0"),
            xorstr_("4e:79:c0:d9:af:c3"),
            xorstr_("4e:81:81:8e:22:4e"),
            xorstr_("52:54:00:3b:78:24"),
            xorstr_("52:54:00:8b:a6:08"),
            xorstr_("52:54:00:a0:41:92"),
            xorstr_("52:54:00:ab:de:59"),
            xorstr_("52:54:00:b3:e4:71"),
            xorstr_("56:b0:6f:ca:0a:e7"),
            xorstr_("56:e8:92:2e:76:0d"),
            xorstr_("5a:e2:a6:a4:44:db"),
            xorstr_("5e:86:e4:3d:0d:f6"),
            xorstr_("60:02:92:3d:f1:69"),
            xorstr_("60:02:92:66:10:79"),
            xorstr_("7e:05:a3:62:9c:4d"),
            xorstr_("90:48:9a:9d:d5:24"),
            xorstr_("92:4c:a8:23:fc:2e"),
            xorstr_("94:de:80:de:1a:35"),
            xorstr_("96:2b:e9:43:96:76"),
            xorstr_("a6:24:aa:ae:e6:12"),
            xorstr_("ac:1f:6b:d0:48:fe"),
            xorstr_("ac:1f:6b:d0:49:86"),
            xorstr_("ac:1f:6b:d0:4d:98"),
            xorstr_("ac:1f:6b:d0:4d:e4"),
            xorstr_("b4:2e:99:c3:08:3c"),
            xorstr_("b4:a9:5a:b1:c6:fd"),
            xorstr_("b6:ed:9d:27:f4:fa"),
            xorstr_("be:00:e5:c5:0c:e5"),
            xorstr_("c2:ee:af:fd:29:21"),
            xorstr_("c8:9f:1d:b6:58:e4"),
            xorstr_("ca:4d:4b:ca:18:cc"),
            xorstr_("d4:81:d7:87:05:ab"),
            xorstr_("d4:81:d7:ed:25:54"),
            xorstr_("d6:03:e4:ab:77:8e"),
            xorstr_("ea:02:75:3c:90:9f"),
            xorstr_("ea:f6:f1:a2:33:76"),
            xorstr_("f6:a5:41:31:b2:78")
    };

    IP_ADAPTER_INFO AdapterInfo[16];
    DWORD dwBufLen = sizeof(AdapterInfo);
    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);

    if (dwStatus != ERROR_SUCCESS) {
        return false;
    }

    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
    while (pAdapterInfo) {
        std::stringstream macStream;
        for (int i = 0; i < pAdapterInfo->AddressLength; ++i) {
            macStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(pAdapterInfo->Address[i]);
            if (i != pAdapterInfo->AddressLength - 1) {
                macStream << ":";
            }
        }
        std::string macAddress = macStream.str();

        for (const auto& prefix : macPrefixes) {
            if (macAddress.find(prefix) == 0) {
                return true;
            }
        }

        pAdapterInfo = pAdapterInfo->Next;
    }

    return false;
}

// get disk names, < triage more bypass
std::vector<std::wstring> SecurityChecks::getDiskNames() {
    std::vector<std::wstring> diskNames;
    DWORD bufferSize = MAX_PATH;
    wchar_t buffer[MAX_PATH];

    DWORD drives = GetLogicalDrives();
    for (int i = 0; i < 26; ++i) {
        if (drives & (1 << i)) {
            wchar_t driveLetter[] = { L'F' + i, L':', L'\\', L'\0' };
            if (GetDriveType(driveLetter) == DRIVE_FIXED) {
                if (QueryDosDevice(driveLetter, buffer, bufferSize)) {
                    diskNames.push_back(buffer);
                }
            }
        }
    }

    return diskNames;
}

// execute command with return value
std::string SecurityChecks::executeCommand(const std::string &cmd) {
    std::array<char, 128> buffer;
    std::string result;
    FILE *pipe = _popen(cmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
    } catch (...) {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    return result;
}

// if hwid is matched with the blacklisted hwid list, return true, if return true, the program will terminate
bool SecurityChecks::isHWIDMatched(const std::vector<std::string> &hwidList, const std::string &currentHWID) {
    for (const auto &hwid : hwidList) {
        if (currentHWID.find(hwid) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// get PC username
std::string SecurityChecks::getUsername() {
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameA(username, &username_len)) {
        return std::string(username);
    } else {
        throw std::runtime_error("Failed to get username");
    }
}

// check if the user is matched with the blacklisted user list, if return true, the program will terminate
bool SecurityChecks::isUserMatched(const std::vector<std::string> &userList, const std::string &currentUser) {
    for (const auto &user : userList) {
        if (currentUser == user) {
            return true;
        }
    }
    return false;
}

// main anti debugging function
bool SecurityChecks::burguer() {

    //======== triage poor bypass
    if (SecurityChecks::isUSBConnected()) {

        Sleep(1000);
        exit(1);
    }

    if (SecurityChecks::isDriveConnectedToF()) {
        Sleep(1000);
        exit(1);
    }
    //=======

     // same check
    PPEB pPEB = (PPEB)__readgsqword(0x60);
    if (pPEB->BeingDebugged) return 0;

    // vmaware check
    const bool is_vm = VM::detect(VM::WIN_HYPERV_DEFAULT);
        if (is_vm) {
            exit(1);
        }

    //==================== idk, some copy pasted  code from guidedhacking 
    HANDLE hDevice = CreateFileW(xorstr_(L"\\\\.\\PhysicalDrive0"), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    DISK_GEOMETRY pDiskGeometry;
    DWORD bytesReturned;
    DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &pDiskGeometry, sizeof(pDiskGeometry), &bytesReturned, (LPOVERLAPPED)NULL);
    DWORD diskSizeGB;
    diskSizeGB = pDiskGeometry.Cylinders.QuadPart * (ULONG)pDiskGeometry.TracksPerCylinder * (ULONG)pDiskGeometry.SectorsPerTrack * (ULONG)pDiskGeometry.BytesPerSector / 1024 / 1024 / 1024;
    if (diskSizeGB < 100) return 0;

    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatusEx(&memoryStatus);
    DWORD RAMMB = memoryStatus.ullTotalPhys / 1024 / 1024;
    if (RAMMB < 2048) return 0;

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    DWORD numberOfProcessors = systemInfo.dwNumberOfProcessors;
    if (numberOfProcessors < 2) return 0;
    //====================

    //==================== check registry keys
    std::wstring registryKeys[] = {
        xorstr_(L"SOFTWARE\\Vmware Inc.\\Vmware Tools"),
        xorstr_(L"HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 2\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0\\Identifier"),
        xorstr_(L"SYSTEM\\CurrentControlSet\\Enum\\SCSI\\Disk&Ven_VMware_&Prod_VMware_Virtual_S"),
        xorstr_(L"SYSTEM\\CurrentControlSet\\Control\\CriticalDeviceDatabase\\root#vmwvmcihostdev"),
        xorstr_(L"SYSTEM\\CurrentControlSet\\Control\\VirtualDeviceDrivers")
    };

    for (const auto& key : registryKeys) {
        if (SecurityChecks::registryKeyExists(key)) {
            exit(1);
        }
    }
    //====================

    // vm dll check
    if (std::filesystem::exists(xorstr_(L"C:\\Windows\\System32\\vmGuestLib.dll")) || std::filesystem::exists(xorstr_(L"C:\\Windows\\System32\\vboxmrxnp.dll"))) {
        exit(1);
    }

    // more virtual disks bypass
    std::vector<std::wstring> diskNames = SecurityChecks::getDiskNames();
    for (const auto& diskName : diskNames) {
        std::wcout << diskName << std::endl;
        if (SecurityChecks::isVirtualDiskName(diskName)) {
            exit(1);
        }
    }

    // blacklisted users list
    std::vector<std::string> USERS = {
        xorstr_("Admin" ),
        xorstr_("BEE7370C-8C0C-4"),
        xorstr_("DESKTOP-NAKFFMT"),
        xorstr_("WIN-5E07COS9ALR"),
        xorstr_("B30F0242-1C6A-4"),
        xorstr_("DESKTOP-VRSQLAG"),
        xorstr_("Q9IATRKPRH"),
        xorstr_("XC64ZB"),
        xorstr_("DESKTOP-D019GDM"),
        xorstr_("DESKTOP-WI8CLET"),
        xorstr_("SERVER1"),
        xorstr_("LISA-PC"),
        xorstr_("JOHN-PC"),
        xorstr_("DESKTOP-B0T93D6"),
        xorstr_("DESKTOP-1PYKP29"),
        xorstr_("DESKTOP-1Y2433R"),
        xorstr_("WILEYPC"),
        xorstr_("WORK"),
        xorstr_("6C4E733F-C2D9-4"),
        xorstr_("RALPHS-PC"),
        xorstr_("DESKTOP-WG3MYJS"),
        xorstr_("DESKTOP-7XC6GEZ"),
        xorstr_("DESKTOP-5OV9S0O"),
        xorstr_("QarZhrdBpj"),
        xorstr_("ORELEEPC"),
        xorstr_("ARCHIBALDPC"),
        xorstr_("JULIA-PC"),
        xorstr_("d1bnJkfVlH"),
        xorstr_("WDAGUtilityAccount"),
        xorstr_("Abby"),
        xorstr_("patex"),
        xorstr_("RDhJ0CNFevzX"),
        xorstr_("kEecfMwgj"),
        xorstr_("Frank"),
        xorstr_("8Nl0ColNQ5bq"),
        xorstr_("Lisa"),
        xorstr_("John"),
        xorstr_("george"),
        xorstr_("PxmdUOpVyx"),
        xorstr_("8VizSM"),
        xorstr_("w0fjuOVmCcP5A"),
        xorstr_("lmVwjj9b"),
        xorstr_("PqONjHVwexsS"),
        xorstr_("3u2v9m8"),
        xorstr_("Julia"),
        xorstr_("HEUeRzl"),
        xorstr_("fred"),
        xorstr_("server"),
        xorstr_("BvJChRPnsxn"),
        xorstr_("Harry Johnson"),
        xorstr_("SqgFOf3G"),
        xorstr_("Lucas"),
        xorstr_("mike"),
        xorstr_("PateX"),
        xorstr_("h7dk1xPr"),
        xorstr_("Louise"),
        xorstr_("User01"),
        xorstr_("test"),
        xorstr_("RGzcBUyrznReg"),
        xorstr_("OgJb6GqgK0O"),
        xorstr_("joshuarob")
    };

    // detect if the pc username is blacklisted
    try {
        std::string username = getUsername();

        if (isUserMatched(USERS, username)) {
            return 0; 
        }

    } catch (const std::exception &e) {
        return 1;
    }

    // blacklisted hwids
    std::vector<std::string> HWIDS = {
        xorstr_("7AB5C494-39F5-4941-9163-47F54D6D5016"),
        xorstr_("03DE0294-0480-05DE-1A06-350700080009"),
        xorstr_("11111111-2222-3333-4444-555555555555"),
        xorstr_("6F3CA5EC-BEC9-4A4D-8274-11168F640058"),
        xorstr_("ADEEEE9E-EF0A-6B84-B14B-B83A54AFC548"),
        xorstr_("4C4C4544-0050-3710-8058-CAC04F59344A"),
        xorstr_("00000000-0000-0000-0000-AC1F6BD04972"),
        xorstr_("00000000-0000-0000-0000-000000000000"),
        xorstr_("5BD24D56-789F-8468-7CDC-CAA7222CC121"),
        xorstr_("49434D53-0200-9065-2500-65902500E439"),
        xorstr_("49434D53-0200-9036-2500-36902500F022"),
        xorstr_("777D84B3-88D1-451C-93E4-D235177420A7"),
        xorstr_("49434D53-0200-9036-2500-369025000C65"),
        xorstr_("B1112042-52E8-E25B-3655-6A4F54155DBF"),
        xorstr_("00000000-0000-0000-0000-AC1F6BD048FE"),
        xorstr_("EB16924B-FB6D-4FA1-8666-17B91F62FB37"),
        xorstr_("A15A930C-8251-9645-AF63-E45AD728C20C"),
        xorstr_("67E595EB-54AC-4FF0-B5E3-3DA7C7B547E3"),
        xorstr_("C7D23342-A5D4-68A1-59AC-CF40F735B363"),
        xorstr_("63203342-0EB0-AA1A-4DF5-3FB37DBB0670"),
        xorstr_("44B94D56-65AB-DC02-86A0-98143A7423BF"),
        xorstr_("6608003F-ECE4-494E-B07E-1C4615D1D93C"),
        xorstr_("D9142042-8F51-5EFF-D5F8-EE9AE3D1602A"),
        xorstr_("49434D53-0200-9036-2500-369025003AF0"),
        xorstr_("8B4E8278-525C-7343-B825-280AEBCD3BCB"),
        xorstr_("4D4DDC94-E06C-44F4-95FE-33A1ADA5AC27"),
        xorstr_("79AF5279-16CF-4094-9758-F88A616D81B4"),
        xorstr_("FF577B79-782E-0A4D-8568-B35A9B7EB76B"),
        xorstr_("08C1E400-3C56-11EA-8000-3CECEF43FEDE"),
        xorstr_("6ECEAF72-3548-476C-BD8D-73134A9182C8"),
        xorstr_("49434D53-0200-9036-2500-369025003865"),
        xorstr_("119602E8-92F9-BD4B-8979-DA682276D385"),
        xorstr_("12204D56-28C0-AB03-51B7-44A8B7525250"),
        xorstr_("63FA3342-31C7-4E8E-8089-DAFF6CE5E967"),
        xorstr_("365B4000-3B25-11EA-8000-3CECEF44010C"),
        xorstr_("D8C30328-1B06-4611-8E3C-E433F4F9794E"),
        xorstr_("00000000-0000-0000-0000-50E5493391EF"),
        xorstr_("00000000-0000-0000-0000-AC1F6BD04D98"),
        xorstr_("4CB82042-BA8F-1748-C941-363C391CA7F3"),
        xorstr_("B6464A2B-92C7-4B95-A2D0-E5410081B812"),
        xorstr_("BB233342-2E01-718F-D4A1-E7F69D026428"),
        xorstr_("9921DE3A-5C1A-DF11-9078-563412000026"),
        xorstr_("CC5B3F62-2A04-4D2E-A46C-AA41B7050712"),
        xorstr_("00000000-0000-0000-0000-AC1F6BD04986"),
        xorstr_("C249957A-AA08-4B21-933F-9271BEC63C85"),
        xorstr_("BE784D56-81F5-2C8D-9D4B-5AB56F05D86E"),
        xorstr_("ACA69200-3C4C-11EA-8000-3CECEF4401AA"),
        xorstr_("3F284CA4-8BDF-489B-A273-41B44D668F6D"),
        xorstr_("BB64E044-87BA-C847-BC0A-C797D1A16A50"),
        xorstr_("2E6FB594-9D55-4424-8E74-CE25A25E36B0"),
        xorstr_("42A82042-3F13-512F-5E3D-6BF4FFFD8518"),
        xorstr_("38AB3342-66B0-7175-0B23-F390B3728B78"),
        xorstr_("48941AE9-D52F-11DF-BBDA-503734826431"),
        xorstr_("A7721742-BE24-8A1C-B859-D7F8251A83D3"),
        xorstr_("3F3C58D1-B4F2-4019-B2A2-2A500E96AF2E"),
        xorstr_("D2DC3342-396C-6737-A8F6-0C6673C1DE08"),
        xorstr_("EADD1742-4807-00A0-F92E-CCD933E9D8C1"),
        xorstr_("AF1B2042-4B90-0000-A4E4-632A1C8C7EB1"),
        xorstr_("FE455D1A-BE27-4BA4-96C8-967A6D3A9661"),
        xorstr_("921E2042-70D3-F9F1-8CBD-B398A21F89C6"),
        xorstr_("6AA13342-49AB-DC46-4F28-D7BDDCE6BE32"),
        xorstr_("F68B2042-E3A7-2ADA-ADBC-A6274307A317"),
        xorstr_("07AF2042-392C-229F-8491-455123CC85FB"),
        xorstr_("4EDF3342-E7A2-5776-4AE5-57531F471D56"),
        xorstr_("032E02B4-0499-05C3-0806-3C0700080009"),
        xorstr_("11111111-2222-3333-4444-555555555555")
    };

    
    std::string command = "wmic csproduct get uuid";
    std::string hwid = executeCommand(command);

    // detect if the pc hwid is blacklisted
    if (isHWIDMatched(HWIDS, hwid)) {
        return 0; 
    }

    // call the anti debugging functions
    bool debugged = false;
    EnumWindows(EnumWindowsProc, (LPARAM)(&debugged));
    if (debugged) return 0;

    if (SecurityChecks::isDebuggerPresent() || SecurityChecks::checkRemoteDebuggerPresent() || SecurityChecks::ntQueryInformationProcess()) {
        exit(1);
    }

    if (SecurityChecks::detectFastExecution() || SecurityChecks::detectEmulation() || SecurityChecks::checkSandboxProcesses()) {
        exit(1);
    }

    if (SecurityChecks::checkMacAddress()) {
        exit(1);
    }

    if (SecurityChecks::checkCPUIDStrings()) {
        exit(1);
    }

    // list of processes to check
    std::vector<std::wstring> processesToCheck = {
        xorstr_(L"ida64.exe"), xorstr_(L"Taskmgr.exe"), xorstr_(L"Dumpcap.exe"), xorstr_(L"Wireshark.exe"), xorstr_(L"SystemInformer.exe"),
        xorstr_(L"ProcessHacker.exe"), xorstr_(L"dotPeek64.exe"), xorstr_(L"ida32.exe"), xorstr_(L"ida.exe"), xorstr_(L"reclass.net.exe"),
        xorstr_(L"reclass.exe"), xorstr_(L"heyrays.exe"), xorstr_(L"lighthouse.exe"), xorstr_(L"cheatengine-x86_64.exe"), xorstr_(L"classinformer.exe"),
        xorstr_(L"ida-x86emu.exe"), xorstr_(L"cffexplorer.exe"), xorstr_(L"winhex.exe"), xorstr_(L"hiew.exe"), xorstr_(L"fiddler.exe"),
        xorstr_(L"httpdebugger.exe"), xorstr_(L"httpdebuggerpro.exe"), xorstr_(L"scylla.exe"), xorstr_(L"Cheat Engine.exe"),
        xorstr_(L"ollydbg.exe"), xorstr_(L"x64dbg.exe"), xorstr_(L"PE.EXE"), xorstr_(L"ida.exe"), xorstr_(L"PEiD.exe"), xorstr_(L"r2.exe"),
        xorstr_(L"procexp.exe"), xorstr_(L"VBoxSDS.exe"), xorstr_(L"VBoxSVC.exe"), xorstr_(L"vpnclient_x64.exe"), xorstr_(L"Everything.exe"), xorstr_(L"VirtualBox.exe"), xorstr_(L"vmware.exe"), xorstr_(L"xenservice.exe"),
        xorstr_(L"vmsrvc.exe"), xorstr_(L"vboxservice.exe,"), xorstr_(L"joeboxserver.exe"), xorstr_(L"prl_cc.exe"), xorstr_(L"VMTools.exe"), xorstr_(L"Vmhgfs"),
        xorstr_(L"Vmscsi.exe"), xorstr_(L"Vmvss.exe"), xorstr_(L"Vmusbmouse.exe"), xorstr_(L"Vmrawdsk.exe"), xorstr_(L"Vmmouse.exe"), xorstr_(L"VMMEMCTL.exe"),
        xorstr_(L"Vmtoolsd.exe"), xorstr_(L"Vmwaretrat.exe"), xorstr_(L"Vmwareuser.exe"), xorstr_(L"Vmacthlp.exe"), xorstr_(L"vboxservice.exe"), xorstr_(L"vboxtray.exe"), xorstr_(L"VMwareTray.exe")
    };

    // close the programs and run the main clicker, at this point, its probably not running on vm
    for (const auto& processName : processesToCheck) {
        if (SecurityChecks::isProcessRunning(processName)) {
            std::string processActive = xorstr_("[+] Blacklisted process found!\n");
            //animateMessage(processActive, 5);
            SecurityChecks::terminateProcessByName(processName);
        }
    }
}