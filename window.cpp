//
// Created by cheungxiongwei on 2022/3/30.
//

#include "window.h"
#include <QMdiArea>
#include <QTimer>



Window::Window(QWidget* parent)
    : KeepRatioWidget(4.0 / 3.0, parent) {


    mMdiWindow = new QMdiArea(this);
    //mMdiWindow->setBackground(QColor(Qt::black));

    auto layout = new QGridLayout;
    layout->setMargin(0);
    layout->addWidget(mMdiWindow);
    setLayout(layout);

    //
    mMainWindow = mMdiWindow->addSubWindow(new CameraWidget, Qt::CustomizeWindowHint);
    mSubWindow = mMdiWindow->addSubWindow(new CameraWidget, Qt::CustomizeWindowHint);

    mSidebarWidget = new SidebarWidget(mMdiWindow);
    mSidebarWidget->setAutoFillBackground(true);
    mSidebarWidget->adjustSize();

    mPopup = new QToolButton(this);
    mPopup->setWindowFlags(Qt::WindowStaysOnTopHint);
    mPopup->setIconSize({48, 48});
    if (mSidebarWidget->isHidden()) {
        mPopup->setIcon(StaticResource::instance().image_021_show().operator*());
    } else {
        mPopup->setIcon(StaticResource::instance().image_021_hide().operator*());
    }

    QObject::connect(mPopup, &QToolButton::clicked, this, [this]() {
        auto call_animation = [&](QWidget* w) {
            QPropertyAnimation* animation = new QPropertyAnimation(w, "pos");
            animation->setDuration(800);
            if (w == mSidebarWidget) {
                animation->setStartValue(QPoint(width(), 0));
                animation->setEndValue(QPoint(width() - mSidebarWidget->width(), 0));
            } else if (w == mPopup) {
                animation->setStartValue(QPoint(mMdiWindow->width() - mPopup->width() / 2, mMdiWindow->height() / 2));
                animation->setEndValue(QPoint(mMdiWindow->width() - mSidebarWidget->width() - mPopup->width() / 2, mMdiWindow->height() / 2));
            }
            animation->start();
        };

        if (mSidebarWidget->isHidden()) {
            mPopup->setIcon(StaticResource::instance().image_021_hide().operator*());
            mSidebarWidget->show();
            call_animation(mSidebarWidget);
            call_animation(mPopup);
        } else {
            mPopup->setIcon(StaticResource::instance().image_021_show().operator*());
            mPopup->move(mMdiWindow->width() - mPopup->width() / 2, mMdiWindow->height() / 2);
            mSidebarWidget->hide();
        }
    });

    setMinimumSize(1200, 900);

    mMdiWindow->installEventFilter(this);
    mSubWindow->installEventFilter(this);
    mMainWindow->installEventFilter(this);
}

Window::~Window() {
}

QSize Window::sizeHint() const {
    return {1200, 900};
}

bool Window::eventFilter(QObject* watched, QEvent* e) {
    if(watched == this) {
        return KeepRatioWidget::eventFilter(watched, e);
    }

    if(e->type() == QEvent::ContextMenu) {
        return true;
    }

    if(watched == mMdiWindow && e->type() == QEvent::Resize){
        mSidebarWidget->setFixedHeight(mMdiWindow->height());
        mSidebarWidget->move(width() - mSidebarWidget->width(), 0);
        if (mSidebarWidget->isHidden()) {
            mPopup->move(mMdiWindow->width() - mPopup->width() / 2, mMdiWindow->height() / 2);
        } else {
            mPopup->move(mMdiWindow->width() - mSidebarWidget->width() - mPopup->width() / 2, mMdiWindow->height() / 2);
        }
    }

    auto adjust_z = [](QWidget *lower,QSize size,QWidget *raise){
        lower->resize(size);
        lower->move(0,0);

        lower->setWindowFlags(lower->windowFlags() & ~Qt::WindowStaysOnTopHint);
        lower->lower();

        raise->setWindowFlags(raise->windowFlags() | Qt::WindowStaysOnTopHint);
        raise->raise();
    };

    if(watched == mMainWindow && e->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
        if(mouseEvent->button() == Qt::RightButton || mouseEvent->button() == Qt::MidButton){
            return true;
        }
        adjust_z(mMainWindow,mMdiWindow->size(),mSubWindow);
        return true;
    }

    if(watched == mSubWindow && e->type() == QEvent::MouseButtonDblClick){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
        if(mouseEvent->button() == Qt::RightButton || mouseEvent->button() == Qt::MidButton){
            return true;
        }
        adjust_z(mSubWindow,mMdiWindow->size(),mMainWindow);
        return true;
    }

    return false;
}

void Window::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
}

