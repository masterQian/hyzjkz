module;
#include "MasterQian.Meta.h"
#include <string>
#include <unordered_map>
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.Parser.Config;
export import MasterQian.Bin;

namespace MasterQian::Parser {
	namespace details {
		template<freestanding::numeric T>
		inline mqcbytes ReadImpl(mqcbytes data, T& t) noexcept {
			freestanding::copy(&t, data, sizeof(T));
			return data + sizeof(T);
		}

		inline mqcbytes ReadImpl(mqcbytes data, std::wstring& t) noexcept {
			mqui64 count{ };
			data = ReadImpl(data, count);
			t.assign(reinterpret_cast<mqcstr>(data), count / sizeof(mqchar));
			return data + count;
		}

		inline mqcbytes ReadImpl(mqcbytes data, Bin& t) noexcept {
			mqui64 count{ };
			data = ReadImpl(data, count);
			t.reserve(count);
			freestanding::copy(t.data(), data, count);
			return data + count;
		}

		template<typename T, typename... Args>
		inline mqcbytes Read(mqcbytes data, T& arg, Args&... args) noexcept {
			data = ReadImpl(data, arg);
			if constexpr (sizeof...(Args) > 0ULL) {
				data = Read(data, args...);
			}
			return data;
		}

		template<freestanding::numeric T>
		inline mqbytes WriteImpl(mqbytes data, T t) noexcept {
			freestanding::copy(data, &t, sizeof(T));
			return data + sizeof(T);
		}

		inline mqbytes WriteImpl(mqbytes data, std::wstring_view t) noexcept {
			mqui64 count{ t.size() * sizeof(mqchar) };
			data = WriteImpl(data, count);
			freestanding::copy(data, t.data(), count);
			return data + count;
		}

		inline mqbytes WriteImpl(mqbytes data, BinView t) noexcept {
			mqui64 count{ t.size() };
			data = WriteImpl(data, count);
			freestanding::copy(data, t.data(), count);
			return data + count;
		}

		template<typename T, typename... Args>
		inline mqbytes Write(mqbytes data, T&& arg, Args&&... args) noexcept {
			data = WriteImpl(data, freestanding::forward<T>(arg));
			if constexpr (sizeof...(Args) > 0ULL) {
				data = Write(data, freestanding::forward<Args>(args)...);
			}
			return data;
		}
	}
	
	// 配置对象
	export struct Config : protected std::unordered_map<std::wstring, Bin, freestanding::isomerism_hash, freestanding::isomerism_equal> {
		using PointT = mqbytes;
		using ConstPointT = mqcbytes;
		using SizeT = mqui64;
		using BaseT = std::unordered_map<std::wstring, Bin, freestanding::isomerism_hash, freestanding::isomerism_equal>;
	public:
		Config() = default;

		// 从字节集创建
		explicit Config(BinView bv) noexcept {
			load(bv);
		}

		using BaseT::empty;
		using BaseT::size;
		using BaseT::contains;
		using BaseT::clear;
		using BaseT::begin;
		using BaseT::end;
		using BaseT::cbegin;
		using BaseT::cend;

		/// <summary>
		/// 置配置
		/// </summary>
		/// <param name="key">配置名</param>
		/// <param name="value">数字配置值</param>
		/// <typeparam name="T">配置类型，支持整数或浮点数类型</typeparam>
		template<freestanding::numeric T>
		void set(std::wstring_view key, T value) noexcept {
			auto data{ reinterpret_cast<ConstPointT>(&value) };
			auto count{ sizeof(T) };
			if (auto iter{ find(key) }; iter != cend()) {
				iter->second = { data, count };
			}
			else {
				try_emplace(std::wstring(key), data, count);
			}
		}

		/// <summary>
		/// 置配置
		/// </summary>
		/// <param name="key">配置名</param>
		/// <param name="value">字符串配置值</param>
		void set(std::wstring_view key, std::wstring_view value) noexcept {
			auto data{ reinterpret_cast<ConstPointT>(value.data()) };
			auto count{ value.size() * sizeof(mqchar) };
			if (auto iter{ find(key) }; iter != cend()) {
				iter->second = { data, count };
			}
			else {
				try_emplace(std::wstring(key), data, count);
			}
		}

