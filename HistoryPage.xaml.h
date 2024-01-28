#pragma once

#include "HistoryPage.g.h"

#include "app.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::hyzjkz::implementation {
    struct HistoryPage : HistoryPageT<HistoryPage> {
        bool mNotAtExternalLink; // �Ƿ񲻴�������
        bool mCoverTurnover; // �Ƿ�����Ӫҵ��
        mqui32 mPhotoNum; // ��Ƭ����

        std::array<Media::Imaging::BitmapImage, 10ULL> bmp_nums; // ������ͼ
        Media::Imaging::BitmapImage bmp_default_img; // Ĭ����Ƭ

        event<Data::PropertyChangedEventHandler> mPropertyChanged;

        HistoryPage();
        F_EVENT void OnNavigatedTo(Navigation::NavigationEventArgs const&);

        F_EVENT void CDP_Main_DateChanged(Controls::CalendarDatePicker const&, Controls::CalendarDatePickerDateChangedEventArgs const&);

        F_EVENT void Refresh_Click(IInspectable const&, RoutedEventArgs const&);
        F_EVENT Windows::Foundation::IAsyncAction AddDefault_Click(IInspectable const&, RoutedEventArgs const&);
        F_EVENT Windows::Foundation::IAsyncAction Import_Click(IInspectable const&, RoutedEventArgs const&);

        F_EVENT Windows::Foundation::IAsyncAction GV_Photos_DoubleClick(IInspectable const&, Input::DoubleTappedRoutedEventArgs const&);
        F_EVENT void ModelPhoto_RightClick(IInspectable const&, Input::RightTappedRoutedEventArgs const&);

        F_EVENT Windows::Foundation::IAsyncAction MenuItem_Click(IInspectable const&, RoutedEventArgs const&);

        F_RT bool NotAtExternalLink() const {
            return mNotAtExternalLink;
        }

        F_RT void NotAtExternalLink(bool value) {
            if (mNotAtExternalLink != value) {
                mNotAtExternalLink = value;
                mPropertyChanged(*this, Data::PropertyChangedEventArgs{ L"NotAtExternalLink" });
            }
        }

        F_RT bool CoverTurnover() const {
            return mCoverTurnover;
        }

        F_RT void CoverTurnover(bool value) {
            if (mCoverTurnover != value) {
                mCoverTurnover = value;
                mPropertyChanged(*this, Data::PropertyChangedEventArgs{ L"CoverTurnover" });
            }
        }

        F_RT mqui32 PhotoNum() const {
            return mPhotoNum;
        }

        F_RT void PhotoNum(mqui32 value) {
            if (mPhotoNum != value) {
                mPhotoNum = value;
                mPropertyChanged(*this, Data::PropertyChangedEventArgs{ L"PhotoNum" });
            }
        }

        F_RT event_token PropertyChanged(Data::PropertyChangedEventHandler const& handler) {
            return mPropertyChanged.add(handler);
        }

        F_RT void PropertyChanged(event_token const& token) noexcept {
            mPropertyChanged.remove(token);
        }

        F_RT Microsoft::UI::Xaml::Visibility GetPVBVisibility(mqui32, mqui32, bool) const;
        F_RT Media::Imaging::BitmapImage GetPVBSource(mqui32, mqui32, bool) const;
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct HistoryPage : HistoryPageT<HistoryPage, implementation::HistoryPage> {};
}
