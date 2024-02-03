module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleName(name) MasterQian_Data_##name
#define MasterQianModuleNameString(name) "MasterQian_Data_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Data.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Data.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("�������������������� Please copy [" MasterQianLibString "] into your program folder ��������������������")

export module MasterQian.Data;
export import MasterQian.Bin;

namespace MasterQian::Data {
	namespace details {
		META_IMPORT_API(mqui32, GetCRC32, mqcbytes, mqui32);
		META_IMPORT_API(void, GetMD5, mqcbytes, mqui32, mqstr);
		META_IMPORT_API(void, Base64Encode, mqcbytes, mqui64, mqstr);
		META_IMPORT_API(void, Base64Decode, mqcstr, mqui64, mqbytes);
		META_MODULE_BEGIN
			META_PROC_API(GetCRC32);
			META_PROC_API(GetMD5);
			META_PROC_API(Base64Encode);
			META_PROC_API(Base64Decode);
		META_MODULE_END
	}
}

export namespace MasterQian::Data {
	/// <summary>
	/// ȡ��ϣֵ
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <returns>��ϣֵ</returns>
	[[nodiscard]] inline mqui64 HashValue(BinView bv) noexcept {
		return freestanding::hash_bytes(bv.data(), bv.size());
	}

	/// <summary>
	/// ȡCRC32У��ֵ
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <returns>CRC32У��ֵ</returns>
	[[nodiscard]] inline mqui32 CRC32(BinView bv) noexcept {
		return details::MasterQian_Data_GetCRC32(bv.data(), bv.size32());
	}

	/// <summary>
	/// ȡMD5ժҪ
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <returns>32λMD5ֵ����д</returns>
	[[nodiscard]] inline std::wstring MD5(BinView bv) noexcept {
		std::wstring md5(32, L'\0');
		details::MasterQian_Data_GetMD5(bv.data(), bv.size32(), md5.data());
		return md5;
	}

	/// <summary>
	/// Base64����
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <returns>Base64�����ַ���</returns>
	[[nodiscard]] inline std::wstring Base64Encode(BinView bv) noexcept {
		auto len{ bv.size() };
		auto num24Bit{ len / 3U };
		auto bPadding{ len > num24Bit * 3 };
		std::wstring base64((num24Bit + bPadding) << 2, L'\0');
		details::MasterQian_Data_Base64Encode(bv.data(), len, base64.data());
		return base64;
	}

	/// <summary>
	/// Base64����
	/// </summary>
	/// <param name="sv">�ַ���</param>
	/// <returns>Base64�����ֽڼ�</returns>
	[[nodiscard]] inline Bin Base64Decode(std::wstring_view sv) noexcept {
		mqui64 len{ sv.size() }, padSize{ }, retSize{ };
		if (len && len % 4 == 0) {
			auto num32Bit{ (len >> 2) - 1 };
			auto pEnd{ &sv.back() };
			if (*pEnd == L'=') padSize = (*(pEnd - 1) == L'=' ? 1 : 2);
			else ++num32Bit;
			retSize = num32Bit * 3 + padSize;
		}
		Bin bin(retSize);
		details::MasterQian_Data_Base64Decode(sv.data(), len, bin.data());
		return bin;
	}
}