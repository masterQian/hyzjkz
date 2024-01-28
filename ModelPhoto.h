#pragma once

#include "ModelPhoto.g.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::hyzjkz::implementation {
    struct ModelPhoto : ModelPhotoT<ModelPhoto> {
    private:
        hstring mDateStr;
        hstring mFilename;
    public:
        ModelPhoto(hstring const& date_str, hstring const& filename) :
            mDateStr{ date_str }, mFilename{ filename } {}

        hstring ThumbPath() const;
        hstring PhotoPath() const;
        Media::Imaging::BitmapImage PhotoSource() const;
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct ModelPhoto : ModelPhotoT<ModelPhoto, implementation::ModelPhoto> {};
}
