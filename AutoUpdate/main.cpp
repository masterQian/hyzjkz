#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

import MasterQian.Log;
import MasterQian.Storage.Path;
import MasterQian.Parser.Ini;

using namespace MasterQian;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Storage;

IAsyncAction Update(ConsoleLogger& logger, winrt::hstring const& CurrentVersion) {
    logger.i(L"客户端版本: ", CurrentVersion);
    HttpClient httpClient;
    Uri url{ L"https://gitee.com/masterQian/hyzjkz_autoupdate/raw/master/version.ini" };
    HttpResponseMessage response{ co_await httpClient.GetAsync(url) };
    response.EnsureSuccessStatusCode();
    Streams::IBuffer buffer{ co_await response.Content().ReadAsBufferAsync() };
    Parser::Ini ini{ ToString(Bin{ buffer.data(), buffer.Length() }) };
    winrt::hstring newVeresion{ ini[L"main"][L"version"] };
    logger.i(L"服务器版本: ", newVeresion);
    if (CurrentVersion == newVeresion) {
        logger.i(L"版本一致, 无需更新");
    }
    else {
        logger.i(L"版本不一致, 正在更新...");
        for (auto& [key, value] : ini[L"write"]) {
            logger.i(L"[更新] 从 ", value, L" 下载到 ", key);
            url = Uri{ value };
            response = co_await httpClient.GetAsync(url);
            response.EnsureSuccessStatusCode();
            buffer = co_await response.Content().ReadAsBufferAsync();
            Storage::Path{ key }.Write({ buffer.data(), buffer.Length() });
        }
        for (auto& [key, value] : ini[L"delete"]) {
            logger.i(L"[删除] 删除 ", key);
            Storage::Path{ key }.Delete();
        }
        for (auto& [key, value] : ini[L"rename"]) {
            logger.i(L"[重命名] 将 ", value, L" 改名为 ", key);
            Storage::Path{ value }.Rename(key);
        }
        logger.i(L"更新成功!");
    }
}

int wmain(int argc, wchar_t** argv) {
    ConsoleLogger logger;
    winrt::init_apartment();
    Storage::Path::AutoCurrentFolder();
    try {
        if (argc == 2) {
            Update(logger, argv[1]).get();
            system("pause");
            return 0;
        }
    }
    catch (...) { }
    logger.e(L"更新失败, 请重试!");
    system("pause");
    return 0;
}