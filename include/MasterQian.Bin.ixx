module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.Bin;
export import MasterQian.freestanding;

namespace MasterQian {
	// �ֽڼ���ͼ
	export struct BinView {
		mqcbytes mData{ };
		mqui64 mSize{ };

		constexpr BinView() noexcept = default;

		constexpr BinView(std::initializer_list<mqbyte> bin) noexcept : mData{ bin.begin() }, mSize{ bin.size() } {}
		
		BinView(mqcbytes data, mqui64 size) noexcept : mData{ data }, mSize{ size } {}

		[[nodiscard]] bool operator == (BinView bv) const noexcept {
			if (mSize != bv.mSize) return false;
			return freestanding::memcmp(mData, bv.mData, mSize) == 0;
		}

		[[nodiscard]] mqbyte operator [] (mqui64 pos) const noexcept {
			return mData[pos];
		}

		[[nodiscard]] mqui64 size() const noexcept {
			return mSize;
		}

		/// <summary>
		/// ȡ32λ����
		/// </summary>
		[[nodiscard]] mqui32 size32() const noexcept {
			return static_cast<mqui32>(mSize);
		}

		[[nodiscard]] mqcbytes data() const noexcept {
			return mData;
		}

		[[nodiscard]] bool empty() const noexcept {
			return mSize;
		}

		[[nodiscard]] mqcbytes begin() const noexcept {
			return mData;
		}

		[[nodiscard]] mqcbytes end() const noexcept {
			return mData + mSize;
		}
	};

	// �ֽڼ�
	export struct Bin : private mqbuffer {
		Bin() noexcept = default;

		/// <summary>
		/// ����ָ�����ȵĿ��ֽڼ�
		/// </summary>
		/// <param name="len">����</param>
		explicit Bin(mqui64 len) noexcept : mqbuffer{ len, true } {}

		/// <summary>
		/// �����ڴ���ַ�볤��ȷ�����ֽڼ�
		/// </summary>
		/// <param name="data">��ַ</param>
		/// <param name="len">����</param>
		Bin(mqcbytes data, mqui64 len) noexcept : mqbuffer{ len, false } {
			freestanding::copy(mData, data, len);
		}

		/// <summary>
		/// �ظ�ָ���ֽڼ�һ������
		/// </summary>
		/// <param name="bin">�ֽڼ�</param>
		/// <param name="count">�ظ�����</param>
		Bin(Bin const& bin, mqui64 count) noexcept : mqbuffer{ bin.mSize * count, false } {
			for (mqui64 i{ }; i < count; ++i) {
				freestanding::copy(mData + i * bin.mSize, bin.mData, bin.mSize);
			}
		}

		/// <summary>
		/// ��ʼ���б����ֽڼ�
		/// </summary>
		/// <param name="bin">��ʼ���б�</param>
		Bin(std::initializer_list<mqbyte> bin) noexcept : Bin{ bin.begin(), bin.size() } {}

		/// <summary>
		/// �ظ�ָ����ʼ���б�һ������
		/// </summary>
		/// <param name="bin">��ʼ���б�</param>
		/// <param name="count">�ظ�����</param>
		Bin(std::initializer_list<mqbyte> bin, mqui64 count) noexcept : mqbuffer{ bin.size() * count, false } {
			for (mqui64 i{ }; i < count; ++i) {
				freestanding::copy(mData + i * bin.size(), bin.begin(), bin.size());
			}
		}

		/// <summary>
		/// ���ֽڼ���ͼ�����ֽڼ�
		/// </summary>
		/// <param name="bv">�ֽڼ���ͼ</param>
		explicit Bin(BinView bv) noexcept : Bin{ bv.mData, bv.mSize } {}

		[[nodiscard]] Bin operator + (BinView bv) const noexcept {
			Bin tmp;
			tmp.reserve(mSize + bv.mSize);
			freestanding::copy(tmp.mData, mData, mSize);
			freestanding::copy(tmp.mData + mSize, bv.mData, bv.mSize);
			return tmp;
		}

		Bin& operator += (BinView bv) noexcept {
			Bin tmp;
			tmp.reserve(mSize + bv.mSize);
			freestanding::copy(tmp.mData, mData, mSize);
			freestanding::copy(tmp.mData + mSize, bv.mData, bv.mSize);
			freestanding::swap(*this, tmp);
			return *this;
		}

		[[nodiscard]] bool operator == (Bin const& bin) const noexcept {
			if (mSize != bin.mSize) return false;
			return freestanding::memcmp(mData, bin.mData, mSize) == 0;
		}

		[[nodiscard]] mqbyte operator [] (mqui64 index) const noexcept {
			return mData[index];
		}

		[[nodiscard]] mqbyte& operator [] (mqui64 index) noexcept {
			return mData[index];
		}

		using mqbuffer::size;

		[[nodiscard]] mqcbytes data() const noexcept {
			return mqbuffer::data<mqbyte>();
		}

		[[nodiscard]] mqbytes data() noexcept {
			return mqbuffer::data<mqbyte>();
		}

		[[nodiscard]] bool empty() const noexcept {
			return mSize != 0ULL;
		}

		[[nodiscard]] mqbytes begin() const noexcept {
			return mData;
		}

		[[nodiscard]] mqbytes end() const noexcept {
			return mData + mSize;
		}

		[[nodiscard]] mqcbytes cbegin() const noexcept {
			return mData;
		}

		[[nodiscard]] mqcbytes cend() const noexcept {
			return mData + mSize;
		}

		void reserve(mqui64 size) noexcept {
			mqbuffer::reserve(size, false);
		}

		[[nodiscard]] operator BinView() const noexcept {
			return BinView{ mData, mSize };
		}

		/// <summary>
		/// ȡ32λ����
		/// </summary>
		[[nodiscard]] mqui32 size32() const noexcept {
			return static_cast<mqui32>(size());
		}

		void unsafe_shrink(mqui64 size) noexcept {
			if (mSize > size) {
				mSize = size;
			}
		}

		/// <summary>
		/// ȡ�ֽڼ����
		/// </summary>
		/// <param name="num">�ֽ���</param>
		[[nodiscard]] Bin left(mqui64 num) const noexcept {
			Bin tmp;
			if (num > mSize) {
				num = mSize;
			}
			tmp.reserve(num);
			freestanding::copy(tmp.mData, mData, num);
			return tmp;
		}

		/// <summary>
		/// ȡ�ֽڼ��ұ�
		/// </summary>
		/// <param name="num">�ֽ���</param>
		[[nodiscard]] Bin right(mqui64 num) const noexcept {
			Bin tmp;
			if (num > mSize) {
				num = mSize;
			}
			tmp.reserve(num);
			freestanding::copy(tmp.mData, mData + mSize - num, num);
			return tmp;
		}

		/// <summary>
		/// ȡ�ֽڼ��м�
		/// </summary>
		/// <param name="start">��ʼλ��</param>
		/// <param name="num">�ֽ���</param>
		[[nodiscard]] Bin middle(mqui64 start, mqui64 num) const noexcept {
			Bin tmp;
			if (start + num > mSize) {
				start = 0ULL;
				num = mSize;
			}
			tmp.reserve(num);
			freestanding::copy(tmp.mData, mData + start, num);
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
			tmp.reserve(len);
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