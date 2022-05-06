//
// Created by cheungxiongwei on 2022/3/30.
//

#include "keep_ratio_widget.h"
#include <QApplication>
#include <QScreen>
#include <Windows.h>

KeepRatioWidget::KeepRatioWidget(qreal ratio, QWidget* parent)
    : QWidget(parent)
    , ratio(ratio) {
#if 1
    ;
    auto screen = QApplication::primaryScreen();
    auto winSize = screen->availableSize();
    auto min = qMin(winSize.width(),winSize.height());
    min == winSize.width() ? setMaximumSize(min, heightForWidth(min)) : setMaximumSize(widthForHeight(min), min);
#endif
}

bool KeepRatioWidget::nativeEvent(const QByteArray& eventType, void* message, long* result) {
    auto rectWidth = [](RECT* r) { return r->right - r->left; };
    auto rectHeight = [](RECT* r) { return r->bottom - r->top; };

    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);

        if (msg->message == WM_SIZING) {
            RECT* rect = (RECT*)(msg->lParam);

            // Overwrite the right parameter with the calculated value:
            switch (msg->wParam) {
            case WMSZ_BOTTOMRIGHT:
                if (rectWidth(rect) < rectHeight(rect))
                    goto wmsz_bottom;
                else
                    goto wmsz_right;// Always wanted to use goto
                break;
            case WMSZ_BOTTOM:
wmsz_bottom:
                rect->right = rect->left + widthForHeight(rectHeight(rect));
                break;
            case WMSZ_RIGHT:
wmsz_right:
                rect->bottom = rect->top + heightForWidth(rectWidth(rect));
                break;
            case WMSZ_TOPLEFT:
                if (rectWidth(rect) < rectHeight(rect))
                    goto wmsz_top;
                else
                    goto wmsz_left;// goto away, wheeeeee
                break;
            case WMSZ_TOP:
wmsz_top:
                rect->left = rect->right - widthForHeight(rectHeight(rect));
                break;
            case WMSZ_LEFT:
wmsz_left:
                rect->top = rect->bottom - heightForWidth(rectWidth(rect));
                break;
            }
        }
    }

    // Pass the modified Windows Message to the original event handler
    return QWidget::nativeEvent(eventType, message, result);
}

int KeepRatioWidget::heightForWidth(int width) const {
    return width / ratio;
}

int KeepRatioWidget::widthForHeight(int height) const {
    return height * ratio;
}
