module;
#include "MasterQian.Meta.h"
#include <string>
#include <unordered_map>
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.Parser.Ini;
export import MasterQian.freestanding;

namespace MasterQian::Parser {
	namespace details {
		mqcstr IniScanLine(mqcstr pData, mqcstr* p1, mqcstr* p2, mqcstr* p3, mqcstr* p4) {
			mqcstr pStart{ pData };
			while (*pData && *pData != L'\r' && *pData != L'\n') ++pData; // Ѱ�ҵ���β
			mqcstr pEnd{ pData };
			while (*pStart == L' ' || *pStart == L'\t') ++pStart; // �������׵Ŀհ�
			while (*pEnd == L'\r' || *pEnd == L'\n') --pEnd; // ������β�Ļ���
			while (*pEnd == L'\0' || *pEnd == L' ' || *pEnd == L'\t') --pEnd; // ������β�Ŀհ�
			std::wstring_view line{ pStart, pEnd + 1ULL };
			while (*pData == L'\r' || *pData == L'\n') ++pData; // ������β�Ļ���
			*p1 = *p2 = *p3 = *p4 = nullptr;
			if (line.front() == L'[' && line.back() == L']') { // ��
				pStart = &line.front() + 1;
				pEnd = &line.back() - 1;
				while (*pStart == L' ' || *pStart == L'\t') ++pStart; // ����[��Ŀհ�
				while (*pEnd == L' ' || *pEnd == L'\t') --pEnd; // ����]ǰ�Ŀհ�
				// ����(��p1 == p2�ǿս���)
				*p1 = pStart;
				*p2 = pEnd + 1;
			}
			else if (line.front() == L';') { } // ע��
			else if (auto pos{ line.rfind(L'=') }; pos != std::wstring_view::npos) { // ��
				auto left{ line.substr(0, pos) };
				auto right{ line.substr(pos + 1) };
				pEnd = &left.back();
				pStart = &right.front();
				while (*pEnd == L' ' || *pEnd == L'\t') --pEnd; // ����=ǰ�Ŀհ�
				while (*pStart == L' ' || *pStart == L'\t') ++pStart; // ����=��Ŀհ�
				*p1 = &left.front(); // ��
				*p2 = pEnd + 1;
				*p3 = pStart; // ֵ
				*p4 = &right.back() + 1;
			}
			else {
				return nullptr;
			}
			return pData;
		}
	}

	// ini�������
	export enum class IniError {
		OK, // �ɹ�
		INVALID_LINE, // �Ƿ���
		EMPTY_SECTION_NAME, // �ս���
		EMPTY_KEY, // �ռ���
		ISOLATED_ITEM // ������
	};

	struct IniView;

	// ����ͼ
	export struct SectionView : protected std::unordered_map<std::wstring_view, std::wstring_view> {
		friend struct IniView;
		using BaseT = std::unordered_map<std::wstring_view, std::wstring_view>;
		using BaseT::empty;
		using BaseT::size;
		using BaseT::contains;

		[[nodiscard]] const auto begin() const noexcept {
			return BaseT::begin();
		}

		[[nodiscard]] const auto end() const noexcept {
			return BaseT::end();
		}

		[[nodiscard]] std::wstring_view operator [] (std::wstring_view key) const noexcept {
			if (auto iter{ find(key) }; iter != cend()) {
				return iter->second;
			}
			return { };
		}
	};

	// ini��ͼ
	export struct IniView : protected std::unordered_map<std::wstring_view, SectionView> {
	protected:
		using BaseT = std::unordered_map<std::wstring_view, SectionView>;

		std::wstring content;
		
		inline static SectionView _empty_section{ };

		void _clear() noexcept {
			clear();
			content.clear();
		}
	public:
		IniView() = default;

		IniView(std::wstring_view data) noexcept {
			Load(data);
		}

		IniView(IniView const&) = delete;
		IniView& operator = (IniView const&) = delete;

		using BaseT::empty;
		using BaseT::size;
		using BaseT::contains;

		[[nodiscard]] const auto begin() const noexcept {
			return BaseT::begin();
		}

		[[nodiscard]] const auto end() const noexcept {
			return BaseT::end();
		}

		[[nodiscard]] SectionView const& operator[](std::wstring_view sv) const noexcept {
			if (auto iter{ find(sv) }; iter != cend()) {
				return iter->second;
			}
			return _empty_section;
		}

