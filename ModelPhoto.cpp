#include "pch.h"
#include "ModelPhoto.h"
#include "ModelPhoto.g.cpp"

#include "app.h"

namespace winrt::hyzjkz::implementation {
	hstring ModelPhoto::ThumbPath() const {
		auto path{ Global.c_thumbPath };
		path /= mDateStr;
		path /= mFilename;
		return hstring(std::wstring_view(path));
	}

	hstring ModelPhoto::PhotoPath() const {
		auto path{ Global.c_photoPath };
		path /= mDateStr;
		path /= mFilename;
		return hstring(std::wstring_view(path));
	}

	Media::Imaging::BitmapImage ModelPhoto::PhotoSource() const {
		auto path{ Global.c_thumbPath };
		path /= mDateStr;
		path /= mFilename;
		return util::FileToBMP(path, Global.c_photoThumbSize);
	}
}