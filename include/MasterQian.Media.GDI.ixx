module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleName(name) MasterQian_Media_GDI_##name
#define MasterQianModuleNameString(name) "MasterQian_Media_GDI_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Media.GDI.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Media.GDI.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("�������������������� Please copy [" MasterQianLibString "] into your program folder ��������������������")

export module MasterQian.Media.GDI;
export import MasterQian.Bin;
export import MasterQian.Media.Color;

namespace MasterQian::Media::GDI {
	// GDI�ı�
	export struct GDIText {
		mqcstr content;
		mqui32 size;
		Color color;
		mqcstr font;
	};

	// ͼ���ʽ
	export enum class ImageFormat : mqenum {
		BMP = 0, JPG = 1, GIF = 2, TIFF = 5, PNG = 6,
		ICO = 10,
		TGA = 20,
		MNG = 30,
		RAW = 40,
		PSD = 50,
		UNKNOWN = static_cast<mqenum>(-1),
	};

	// �㷨ģʽ
	export enum class AlgorithmMode : bool {
		FAST, QUALITY
	};

	// �㷨ģʽ��
	export struct AlgorithmModes {
		AlgorithmMode compositing; // ����㷨
		AlgorithmMode pixeloffset; // ����ƫ���㷨
		AlgorithmMode smoothing; // ������㷨
		AlgorithmMode interpolation; // ��ֵ�㷨
	};

	// ����ģʽ
	export constexpr AlgorithmModes FAST_MODE{ AlgorithmMode::FAST, AlgorithmMode::FAST, AlgorithmMode::FAST, AlgorithmMode::FAST };
	/// <summary>
	/// ������ģʽ
	/// </summary>
	export constexpr AlgorithmModes QUALITY_MODE{ AlgorithmMode::QUALITY, AlgorithmMode::QUALITY, AlgorithmMode::QUALITY, AlgorithmMode::QUALITY };

	namespace details {
		META_IMPORT_API(void, StartupGDI);
		META_IMPORT_API(void, ShutdownGDI);
		META_IMPORT_API(mqhandle, CreateImageFromSize, mqsize, Color);
		META_IMPORT_API(mqhandle, CreateImageFromMemory, mqcbytes, mqui32);
		META_IMPORT_API(mqhandle, CreateImageFromFile, mqcstr);
		META_IMPORT_API(mqhandle, CreateImageFromFileIcon, mqcstr, mqui32);
		META_IMPORT_API(mqhandle, CreateImageFromClone, mqhandle);
		META_IMPORT_API(void, DeleteImage, mqhandle);
		META_IMPORT_API(mqbool, OK, mqhandle);
		META_IMPORT_API(mqbool, SaveToFile, mqhandle, mqcstr, ImageFormat);
		META_IMPORT_API(mqhandle, SaveToStream, mqhandle, ImageFormat, mqui32*);
		META_IMPORT_API(void, StreamReadRelease, mqhandle, mqbytes, mqui32);
		META_IMPORT_API(mqsize, Size, mqhandle);
		META_IMPORT_API(mqsize, GetDPI, mqhandle);
		META_IMPORT_API(void, SetDPI, mqhandle, mqsize);
		META_IMPORT_API(mqui32, BitDepth, mqhandle);
		META_IMPORT_API(mqhandle, Thumbnail, mqhandle, mqsize);
		META_IMPORT_API(mqhandle, Crop, mqhandle, mqrect, AlgorithmModes);
		META_IMPORT_API(mqhandle, Border, mqhandle, mqrange, Color, AlgorithmModes);
		META_IMPORT_API(mqhandle, Rotate, mqhandle, mqf64, Color, AlgorithmModes);
		META_IMPORT_API(void, RotateLeft, mqhandle);
		META_IMPORT_API(void, RotateRight, mqhandle);
		META_IMPORT_API(void, FlipX, mqhandle);
		META_IMPORT_API(void, FlipY, mqhandle);
		META_IMPORT_API(void, GrayScale, mqhandle, AlgorithmModes);
		META_IMPORT_API(mqhandle, Resample, mqhandle, mqsize, AlgorithmModes);
		META_IMPORT_API(mqhandle, Combine, mqhandle, mqhandle, mqrect, mqrect, mqbool, AlgorithmModes);
		META_IMPORT_API(void, DrawLine, mqhandle, mqrect, Color, mqf64, AlgorithmModes);
		META_IMPORT_API(void, DrawRectangle, mqhandle, mqrect, Color, mqf64, AlgorithmModes);
		META_IMPORT_API(void, DrawRectangles, mqhandle, mqrect const*, mqui32, Color, mqf64, AlgorithmModes);
		META_IMPORT_API(void, DrawCircle, mqhandle, mqpoint, mqsize, Color, mqf64, AlgorithmModes);
		META_IMPORT_API(void, DrawString, mqhandle, mqpoint, mqcstr, mqui32, Color, mqcstr, AlgorithmModes);
		META_IMPORT_API(void, DrawImage, mqhandle, mqhandle, mqpoint, mqsize, AlgorithmModes);
		META_IMPORT_API(void, FillRectangle, mqhandle, mqrect, Color, AlgorithmModes);
		META_IMPORT_API(void, FillRectangles, mqhandle, mqrect const*, mqui32, Color, AlgorithmModes);
		META_IMPORT_API(void, FillCircle, mqhandle, mqpoint, mqsize, Color, AlgorithmModes);
		META_IMPORT_API(mqhandle, StartPrinterFromName, mqcstr);
		META_IMPORT_API(void, EndPrinter, mqhandle);
		META_IMPORT_API(void, DrawImageToPrinter, mqhandle, mqhandle, AlgorithmModes);