		/// <summary>
		/// ����
		/// </summary>
		/// <param name="data">ini�ı�</param>
		IniError Load(std::wstring_view data) noexcept {
			clear();
			content = data;
			SectionView* curSection{ };
			mqcstr pData{ content.data() }, p1{ }, p2{ }, p3{ }, p4{ };
			while (pData && *pData) {
				pData = details::IniScanLine(pData, &p1, &p2, &p3, &p4);
				if (pData) {
					if (p3 && p4) { // ��
						if (curSection) { // ��ǰ�ڴ���
							std::wstring_view key{ p1, p2 }, value{ p3, p4 };
							if (key.empty()) {
								_clear();
								return IniError::EMPTY_KEY;
							}
							else {
								if (auto iter{ curSection->find(key) }; iter != curSection->cend()) {
									iter->second = value; // ���µ�ǰ��
								}
								else {
									curSection->emplace(key, value); // ��������
								}
							}
						}
						else {
							_clear();
							return IniError::ISOLATED_ITEM;
						}
					}
					else if (p1 && p2) { // ��
						if (std::wstring_view name{ p1, p2 }; !name.empty()) {
							if (auto iter{ find(name) }; iter != cend()) { // ���µ�ǰ��
								curSection = &iter->second;
							}
							else {
								curSection = &emplace(name, SectionView{ }).first->second; // �����½�
							}
						}
						else {
							_clear();
							return IniError::EMPTY_SECTION_NAME;
						}
					}
					else { } // ע��
				}
				else {
					_clear();
					return IniError::INVALID_LINE;
				}
			}
			return IniError::OK;
		}
	};

	// ��
	export using Section = std::unordered_map<std::wstring, std::wstring, freestanding::isomerism_hash, freestanding::isomerism_equal>;

	// ini�����ļ�
	export struct Ini : public std::unordered_map<std::wstring, Section, freestanding::isomerism_hash, freestanding::isomerism_equal> {
		Ini() = default;

		Ini(std::wstring_view data) noexcept {
			Load(data);
		}

		/// <summary>
		/// ����
		/// </summary>
		/// <param name="data">ini�ı�</param>
		IniError Load(std::wstring_view data) noexcept {
			clear();
			Section* curSection{ };
			mqcstr pData{ data.data() }, p1{ }, p2{ }, p3{ }, p4{ };
			while (pData && *pData) {
				pData = details::IniScanLine(pData, &p1, &p2, &p3, &p4);
				if (pData) {
					if (p3 && p4) { // ��
						if (curSection) { // ��ǰ�ڴ���
							std::wstring key{ p1, p2 }, value{ p3, p4 };
							if (key.empty()) {
								clear();
								return IniError::EMPTY_KEY;
							}
							else {
								if (auto iter{ curSection->find(key) }; iter != curSection->cend()) {
									iter->second = value; // ���µ�ǰ��
								}
								else {
									curSection->emplace(std::move(key), std::move(value)); // ��������
								}
							}
						}
						else {
							clear();
							return IniError::ISOLATED_ITEM;
						}
					}
					else if (p1 && p2) { // ��
						if (std::wstring name{ p1, p2 }; !name.empty()) {
							if (auto iter{ find(name) }; iter != cend()) { // ���µ�ǰ��
								curSection = &iter->second;
							}
							else {
								curSection = &emplace(std::move(name), Section{ }).first->second; // �����½�
							}
						}
						else {
							clear();
							return IniError::EMPTY_SECTION_NAME;
						}
					}
					else { } // ע��
				}
				else {
					clear();
					return IniError::INVALID_LINE;
				}
			}
			return IniError::OK;
		}

		/// <summary>
		/// ����
		/// </summary>
		/// <returns>ini�ı�</returns>
		std::wstring Save() const noexcept {
			mqui64 totalSize{ };
			for (auto& [name, sec] : *this) {
				totalSize += name.size();
				for (auto& [key, value] : sec) {
					totalSize += key.size() + value.size();
				}
				totalSize += sec.size() * 2ULL; // �����=��\n
			}
			totalSize += size() * 3ULL; // �����[]��\n

			std::wstring buf;
			buf.reserve(totalSize);
			for (auto& [name, sec] : *this) {
				buf.push_back(L'[');
				buf += name;
				buf += L"]\n";
				for (auto& [key, value] : sec) {
					buf += key;
					buf.push_back(L'=');
					buf += value;
					buf.push_back(L'\n');
				}
			}
			return buf;
		}
	};
}