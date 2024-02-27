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
#pragma message("�������������������� Please copy [" MasterQianLibString "] into your program folder ��������������������")

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
	/// <para>����Google benchmark [ Source: https://github.com/google/benchmark ]</para>
	/// </summary>
	/// <example>
	/// <para>[��1:����������Benchmark]</para>
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
	/// <para>Benchmark��ʱ����ֻ��Ҫ��ֻ�ܳ�ʼ��һ��</para>
	/// <para>ͨ��Add�������һ������, Add������ʽ����</para>
	/// <para>��ͨ��Run����������������</para>
	/// <para>��һ��������һ������ָ��, ����Ժܷ����ʹ��lambda���ʽ</para>
	/// <para>������򽫶�����10000����������Ӧ�ַ�����������benchmark</para>
	/// </example>
	/// <example>
	/// <para>[��2:��������Benchmark]</para>
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
	/// <para>Я������ʱ�ں���ָ��󲹳��˲����б�, Add�ĺ����������ǵ�����������������, ����ʡ��, Ĭ��ֵ�ֱ�Ϊ1�Ϳ�,</para>
	/// <para>����֧���κ�����(������), ����Ҫ��Addģ�崦��ʽ���, ��������int</para>
	/// <para>�����б�����Է����ʹ�ó�ʼ���б�{ }����</para>
	/// <para>�ں����ص���, ��auto arg��ʽ����ػ�ò�����ָ��, ������autoָ��int*</para>
	/// <para>������򽫷ֱ������100, 1000, 10000, 100000����������Ӧ�ַ������Ĵ�������benchmark</para>
	/// </example>
	

	/// <summary>
	/// benchmark����
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
		/// ����в�����
		/// </summary>
		/// <param name="func">���Ժ���</param>
		/// <param name="args">��������</param>
		/// <param name="count">����������Ĭ��Ϊ1</param>
		/// <param name="name">��������Ĭ��Ϊ��</param>
		/// <typeparam name="Arg">��������</typeparam>
		template<typename Arg>
		Benchmark& Add(Function<Arg> func, mqlist<Arg> const& args, mqui64 count = 1ULL, std::wstring_view name = L"") noexcept {
			details::MasterQian_Tool_Benchmark_BenchmarkAdd(func, args.begin(), args.size(), sizeof(Arg), count, name.data());
			return *this;
		}

		/// <summary>
		/// ����޲�����
		/// </summary>
		/// <param name="func">���Ժ���</param>
		/// <param name="count">����������Ĭ��Ϊ1</param>
		/// <param name="name">��������Ĭ��Ϊ��</param>
		Benchmark& Add(Function<nullarg> func, mqui64 count = 1ULL, std::wstring_view name = L"") noexcept {
			return Add(func, { { } }, count, name);
		}

		/// <summary>
		/// ����
		/// </summary>
		void Run() noexcept {
			details::MasterQian_Tool_Benchmark_BenchmarkRun();
		}
	};
}