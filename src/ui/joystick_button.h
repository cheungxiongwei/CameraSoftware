//
// Created by cheungxiongwei on 2022/3/31.
//

#ifndef CAMERASOFTWARE_JOYSTICK_BUTTON_H
#define CAMERASOFTWARE_JOYSTICK_BUTTON_H

#include <QWidget>
#include <QPixmap>
#include <QPainterPath>
#include <QPointF>
#include "../static_resource.h"

class QVariantAnimation;
class QPaintEvent;
class QMouseEvent;
class QTimer;
class JoystickButton : public QWidget {
    Q_OBJECT
public:
    explicit JoystickButton(QWidget* parent = nullptr);
    ~JoystickButton();
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;
protected:
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void mousePressEvent(QMouseEvent* e) override;
    virtual void mouseMoveEvent(QMouseEvent* e) override;
    virtual void mouseReleaseEvent(QMouseEvent* e) override;
signals:
    void stopMotion();
    void panLeft(uchar speed);
    void panRight(uchar speed);
    void tiltUp(uchar speed);
    void tiltDown(uchar speed);
private slots:
    void onValueChanged(const QVariant& value);
    void onTimeout();

private:
    void joystickControl(QPoint& pos);

private:
    QPixmap mBackground;
    QPixmap mMiddleButton;
    QVariantAnimation* mAnimation;
    QTimer* mTimer;

    QPoint mPointMiddleButton;
    QPainterPath mJoystickMoveRange;
    QPainterPath mJoystickCenterRange;
    QPainterPath mJoystickCenterRangeR1;

    bool mHasSelect = false;

    QPointF mOri;
    const double mDistance = 65.0;
};



#endif//CAMERASOFTWARE_JOYSTICK_BUTTON_H