		META_MODULE_BEGIN
			META_PROC_API(StartupGDI);
			META_PROC_API(ShutdownGDI);
			META_PROC_API(CreateImageFromSize);
			META_PROC_API(CreateImageFromMemory);
			META_PROC_API(CreateImageFromFile);
			META_PROC_API(CreateImageFromFileIcon);
			META_PROC_API(CreateImageFromClone);
			META_PROC_API(DeleteImage);
			META_PROC_API(OK);
			META_PROC_API(SaveToFile);
			META_PROC_API(SaveToStream);
			META_PROC_API(StreamReadRelease);
			META_PROC_API(Size);
			META_PROC_API(GetDPI);
			META_PROC_API(SetDPI);
			META_PROC_API(BitDepth);
			META_PROC_API(Thumbnail);
			META_PROC_API(Crop);
			META_PROC_API(Border);
			META_PROC_API(Rotate);
			META_PROC_API(RotateLeft);
			META_PROC_API(RotateRight);
			META_PROC_API(FlipX);
			META_PROC_API(FlipY);
			META_PROC_API(GrayScale);
			META_PROC_API(Resample);
			META_PROC_API(Combine);
			META_PROC_API(DrawLine);
			META_PROC_API(DrawRectangle);
			META_PROC_API(DrawRectangles);
			META_PROC_API(DrawCircle);
			META_PROC_API(DrawString);
			META_PROC_API(DrawImage);
			META_PROC_API(FillRectangle);
			META_PROC_API(FillRectangles);
			META_PROC_API(FillCircle);
			META_PROC_API(StartPrinterFromName);
			META_PROC_API(EndPrinter);
			META_PROC_API(DrawImageToPrinter);
		META_MODULE_END
	}

	// ����GDI
	export inline void StartupGDI() noexcept {
		details::MasterQian_Media_GDI_StartupGDI();
	}

	// �ر�GDI
	export inline void ShutdownGDI() noexcept {
		details::MasterQian_Media_GDI_ShutdownGDI();
	}

	// ͼ��
	export struct Image {
		friend struct Printer;
	private:
		mqhandle handle{ };

		static constexpr struct _placeholder {}_unused;

		Image(mqhandle h, _placeholder) noexcept {
			handle = h;
		}

	public:
		/// <summary>
		/// ���촿ɫͼ��
		/// </summary>
		/// <param name="size">�ߴ�</param>
		/// <param name="color">��ɫ</param>
		Image(mqsize size, Color color) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromSize(size, color);
		}

