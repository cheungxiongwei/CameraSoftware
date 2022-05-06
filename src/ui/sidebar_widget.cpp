//
// Created by cheungxiongwei on 2022/3/30.
//

#include "sidebar_widget.h"
#include <QFrame>

SidebarWidget::SidebarWidget(QWidget* parent)
    : QWidget(parent) {

    this->setStyleSheet(QString::fromUtf8(""));

    mGridLayout = new QGridLayout();
    mGridLayout->setMargin(0);
    mGridLayout->setContentsMargins(0, 0, 0, 0);
    mGridLayout->setSpacing(3);

    auto helper = StaticResource::instance();

    // define function
    auto addBtn = [&helper, layout = mGridLayout](decltype(helper.image_001()) input, int row, int column, int width = 84, int height = 84) {
        auto btn = new QToolButton();
        helper.setButtonToolIcon(input.operator*(), btn, {width, height});
        layout->addWidget(btn, row, column);
        return btn;
    };

    addBtn(helper.image_001(), 0, 0);
    addBtn(helper.image_002(), 0, 1);
    addBtn(helper.image_003(), 0, 2);

    addBtn(helper.image_004(), 1, 0);
    addBtn(helper.image_005(), 1, 1);
    addBtn(helper.image_006(), 1, 2);

    addBtn(helper.image_007(), 2, 0);
    addBtn(helper.image_008(), 2, 1);
    addBtn(helper.image_009(), 2, 2);

    addBtn(helper.image_010(), 3, 0);
    addBtn(helper.image_011(), 3, 1);
    addBtn(helper.image_012(), 3, 2);

    addBtn(helper.image_013(), 4, 0);
    addBtn(helper.image_014(), 4, 1)->hide();
    addBtn(helper.image_015(), 4, 2);

    // Joystick
    mJoystickButton = new JoystickButton(this);
    // calc above. value = 3 * width + 2 * space
    mJoystickButton->setObjectName(QString::fromUtf8("joystickButton"));
    mJoystickButton->setFixedSize(288,288);
    mGridLayout->addWidget(mJoystickButton,5,0,1,3);

    mGridLayout->setRowStretch(6,1);

    auto addOtherBtn = [&helper, layout = mGridLayout](decltype(helper.image_001()) input, int row, int column, int width = 84, int height = 42) {
      auto btn = new QToolButton();
      helper.setButtonToolIcon(input.operator*(), btn, {width, height});
      layout->addWidget(btn, row, column);
    };

    addOtherBtn(helper.image_016(), 7, 0);
    addOtherBtn(helper.image_017(), 7, 2);

    addOtherBtn(helper.image_018(), 8, 0);
    addOtherBtn(helper.image_019(), 8, 2);

    setLayout(mGridLayout);

    QTimer::singleShot(1000, [&layout = mGridLayout]() {
      auto w = layout->itemAt(0)->widget();
      qDebug() << w->size() << w->rect() << w->sizeHint();
    });
}

SidebarWidget::~SidebarWidget() {
}
