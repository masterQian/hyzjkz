module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleName(name) MasterQian_Tool_Benchmark_##name
#define MasterQianModuleNameString(name) "MasterQian_Tool_Benchmark_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Tool.Benchmark.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Tool.Benchmark.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

export module MasterQian.Tool.Benchmark;
export import MasterQian.freestanding;

namespace MasterQian::Tool {
	namespace details {
		META_IMPORT_API(void, BenchmarkInitialize);
		META_IMPORT_API(void, BenchmarkRun);
		META_IMPORT_API(void, BenchmarkShutdown);
		META_IMPORT_API(void, BenchmarkAdd, mqcmem, mqcmem, mqui64, mqui64, mqui64, mqcstr);
		META_MODULE_BEGIN
			META_PROC_API(BenchmarkInitialize);
			META_PROC_API(BenchmarkRun);
			META_PROC_API(BenchmarkShutdown);
			META_PROC_API(BenchmarkAdd);
		META_MODULE_END
	}

	/// <summary>
	/// <para>基于Google benchmark [ Source: https://github.com/google/benchmark ]</para>
	/// </summary>
	/// <example>
	/// <para>[例1:不带参数的Benchmark]</para>
	/// <code>
	/// int main() {
	///		Benchmark().Add([ ] (auto arg) noexcept {
	///			mqlist<std::string> s;
	///			for (int i = 0; i < 10000; ++i) {
	///				s.add(std::to_string(i));
	///			}
	///		}).Run();
	/// }
	/// </code>
	/// <para>Benchmark临时对象只需要且只能初始化一次</para>
	/// <para>通过Add方法添加一次任务, Add可以链式调用</para>
	/// <para>再通过Run方法运行所有任务</para>
	/// <para>第一个参数是一个函数指针, 你可以很方便地使用lambda表达式</para>
	/// <para>这个程序将对生成10000以内整数对应字符串的任务作benchmark</para>
	/// </example>
	/// <example>
	/// <para>[例2:带参数的Benchmark]</para>
	/// <code>
	/// int main() {
	///		Benchmark().Add<int>([ ] (auto arg) noexcept {
	///			mqlist<std::string> s;
	///			for (int i = 0; i < *arg; ++i) {
	///				s.add(std::to_string(i));
	///			}
	///		}, { 100, 1000, 10000, 100000 }, 10, L"test").Run();
	/// }
	/// </code>
	/// <para>携带参数时在函数指针后补充了参数列表, Add的后两个参数是迭代次数与任务名称, 均可省略, 默认值分别为1和空,</para>
	/// <para>参数支持任何类型(包括类), 你需要在Add模板处显式表达, 此例中是int</para>
	/// <para>参数列表你可以方便地使用初始化列表{ }构造</para>
	/// <para>在函数回调中, 以auto arg形式方便地获得参数的指针, 本例中auto指代int*</para>
	/// <para>这个程序将分别对生成100, 1000, 10000, 100000以内整数对应字符串的四次任务作benchmark</para>
	/// </example>
	

	/// <summary>
	/// benchmark对象
	/// </summary>
	export struct Benchmark {
		template<typename Arg>
		using Function = void(__stdcall*)(Arg const* arg) noexcept;

		struct nullarg {};

		Benchmark() noexcept {
			details::MasterQian_Tool_Benchmark_BenchmarkInitialize();
		}

		Benchmark(Benchmark const&) = delete;
		Benchmark(Benchmark&&) = delete;
		Benchmark& operator = (Benchmark const&) = delete;
		Benchmark& operator = (Benchmark&&) = delete;

		~Benchmark() noexcept {
			details::MasterQian_Tool_Benchmark_BenchmarkShutdown();
		}

		/// <summary>
		/// 添加有参任务
		/// </summary>
		/// <param name="func">测试函数</param>
		/// <param name="args">参数集合</param>
		/// <param name="count">迭代次数，默认为1</param>
		/// <param name="name">任务名，默认为空</param>
		/// <typeparam name="Arg">参数类型</typeparam>
		template<typename Arg>
		Benchmark& Add(Function<Arg> func, mqlist<Arg> const& args, mqui64 count = 1ULL, std::wstring_view name = L"") noexcept {
			details::MasterQian_Tool_Benchmark_BenchmarkAdd(func, args.begin(), args.size(), sizeof(Arg), count, name.data());
			return *this;
		}

		/// <summary>
		/// 添加无参任务
		/// </summary>
		/// <param name="func">测试函数</param>
		/// <param name="count">迭代次数，默认为1</param>
		/// <param name="name">任务名，默认为空</param>
		Benchmark& Add(Function<nullarg> func, mqui64 count = 1ULL, std::wstring_view name = L"") noexcept {
			return Add(func, { { } }, count, name);
		}

		/// <summary>
		/// 运行
		/// </summary>
		void Run() noexcept {
			details::MasterQian_Tool_Benchmark_BenchmarkRun();
		}
	};
}