		/// <summary>
		/// 置配置
		/// </summary>
		/// <param name="key">配置名</param>
		/// <param name="value">字节集配置值</param>
		void set(std::wstring_view key, BinView value) noexcept {
			if (auto iter{ find(key) }; iter != cend()) {
				iter->second = Bin(value);
			}
			else {
				try_emplace(std::wstring(key), value);
			}
		}

		/// <summary>
		/// 置配置
		/// </summary>
		/// <param name="key">配置名</param>
		/// <param name="value">子配置</param>
		void set(std::wstring_view key, Config const& value) noexcept {
			if (auto iter{ find(key) }; iter != cend()) {
				iter->second = value.save();
			}
			else {
				try_emplace(std::wstring(key), value.save());
			}
		}

		/// <summary>
		/// 取配置
		/// </summary>
		/// <param name="key">配置名</param>
		/// <param name="default_value">默认值</param>
		/// <typeparam name="T">配置类型，支持整数或浮点数</typeparam>
		/// <returns>配置值</returns>
		template<freestanding::numeric T>
		[[nodiscard]] T get(std::wstring_view key, T default_value = { }) const noexcept {
			if (auto iter{ find(key) }; iter != cend()) {
				return *reinterpret_cast<T const*>(iter->second.data());
			}
			else {
				return default_value;
			}
		}

		/// <summary>
		/// 取字符串配置
		/// </summary>
		/// <param name="key">配置名</param>
		/// <param name="default_value">默认值</param>
		/// <returns>配置值</returns>
		[[nodiscard]] std::wstring get(std::wstring_view key, std::wstring_view default_value = { }) const noexcept {
			if (auto iter{ find(key) }; iter != cend()) {
				auto data{ reinterpret_cast<mqcstr>(iter->second.data()) };
				auto count{ iter->second.size() / sizeof(mqchar) };
				return { data, data + count };
			}
			else {
				return std::wstring{ default_value };
			}
		}

		/// <summary>
		/// 取字节集配置
		/// </summary>
		/// <param name="key">配置名</param>
		/// <param name="default_value">默认值</param>
		/// <returns>配置值</returns>
		[[nodiscard]] Bin get_bin(std::wstring_view key, BinView default_value = { }) const noexcept {
			if (auto iter{ find(key) }; iter != cend()) {
				return iter->second;
			}
			else {
				return Bin{ default_value };
			}
		}

		/// <summary>
		/// 取子配置
		/// </summary>
		/// <param name="key">配置名</param>
		/// <param name="default_value">默认值</param>
		/// <returns>配置值</returns>
		[[nodiscard]] Config get_config(std::wstring_view key, Config const& default_value = { }) const noexcept {
			if (auto iter{ find(key) }; iter != cend()) {
				return Config(iter->second);
			}
			else {
				return default_value;
			}
		}

		/// <summary>
		/// 移除配置
		/// </summary>
		/// <param name="key">配置名</param>
		bool remove(std::wstring_view key) noexcept {
			if (auto iter{ find(key) }; iter != cend()) {
				erase(iter);
				return true;
			}
			else {
				return false;
			}
		}

		/// <summary>
		/// 从字节集载入
		/// </summary>
		/// <param name="bv">字节集</param>
		void load(BinView bv) noexcept {
			clear();
			ConstPointT p{ bv.data() };
			SizeT count{ };
			p = details::Read(p, count);
			for (SizeT i{ }; i < count; ++i) {
				std::wstring key;
				Bin value;
				p = details::Read(p, key, value);
				try_emplace(freestanding::move(key), freestanding::move(value));
			}
		}

		/// <summary>
		/// 保存字节集
		/// </summary>
		/// <returns>字节集</returns>
		[[nodiscard]] Bin save() const noexcept {
			SizeT totalCount{ sizeof(SizeT) };
			for (auto& [key, value] : *this) {
				totalCount += key.size() * sizeof(mqchar) + value.size() + sizeof(SizeT) * 2ULL;
			}
			Bin bin(totalCount);
			auto data{ bin.data() };
			data = details::Write(data, BaseT::size());
 			for (auto& [key, value] : *this) {
				data = details::Write(data, key, value);
			}
			return bin;
		}
	};
}