		/// <summary>
		/// ���ļ�����
		/// </summary>
		/// <param name="fn">�ļ���</param>
		explicit Image(std::wstring_view fn) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromFile(fn.data());
		}

		/// <summary>
		/// �ӳ���ͼ�깹��
		/// </summary>
		/// <param name="fileIconPath">��ִ�г����DLL·��</param>
		/// <param name="index">ͼ������</param>
		Image(std::wstring_view fileIconPath, mqui32 index) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromFileIcon(fileIconPath.data(), index);
		}

		/// <summary>
		/// ���ڴ湹��
		/// </summary>
		/// <param name="bv">�ֽڼ�</param>
		explicit Image(BinView bv) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromMemory(bv.data(), bv.size32());
		}

		Image(Image const& image) noexcept {
			handle = details::MasterQian_Media_GDI_CreateImageFromClone(image.handle);
		}

		Image(Image&& image) noexcept {
			std::swap(handle, image.handle);
		}

		Image& operator = (Image const& image) noexcept {
			if (&image != this) {
				details::MasterQian_Media_GDI_DeleteImage(handle);
				handle = details::MasterQian_Media_GDI_CreateImageFromClone(image.handle);
			}
			return *this;
		}

		Image& operator = (Image&& image) noexcept {
			if (this != &image) {
				std::swap(handle, image.handle);
			}
			return *this;
		}

		~Image() noexcept {
			details::MasterQian_Media_GDI_DeleteImage(handle);
		}

		[[nodiscard]] mqhandle unsafe_handle() const noexcept {
			return handle;
		}

		/// <summary>
		/// ���浽�ļ�
		/// </summary>
		/// <param name="fn">�ļ���</param>
		/// <param name="format">ͼ���ʽ</param>
		bool Save(std::wstring_view fn, ImageFormat format) const noexcept {
			return details::MasterQian_Media_GDI_SaveToFile(handle, fn.data(), format);
		}

		/// <summary>
		/// ���浽�ڴ�
		/// </summary>
		/// <param name="format">ͼ���ʽ</param>
		[[nodiscard]] Bin Save(ImageFormat format) const noexcept {
			Bin bin;
			mqui32 size{ };
			if (auto hStream{ details::MasterQian_Media_GDI_SaveToStream(handle, format, &size) }) {
				bin.reserve(size);
				details::MasterQian_Media_GDI_StreamReadRelease(hStream, bin.data(), size);
			}
			return bin;
		}

		/// <summary>
		/// ���浽�����˷���Σ�գ������COM��ΪIStream*��Ӧ���û��Լ�����Release
		/// </summary>
		/// <param name="format">ͼ���ʽ</param>
		[[nodiscard]] mqhandle SaveToUnsafeStream(ImageFormat format) const noexcept {
			mqui32 size{ };
			return details::MasterQian_Media_GDI_SaveToStream(handle, format, &size);
		}

		[[nodiscard]] bool OK() const noexcept {
			return details::MasterQian_Media_GDI_OK(handle);
		}

		/// <summary>
		/// ȡ���
		/// </summary>
		[[nodiscard]] mqui32 Width() const noexcept {
			return details::MasterQian_Media_GDI_Size(handle).width;
		}

		/// <summary>
		/// ȡ�߶�
		/// </summary>
		[[nodiscard]] mqui32 Height() const noexcept {
			return details::MasterQian_Media_GDI_Size(handle).height;
		}

		/// <summary>
		/// ȡ�ߴ�
		/// </summary>
		[[nodiscard]] mqsize Size() const noexcept {
			return details::MasterQian_Media_GDI_Size(handle);
		}

		/// <summary>
		/// ȡ�ֱ���
		/// </summary>
		[[nodiscard]] mqsize DPI() const noexcept {
			return details::MasterQian_Media_GDI_GetDPI(handle);
		}

		/// <summary>
		/// �÷ֱ���
		/// </summary>
		/// <param name="dpi">�ֱ���</param>
		void DPI(mqsize dpi) noexcept {
			details::MasterQian_Media_GDI_SetDPI(handle, dpi);
		}

		/// <summary>
		/// ȡλ���
		/// </summary>
		[[nodiscard]] mqui32 BitDepth() const noexcept {
			return details::MasterQian_Media_GDI_BitDepth(handle);
		}

		/// <summary>
		/// ȡ����ͼ
		/// </summary>
		/// <param name="size">�ߴ�</param>
		[[nodiscard]] Image Thumbnail(mqsize size) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Thumbnail(handle, size), _unused };
		}

		/// <summary>
		/// �ü�
		/// </summary>
		/// <param name="rect">�ü�����</param>
		/// <param name="mode">�㷨</param>
		[[nodiscard]] Image Crop(mqrect rect, AlgorithmModes mode = FAST_MODE) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Crop(handle, rect, mode), _unused };
		}

		/// <summary>
		/// �߿�
		/// </summary>
		/// <param name="pos">�߿���</param>
		/// <param name="color">�߿���ɫ</param>
		/// <param name="mode">�㷨</param>
		[[nodiscard]] Image Border(mqrange pos, Color color, AlgorithmModes mode = FAST_MODE) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Border(handle, pos, color, mode), _unused };
		}

		/// <summary>
		/// ��ת
		/// </summary>
		/// <param name="angle">��ת�Ƕȣ��Ƕ���</param>
		/// <param name="color">��������ɫ��Ĭ����͸��</param>
		/// <param name="mode">�㷨</param>
		[[nodiscard]] Image Rotate(mqf64 angle, Color color = Colors::Transparency, AlgorithmModes mode = FAST_MODE) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Rotate(handle, angle, color, mode), _unused };
		}

		/// <summary>
		/// ����ת90��
		/// </summary>
		Image& RotateLeft() noexcept {
			details::MasterQian_Media_GDI_RotateLeft(handle);
			return *this;
		}

		/// <summary>
		/// ����ת90��
		/// </summary>
		Image& RotateRight() noexcept {
			details::MasterQian_Media_GDI_RotateRight(handle);
			return *this;
		}

		/// <summary>
		/// ˮƽ��ת
		/// </summary>
		Image& FlipX() noexcept {
			details::MasterQian_Media_GDI_FlipX(handle);
			return *this;
		}

		/// <summary>
		/// ��ֱ��ת
		/// </summary>
		Image& FlipY() noexcept {
			details::MasterQian_Media_GDI_FlipY(handle);
			return *this;
		}

		/// <summary>
		/// �Ҷ�ͼ
		/// </summary>
		/// <param name="mode">�㷨</param>
		Image& GrayScale(AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_GrayScale(handle, mode);
			return *this;
		}

		/// <summary>
		/// ���²���
		/// </summary>
		/// <param name="size">�ߴ�</param>
		/// <param name="mode">�㷨</param>
		[[nodiscard]] Image Resample(mqsize size, AlgorithmModes mode = FAST_MODE) const noexcept {
			return Image{ details::MasterQian_Media_GDI_Resample(handle, size, mode), _unused };
		}

		/// <summary>
		/// �ϲ���������ͼ��ƴ��
		/// </summary>
		/// <param name="image">ͼ��2</param>
		/// <param name="rect1">ͼ��1��ƴ������</param>
		/// <param name="rect2">ͼ��2��ƴ������</param>
		/// <param name="mode">�㷨</param>
		/// <typeparam name="isHorizontal">�Ƿ�����ƴ�ӣ���֮����ƴ��</typeparam>
		template<bool isHorizontal = true>
		[[nodiscard]] Image Combine(Image const& image, mqrect rect1 = { }, mqrect rect2 = { }, AlgorithmModes mode = FAST_MODE) const noexcept {
			if (rect1 == mqrect{ }) {
				auto [width, height] = Size();
				rect1.width = width;
				rect1.height = height;
			}
			if (rect2 == mqrect{ }) {
				auto [width, height] = image.Size();
				rect2.width = width;
				rect2.height = height;
			}
			return Image{ details::MasterQian_Media_GDI_Combine(handle, image.handle, rect1, rect2, isHorizontal, mode), _unused };
		}

		/// <summary>
		/// ����
		/// </summary>
		/// <param name="rect">�����ھ�������</param>
		/// <param name="color">������ɫ</param>
		/// <param name="thickness">������ϸ</param>
		/// <param name="mode">�㷨</param>
		Image& DrawLine(mqrect rect, Color color, mqf64 thickness = 1.0, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawLine(handle, rect, color, thickness, mode);
			return *this;
		}

		/// <summary>
		/// ������
		/// </summary>
		/// <param name="rect">��������</param>
		/// <param name="color">������ɫ</param>
		/// <param name="thickness">������ϸ</param>
		/// <param name="mode">�㷨</param>
		Image& DrawRectangle(mqrect rect, Color color, mqf64 thickness = 1.0, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawRectangle(handle, rect, color, thickness, mode);
			return *this;
		}

		/// <summary>
		/// ���������
		/// </summary>
		/// <param name="rects">�������򼯺�</param>
		/// <param name="color">������ɫ</param>
		/// <param name="thickness">������ϸ</param>
		/// <param name="mode">�㷨</param>
		Image& DrawRectangles(mqlist<mqrect> const& rects, Color color, mqf64 thickness = 1.0, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawRectangles(handle, rects.data(), rects.size32(), color, thickness, mode);
			return *this;
		}

		/// <summary>
		/// ����Բ
		/// </summary>
		/// <param name="point">Բ��λ��</param>
		/// <param name="r">���������뾶</param>
		/// <param name="color">������ɫ</param>
		/// <param name="thickness">������ϸ</param>
		/// <param name="mode">�㷨</param>
		Image& DrawCircle(mqpoint point, mqsize r, Color color, mqf64 thickness = 1.0, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawCircle(handle, point, r, color, thickness, mode);
			return *this;
		}

		/// <summary>
		/// д�ı�
		/// </summary>
		/// <param name="point">�ı�����λ��</param>
		/// <param name="textInfo">�ı���Ϣ</param>
		/// <param name="mode">�㷨</param>
		Image& DrawString(mqpoint point, GDIText const& textInfo, AlgorithmModes mode = FAST_MODE) noexcept {
			mode.compositing = AlgorithmMode::QUALITY;
			details::MasterQian_Media_GDI_DrawString(handle, point, textInfo.content,
				textInfo.size, textInfo.color, textInfo.font, mode);
			return *this;
		}

		/// <summary>
		/// ��ͼ��
		/// </summary>
		/// <param name="image">ͼ��2</param>
		/// <param name="point">ͼ��2��ԭͼ��λ��</param>
		/// <param name="size">ͼ��2�ߴ�</param>
		/// <param name="mode">�㷨</param>
		Image& DrawImage(Image const& image, mqpoint point, mqsize size = { }, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawImage(handle, image.handle, point, size, mode);
			return *this;
		}

		/// <summary>
		/// ������
		/// </summary>
		/// <param name="rect">��������</param>
		/// <param name="color">������ɫ</param>
		/// <param name="mode">�㷨</param>
		Image& FillRectangle(mqrect rect, Color color, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_FillRectangle(handle, rect, color, mode);
			return *this;
		}

		/// <summary>
		/// ���������
		/// </summary>
		/// <param name="rects">�������򼯺�</param>
		/// <param name="color">�����ɫ</param>
		/// <param name="mode">�㷨</param>
		Image& FillRectangles(mqlist<mqrect> const& rects, Color color, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_FillRectangles(handle, rects.data(), rects.size32(), color, mode);
			return *this;
		}

		/// <summary>
		/// �����Բ
		/// </summary>
		/// <param name="point">Բ��λ��</param>
		/// <param name="r">���������뾶</param>
		/// <param name="color">�����ɫ</param>
		/// <param name="mode">�㷨</param>
		Image& FillCircle(mqpoint point, mqsize r, Color color, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_FillCircle(handle, point, r, color, mode);
			return *this;
		}
	};

	/// <summary>
	/// ��ӡ��
	/// <para>�˶��󴴽�ʱ������ӡ��������ʱ��ɴ�ӡ</para>
	/// </summary>
	export struct Printer {
	private:
		mqhandle handle{ };
	public:
		/// <summary>
		/// ���ô�ӡ��
		/// </summary>
		/// <param name="name">��ӡ������</param>
		explicit Printer(std::wstring_view name) noexcept {
			handle = details::MasterQian_Media_GDI_StartPrinterFromName(name.data());
		}

		Printer(Printer const&) noexcept = delete;
		Printer& operator = (Printer const&) noexcept = delete;

		Printer(Printer&& printer) noexcept {
			std::swap(handle, printer.handle);
		}
		
		Printer& operator = (Printer&& printer) noexcept {
			std::swap(handle, printer.handle);
			return *this;
		}

		~Printer() noexcept {
			details::MasterQian_Media_GDI_EndPrinter(handle);
		}

		[[nodiscard]] bool OK() const noexcept {
			return handle != nullptr;
		}

		/// <summary>
		/// ��ͼ��
		/// </summary>
		/// <param name="image">ͼ��</param>
		/// <param name="mode">�㷨</param>
		Printer& DrawImage(Image const& image, AlgorithmModes mode = FAST_MODE) noexcept {
			details::MasterQian_Media_GDI_DrawImageToPrinter(handle, image.handle, mode);
			return *this;
		}
	};
}