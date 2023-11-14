#include <fstream>
#include <vector>
#include <codecvt>

#include "WslPluginApi.h"

std::ofstream g_logfile;

const WSLPluginAPIV1* g_api = nullptr;

std::vector<char> ReadFromSocket(SOCKET socket)
{
    // Simplfied error handling for the sake of the demo.
    int result = 0;
    int offset = 0;

    std::vector<char> content(1024);
    while ((result = recv(socket, content.data() + offset, 1024, 0)) > 0)
    {
        offset += result;
        content.resize(offset + 1024);
    }

    content.resize(offset);
    return content;
}

HRESULT OnVmStarted(const WSLSessionInformation* Session, const WSLVmCreationSettings* Settings)
{
    g_logfile << "VM created. SessionId=" << Session->SessionId
        << "CustomConfigurationFlags=" << Settings->CustomConfigurationFlags << std::endl;

    // Launch cat /proc/version to get the VM's kernel version
    std::vector<const char*> arguments = {"/bin/cat", "/proc/version", nullptr};
    SOCKET socket{};

    auto result = g_api->ExecuteBinary(Session->SessionId, arguments[0], arguments.data(), &socket);
    if (FAILED(result))
    {
        g_logfile << "Failed to create process, " << result << std::endl;

        return result;
    }

    // Read the process' output.
    auto output = ReadFromSocket(socket);
    closesocket(socket);

    if (output.empty())
    {
        g_logfile << "cat /proc/version returned no output" << std::endl;
        return S_OK;
    }

    if (output.back() == '\n')
    {
        output.pop_back(); // Remove extra LF to clean the output.
    }

    if (output.back() != '\0')
    {
        output.emplace_back('\0'); // Make sure string is null terminated.
    }

    g_logfile << "Kernel version info: " << output.data() << std::endl;

    return S_OK;
}

HRESULT OnVmStopping(const WSLSessionInformation* Session)
{
    g_logfile << "VM Stopping. SessionId=" << Session->SessionId << std::endl;

    return S_OK;
}

HRESULT OnDistroStarted(const WSLSessionInformation* Session, const WSLDistributionInformation* Distribution)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

    g_logfile << "Distribution started. Sessionid= " << Session->SessionId << ", Name=" << converter.to_bytes(Distribution->Name)
        << ", Package=" << converter.to_bytes(Distribution->PackageFamilyName) << ", PidNs=" << Distribution->PidNamespace
        << ", InitPid=" << Distribution->InitPid << std::endl;

    return S_OK;
}

HRESULT OnDistroStopping(const WSLSessionInformation* Session, const WSLDistributionInformation* Distribution)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

    g_logfile << "Distribution Stopping. SessionId=" << Session->SessionId << ", name=" << converter.to_bytes(Distribution->Name)
        << ", package=" << converter.to_bytes(Distribution->PackageFamilyName) << ", PidNs=" << Distribution->PidNamespace
        << ", InitPid=" << Distribution->InitPid << std::endl;

    return S_OK;
}


// Entry point called by wslservice when the plugin is loaded.
EXTERN_C __declspec(dllexport) HRESULT WSLPLUGINAPI_ENTRYPOINTV1(const WSLPluginAPIV1* Api, WSLPluginHooksV1* Hooks)
{
    // Save the API methods to call them later.
    g_api = Api;

    // Open log file.
    g_logfile.open("C:\\wsl-plugin-demo.txt");
    if (!g_logfile)
    {
        // If the log file couldn't be opened, fail with an error.
        return E_ABORT;
    }

    g_logfile << "Plugin loaded. WSL version: " << Api->Version.Major << "." << Api->Version.Minor << "." << Api->Version.Revision
        << std::endl;

    // Require WSL >= 2.0.5 since we use WSLDistributionInformation::InitPid
    WSL_PLUGIN_REQUIRE_VERSION(2, 0, 5, Api);

    // Register the plugin hooks with WSL
    Hooks->OnVMStarted = &OnVmStarted;
    Hooks->OnVMStopping = &OnVmStopping;
    Hooks->OnDistributionStarted = &OnDistroStarted;
    Hooks->OnDistributionStopping = &OnDistroStopping;

    return S_OK;
}