#include "pch.h"
#include <iostream>
#include <MasterQian.Parser.Ini.h>
#include <MasterQian.Storage.Path.h>

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace Windows::Storage;

IAsyncAction Update(hstring const& CurrentVersion) {
    wcout << L"客户端版本: " << CurrentVersion.data() << L"\n";
    HttpClient httpClient;
    Uri url{ L"https://gitee.com/masterQian/hyzjkz_autoupdate/raw/master/version.ini" };
    HttpResponseMessage response{ co_await httpClient.GetAsync(url) };
    response.EnsureSuccessStatusCode();
    Streams::IBuffer buffer{ co_await response.Content().ReadAsBufferAsync() };
    string str{ (char*)buffer.data(), buffer.Length() };
    MasterQian::Parser::Ini ini{ to_hstring(str) };
    hstring newVeresion{ ini[L"main"][L"version"] };
    wcout << L"服务器版本: " << newVeresion.data() << L"\n";
    if (CurrentVersion == newVeresion) {
        wcout << L"版本一致, 无需更新\n";
    }
    else {
        wcout << L"版本不一致, 正在更新...\n";

        for (auto& [key, value] : ini[L"write"]) {
            wcout << L"[更新] 从 " << value << L" 下载到 " << key << L"\n";

            url = Uri{ value };
            response = co_await httpClient.GetAsync(url);
            response.EnsureSuccessStatusCode();
            buffer = co_await response.Content().ReadAsBufferAsync();
            MasterQian::Storage::Path{ key }.Write({ buffer.data(), buffer.Length() });
        }

        for (auto& [key, value] : ini[L"delete"]) {
            wcout << L"[删除] 删除 " << key << L"\n";

            MasterQian::Storage::Path{ key }.Delete();
        }

        for (auto& [key, value] : ini[L"rename"]) {
            wcout << L"[重命名] 将 " << value << L" 改名为 " << key << L"\n";

            MasterQian::Storage::Path{ value }.Rename(key);
        }

        wcout << L"更新成功!\n";
    }
}


int wmain(int argc, wchar_t** argv) {
    wcout.imbue(locale("chs"));
    init_apartment();
    MasterQian::Storage::Path::AutoCurrentFolder();
    try {
        if (argc == 2) {
            Update(argv[1]).get();
            system("pause");
            return 0;
        }
    }
    catch (...) { }
    wcout << L"更新失败, 请重试!\n";
    system("pause");
    return 0;
}