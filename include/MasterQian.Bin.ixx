module;
#include "MasterQian.Meta.h"
#include <string>
#include <vector>
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.Bin;
export import MasterQian.freestanding;

namespace MasterQian {
	// �ֽڼ���ͼ
	export struct BinView {
		using ValueT = mqbyte;
		using SizeT = mqui64;
		using PointerT = ValueT const*;

		PointerT mData{ };
		SizeT mSize{ };

		constexpr BinView() noexcept = default;
		
		/// <param name="data">�ֽڼ���ַ</param>
		/// <param name="size">�ֽڼ�����</param>
		BinView(PointerT data, SizeT size) noexcept : mData{ data }, mSize{ size } {}

		[[nodiscard]] bool operator == (BinView bv) const noexcept {
			if (mSize != bv.mSize) return false;
			return std::equal(mData, mData + mSize, bv.mData, bv.mData + bv.mSize);
		}

		[[nodiscard]] ValueT operator [] (SizeT pos) const noexcept {
			return mData[pos];
		}

		/// <summary>
		/// ȡ����
		/// </summary>
		[[nodiscard]] SizeT size() const noexcept {
			return mSize;
		}

		/// <summary>
		/// ȡ32λ����
		/// </summary>
		[[nodiscard]] mqui32 size32() const noexcept {
			return static_cast<mqui32>(mSize);
		}

		/// <summary>
		/// ȡ��ַ
		/// </summary>
		[[nodiscard]] PointerT data() const noexcept {
			return mData;
		}

		/// <summary>
		/// �Ƿ�Ϊ��
		/// </summary>
		[[nodiscard]] bool empty() const noexcept {
			return mSize;
		}

		/// <summary>
		/// ��������
		/// </summary>
		[[nodiscard]] PointerT begin() const noexcept {
			return mData;
		}

		/// <summary>
		/// ������β
		/// </summary>
		[[nodiscard]] PointerT end() const noexcept {
			return mData + mSize;
		}
	};

	// �ֽڼ�
	export struct Bin : private std::vector<mqbyte> {
		using ValueT = mqbyte;
		using SizeT = mqui64;
		using PointerT = ValueT*;
		using ConstPointerT = ValueT const*;
		using BaseT = std::vector<mqbyte>;
		using StructT = Bin;
		using ReferenceT = Bin&;
		using ConstReferenceT = Bin const&;

		constexpr Bin() noexcept {}

		/// <summary>
		/// ����ָ�����ȵĿ��ֽڼ�
		/// </summary>
		/// <param name="len">����</param>
		explicit Bin(SizeT len) noexcept : BaseT::vector(len) {}

		/// <summary>
		/// �����ڴ���ַ�볤��ȷ�����ֽڼ�
		/// </summary>
		/// <param name="data">��ַ</param>
		/// <param name="len">����</param>
		Bin(ConstPointerT data, SizeT len) noexcept : BaseT::vector(data, data + len) {}

		/// <summary>
		/// �ظ�ָ���ֽڼ�һ������
		/// </summary>
		/// <param name="bin">�ֽڼ�</param>
		/// <param name="count">�ظ�����</param>
		Bin(ConstReferenceT bin, SizeT count) noexcept {
			reserve(bin.size() * count);
			for (SizeT i{ }; i < count; ++i) {
				insert(end(), bin.begin(), bin.end());
			}
		}

		/// <summary>
		/// ��ʼ���б����ֽڼ�
		/// </summary>
		/// <param name="bin">��ʼ���б�</param>
		Bin(std::initializer_list<ValueT> bin) noexcept : BaseT::vector(bin) {}

		/// <summary>
		/// �ظ�ָ����ʼ���б�һ������
		/// </summary>
		/// <param name="bin">��ʼ���б�</param>
		/// <param name="count">�ظ�����</param>
		Bin(std::initializer_list<ValueT> bin, SizeT count) noexcept {
			reserve(bin.size() * count);
			for (SizeT i{ }; i < count; ++i) {
				insert(end(), bin.begin(), bin.end());
			}
		}

		/// <summary>
		/// ���ֽڼ���ͼ�����ֽڼ�
		/// </summary>
		/// <param name="bv">�ֽڼ���ͼ</param>
		explicit Bin(BinView bv) noexcept : BaseT::vector(bv.mData, bv.mData + bv.mSize) {}

		[[nodiscard]] StructT operator + (ConstReferenceT bin) const noexcept {
			StructT tmp;
			tmp.reserve(size() + bin.size());
			tmp.insert(tmp.end(), begin(), end());
			tmp.insert(tmp.end(), bin.begin(), bin.end());
			return tmp;
		}

		ReferenceT operator += (ConstReferenceT bin) noexcept {
			insert(end(), bin.begin(), bin.end());
			return *this;
		}

		[[nodiscard]] bool operator == (ConstReferenceT bin) const noexcept {
			return std::operator==(*this, bin);
		}

