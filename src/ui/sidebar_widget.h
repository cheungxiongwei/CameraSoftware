//
// Created by cheungxiongwei on 2022/3/30.
//

#ifndef CAMERASOFTWARE_SIDEBAR_WIDGET_H
#define CAMERASOFTWARE_SIDEBAR_WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QToolButton>
#include <QTimer>
#include <QDebug>
#include "../static_resource.h"
#include "joystick_button.h"

class SidebarWidget : public QWidget {
    Q_OBJECT
public:
    SidebarWidget(QWidget* parent = nullptr);
    ~SidebarWidget();
private:
    QGridLayout* mGridLayout;
    JoystickButton *mJoystickButton;
};

#endif//CAMERASOFTWARE_SIDEBAR_WIDGET_H
