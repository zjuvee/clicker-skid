
/*

the antisandbox/antidebugging part was fully coded by tosted

it is a pretty bad bypass, if you want you can improve it or adapt it to your needs

*/


#ifndef SECURITYCHECKS_HPP
#define SECURITYCHECKS_HPP

class SecurityChecks {
public:
    static bool isProcessRunning(const std::wstring& processName);
    static void terminateProcessByName(const std::wstring& processName);
    static bool isHWIDMatched(const std::vector<std::string>& hwidList, const std::string& currentHWID);
    static bool isDebuggerPresent();
    static bool checkRemoteDebuggerPresent();
    static bool isUSBConnected();
    static bool isDriveConnectedToF();
    static bool registryKeyExists(const std::wstring& keyName);
    static bool checkCPUIDStrings();
    static bool isVirtualDiskName(const std::wstring& diskName);
    static bool detectFastExecution();
    static bool ntQueryInformationProcess();
    static bool detectEmulation();
    static bool checkSandboxProcesses();
    static bool checkMacAddress();
    static bool burguer();
    static std::vector<std::wstring> getDiskNames();
    static std::string executeCommand(const std::string& cmd);
    static std::string getUsername();
    static bool isUserMatched(const std::vector<std::string>& userList, const std::string& currentUser);
};

#endif // SECURITYCHECKS_HPP
