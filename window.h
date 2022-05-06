//
// Created by cheungxiongwei on 2022/3/30.
//

#ifndef CAMERASOFTWARE_WINDOW_H
#define CAMERASOFTWARE_WINDOW_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "src/keep_ratio_widget.h"
#include "src/static_resource.h"
#include "src/camera_widget.h"
#include "src/ui/sidebar_widget.h"

class QMdiArea;
class Window : public KeepRatioWidget {
    Q_OBJECT
public:
    explicit Window(QWidget* parent = nullptr);
    ~Window();
    QSize sizeHint() const override;
    virtual bool eventFilter(QObject *watched, QEvent *e) override;
protected:
    virtual void resizeEvent(QResizeEvent* e) override;
private:
    QMdiArea* mMdiWindow;
    QMdiSubWindow* mMainWindow;
    QMdiSubWindow* mSubWindow;

    SidebarWidget* mSidebarWidget;

    QToolButton *mPopup;
};

#endif//CAMERASOFTWARE_WINDOW_H
