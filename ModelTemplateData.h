#pragma once
#include "ModelTemplateData.g.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::hyzjkz::implementation {
    struct ModelTemplateData : ModelTemplateDataT<ModelTemplateData> {
    private:
        double mTLeft, mTTop, mTWidth, mTHeight;
        event<Data::PropertyChangedEventHandler> mPropertyChanged;
    public:
        ModelTemplateData(double t_left, double t_top, double t_width, double t_height) :
            mTLeft{ t_left }, mTTop{ t_top }, mTWidth{ t_width }, mTHeight{ t_height } {}

        double TLeft() const {
            return mTLeft;
        }

        void TLeft(double value) {
            if (mTLeft != value) {
                mTLeft = value;
                mPropertyChanged(*this, Data::PropertyChangedEventArgs{ L"TLeft" });
            }
        }

        double TTop() const {
            return mTTop;
        }

        void TTop(double value) {
            if (mTTop != value) {
                mTTop = value;
                mPropertyChanged(*this, Data::PropertyChangedEventArgs{ L"TTop" });
            }
        }

        double TWidth() const {
            return mTWidth;
        }

        void TWidth(double value) {
            if (mTWidth != value) {
                mTWidth = value;
                mPropertyChanged(*this, Data::PropertyChangedEventArgs{ L"TWidth" });
            }
        }

        double THeight() const {
            return mTHeight;
        }

        void THeight(double value) {
            if (mTHeight != value) {
                mTHeight = value;
                mPropertyChanged(*this, Data::PropertyChangedEventArgs{ L"THeight" });
            }
        }

        event_token PropertyChanged(Data::PropertyChangedEventHandler const& handler) {
            return mPropertyChanged.add(handler);
        }

        void PropertyChanged(event_token const& token) noexcept {
            mPropertyChanged.remove(token);
        }
    };
}
namespace winrt::hyzjkz::factory_implementation {
    struct ModelTemplateData : ModelTemplateDataT<ModelTemplateData, implementation::ModelTemplateData> {};
}