		using BaseT::operator[];
		using BaseT::size;
		using BaseT::data;
		using BaseT::empty;
		using BaseT::begin;
		using BaseT::end;
		using BaseT::cbegin;
		using BaseT::cend;
		using BaseT::rbegin;
		using BaseT::rend;
		using BaseT::crbegin;
		using BaseT::crend;
		using BaseT::clear;
		using BaseT::resize;

		[[nodiscard]] operator BinView() const noexcept {
			return BinView{ data(), size() };
		}

		/// <summary>
		/// ȡ32λ����
		/// </summary>
		[[nodiscard]] mqui32 size32() const noexcept {
			return static_cast<mqui32>(size());
		}

		/// <summary>
		/// ȡ�ֽڼ����
		/// </summary>
		/// <param name="num">�ֽ���</param>
		[[nodiscard]] StructT left(SizeT num) const noexcept {
			StructT tmp;
			if (num <= size()) {
				tmp.insert(tmp.end(), begin(), begin() + num);
			}
			return tmp;
		}

		/// <summary>
		/// ȡ�ֽڼ��ұ�
		/// </summary>
		/// <param name="num">�ֽ���</param>
		[[nodiscard]] StructT right(SizeT num) const noexcept {
			StructT tmp;
			if (num <= size()) {
				tmp.insert(tmp.end(), begin() + size() - num, end());
			}
			return tmp;
		}

		/// <summary>
		/// ȡ�ֽڼ��м�
		/// </summary>
		/// <param name="start">��ʼλ��</param>
		/// <param name="num">�ֽ���</param>
		[[nodiscard]] StructT middle(SizeT start, SizeT num) const noexcept {
			StructT tmp;
			if (start + num <= size()) {
				tmp.insert(tmp.end(), begin() + start, begin() + start + num);
			}
			return tmp;
		}

		/// <summary>
		/// �����Դ����Ű�����ʾÿ���ֽ����ݵ��Ѻ��ַ��������ڳ���520���ȵ��ֽڼ�������ʾ
		/// ���������MasterQian.Log�⽫��ֱ���������Ӧ�豸
		/// </summary>
		void Log(std::wstring& buf) const noexcept {
			auto len{ size() };
			buf += L"Bin[";
			buf += std::to_wstring(len);
			buf += L"]";
			if (len <= 520ULL) { // specific information will not be displayed if the length exceeds 520
				buf.reserve(4 * len); // 0 - 255 has 2 character length in average
				buf.push_back(L'{');
				for (auto v : *this) {
					buf += std::to_wstring(v);
					buf.push_back(L',');
				}
				if (len) buf.back() = L'}';
				else buf.push_back(L'}');
			}
		}
	};

	// �ı���������
	export enum class CodePage : mqenum {
		ANSI = 0U, UTF8 = 65001U
	};

	/// <summary>
	/// �ֽڼ�ת�ַ���
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <param name="cp">���룬Ĭ��ΪUTF8</param>
	/// <returns>�ַ���</returns>
	export [[nodiscard]] inline std::wstring ToString(BinView bv, CodePage cp = CodePage::UTF8) noexcept {
		std::wstring tmp;
		if (auto len{ api::MultiByteToWideChar(static_cast<mqui32>(cp), 0, reinterpret_cast<mqcstra>(bv.data()),
			static_cast<mqi32>(bv.size32()), nullptr, 0) }; len > 0) {
			tmp.resize(len);
			api::MultiByteToWideChar(static_cast<mqui32>(cp), 0, reinterpret_cast<mqcstra>(bv.data()),
				static_cast<mqi32>(bv.size32()), tmp.data(), len);
		}
		return tmp;
	}

	/// <summary>
	/// �ַ���ת�ֽڼ�
	/// </summary>
	/// <param name="sv">�ַ���</param>
	/// <param name="cp">���룬Ĭ��ΪUTF8</param>
	/// <returns>�ֽڼ�</returns>
	export [[nodiscard]] inline Bin ToBin(std::wstring_view sv, CodePage cp = CodePage::UTF8) noexcept {
		Bin tmp;
		if (auto len{ api::WideCharToMultiByte(static_cast<mqui32>(cp), 0, sv.data(),
			static_cast<mqui32>(sv.size()), nullptr, 0, nullptr, nullptr) }; len > 0) {
			tmp.resize(len);
			api::WideCharToMultiByte(static_cast<mqui32>(cp), 0, sv.data(), static_cast<mqui32>(sv.size()),
				reinterpret_cast<mqstra>(tmp.data()), len, nullptr, nullptr);
		}
		return tmp;
	}
}

export [[nodiscard]] inline MasterQian::Bin operator ""_ansi(mqcstr str, mqui64 size) noexcept {
	return MasterQian::ToBin(std::wstring_view{ str, size }, MasterQian::CodePage::ANSI);
}

export [[nodiscard]] inline MasterQian::Bin operator ""_utf8(mqcstr str, mqui64 size) noexcept {
	return MasterQian::ToBin(std::wstring_view{ str, size }, MasterQian::CodePage::UTF8);
}