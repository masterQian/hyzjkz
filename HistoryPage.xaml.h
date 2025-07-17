#pragma once

#include "HistoryPage.g.h"

#include "app.h"

namespace winrt::hyzjkz::implementation {
    struct HistoryPage : HistoryPageT<HistoryPage> {
        bool mNotAtExternalLink; // 是否不处于外链
        bool mCoverTurnover; // 是否隐藏营业额
        mqui32 mPhotoNum; // 照片数量

        std::array<Microsoft::UI::Xaml::Media::Imaging::BitmapImage, 10ULL> bmp_nums; // 数字贴图
        Microsoft::UI::Xaml::Media::Imaging::BitmapImage bmp_default_img; // 默认照片

        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> mPropertyChanged;

        HistoryPage();
        F_EVENT void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const&);

        F_EVENT void CDP_Main_DateChanged(Controls::CalendarDatePicker const&, Controls::CalendarDatePickerDateChangedEventArgs const&);

        F_EVENT void Refresh_Click(IInspectable const&, RoutedEventArgs const&);
        F_EVENT IAsyncAction AddDefault_Click(IInspectable const&, RoutedEventArgs const&);
        F_EVENT IAsyncAction Import_Click(IInspectable const&, RoutedEventArgs const&);
        F_EVENT IAsyncAction AddCopyData_Click(IInspectable const&, RoutedEventArgs const&);
        F_EVENT void AddCopyData_Plus_Click(IInspectable const&, RoutedEventArgs const&);
        F_EVENT void AddCopyData_Set_Click(IInspectable const&, RoutedEventArgs const&);

        F_EVENT void GV_Photos_Click(IInspectable const&, Input::TappedRoutedEventArgs const&);

        F_EVENT IAsyncAction MenuItem_Click(IInspectable const&, RoutedEventArgs const&);

        F_RT bool HasExternalLink() const;

        F_RT bool NotAtExternalLink() const {
            return mNotAtExternalLink;
        }

        F_RT void NotAtExternalLink(bool value) {
            if (mNotAtExternalLink != value) {
                mNotAtExternalLink = value;
                mPropertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"NotAtExternalLink" });
            }
        }

        F_RT bool CoverTurnover() const {
            return mCoverTurnover;
        }

        F_RT void CoverTurnover(bool value) {
            if (mCoverTurnover != value) {
                mCoverTurnover = value;
                mPropertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"CoverTurnover" });
            }
        }

        F_RT mqui32 PhotoNum() const {
            return mPhotoNum;
        }

        F_RT void PhotoNum(mqui32 value) {
            if (mPhotoNum != value) {
                mPhotoNum = value;
                mPropertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"PhotoNum" });
            }
        }

        F_RT event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler) {
            return mPropertyChanged.add(handler);
        }

        F_RT void PropertyChanged(event_token const& token) noexcept {
            mPropertyChanged.remove(token);
        }

        F_RT Microsoft::UI::Xaml::Visibility GetPVBVisibility(mqui32, mqui32, bool) const;
        F_RT Microsoft::UI::Xaml::Media::Imaging::BitmapImage GetPVBSource(mqui32, mqui32, bool) const;
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct HistoryPage : HistoryPageT<HistoryPage, implementation::HistoryPage> {};
}
