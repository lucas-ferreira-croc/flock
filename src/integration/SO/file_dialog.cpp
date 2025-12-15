#include "file_dialog.hpp"
#include <windows.h>
#include <ShObjIdl.h>

std::string openFileDialog(const char* filter)
{
    std::string result;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if(FAILED(hr))
        return result;
    
    IFileOpenDialog* pFileOpen = nullptr;
    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if(SUCCEEDED(hr))
    {
        COMDLG_FILTERSPEC fileTypes[] = {
            { L"3D Models", L"*.fbx;*.obj;*.gltf;*.glb;*.dae" },
            { L"All Files", L"*.*" }
        };

        pFileOpen->SetFileTypes(2, fileTypes);
        pFileOpen->SetOptions(FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST);

        hr = pFileOpen->Show(nullptr);

        if(SUCCEEDED(hr))
        {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);

            if(SUCCEEDED(hr))
            {
                PWSTR filepath = nullptr;
                pItem->GetDisplayName(SIGDN_FILESYSPATH, &filepath);

                if(filepath)
                {
                    char buffer[MAX_PATH];
                    wcstombs(buffer, filepath, MAX_PATH);
                    result = buffer;
                    CoTaskMemFree(filepath);
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }
    CoUninitialize();
    return result;
}