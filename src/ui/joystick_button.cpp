//
// Created by cheungxiongwei on 2022/3/31.
//

#include "joystick_button.h"
#include <QVariantAnimation>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QtGlobal>
#include <QBrush>
#include <QTimer>
#include <QStyleOption>
#include <QDebug>

JoystickButton::JoystickButton(QWidget* parent)
    : QWidget(parent) {
    //setAutoFillBackground(true);
    //setAttribute(Qt::WA_OpaquePaintEvent);
    //setAttribute(Qt::WA_TranslucentBackground);
    //setWindowFlag(Qt::FramelessWindowHint);

    mBackground = StaticResource::instance().image_020_01().operator*();
    mBackground = mBackground.scaled(288, 288, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    mMiddleButton = StaticResource::instance().image_020_02().operator*();
    mMiddleButton = mMiddleButton.scaled(78, 78, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    mPointMiddleButton = QPoint{mBackground.width() / 2, mBackground.height() / 2};

    // 280 / 320 * 234 = 210
    // (288 - 210) / 2 = 39
    mJoystickMoveRange.addEllipse(39, 39, 210, 210);
    // (288 - 78) / 2 = 105
    mJoystickCenterRange.addEllipse(105, 105, 78, 78);
    mJoystickCenterRangeR1.addEllipse(58, 58, 171, 171);
    mOri = QPointF(288 / 2, 288 / 2);
    //
    mAnimation = new QVariantAnimation(this);
    mAnimation->setDuration(400);
    mAnimation->setEndValue(mPointMiddleButton);
    QObject::connect(mAnimation, &QVariantAnimation::valueChanged, this, &JoystickButton::onValueChanged);

    mTimer = new QTimer(this);
    QObject::connect(mTimer, &QTimer::timeout, this, &JoystickButton::onTimeout);
}

JoystickButton::~JoystickButton() {
}

QSize JoystickButton::sizeHint() const {
    return {288, 288};
}

QSize JoystickButton::minimumSizeHint() const {
    return {288, 288};
}

void JoystickButton::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.eraseRect(rect());
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::NoBrush);

    QStyleOption styleOption;
    styleOption.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);

    painter.drawPixmap(rect(), mBackground, mBackground.rect());
    if (mHasSelect || mAnimation->state() == QAbstractAnimation::Running) {
        painter.drawPixmap(QRect(mPointMiddleButton.x() - 39, mPointMiddleButton.y() - 39, 78, 78), mMiddleButton);
    } else {
        //painter.drawPixmap(e->rect().adjusted(dW, dH, -dW, -dH), mMiddleButton, mMiddleButton.rect());
        // (bg_width - middle_width) / 2
        painter.drawPixmap(rect().adjusted(105, 105, -105, -105), mMiddleButton, mMiddleButton.rect());
    }

    // debug
#if 0
    QPen pen(Qt::red);
    painter.setPen(pen);
    painter.drawRect(rect());
    painter.drawPath(mJoystickMoveRange);
    painter.drawPath(mJoystickCenterRange);
    painter.drawPath(mJoystickCenterRangeR1);
    painter.setPen(Qt::NoPen);
#endif
    QWidget::paintEvent(e);
}

void JoystickButton::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        if (mJoystickMoveRange.contains(e->pos())) {
            mAnimation->stop();
            mHasSelect = true;
        }
    }
    QWidget::mousePressEvent(e);
}

void JoystickButton::mouseMoveEvent(QMouseEvent* e) {
    if (mHasSelect) {
        QPointF p1 = mOri;
        QPointF p2(e->pos());
        QLineF line(p1, p2);
        if (line.length() > mDistance) {
            line.setLength(mDistance);
            mPointMiddleButton = line.p2().toPoint();
        } else {
            mPointMiddleButton = e->pos();
        }
#ifdef JOYSTICK_FLC
        mTimer->start(250);
#else
        joystickControl(mPointMiddleButton);
#endif
        update();
    }
    QWidget::mouseMoveEvent(e);
}

void JoystickButton::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        if (mHasSelect) {
            mHasSelect = false;
            QPointF p1 = mOri;
            QPointF p2(e->pos());
            QLineF line(p1, p2);
            if (line.length() > mDistance) {
                line.setLength(mDistance);
                mAnimation->setStartValue(line.p2().toPoint());
            } else {
                mAnimation->setStartValue(e->pos());
            }
            mAnimation->start();
        }
#ifdef JOYSTICK_FLC
        mTimer->stop();
#endif
        emit stopMotion();
    }

    QWidget::mouseReleaseEvent(e);
}

void JoystickButton::onValueChanged(const QVariant& value) {
    mPointMiddleButton = value.toPoint();
    update();
}

void JoystickButton::onTimeout() {
    joystickControl(mPointMiddleButton);
}

void JoystickButton::joystickControl(QPoint& pos) {
    QLineF line(mOri, QPointF(pos));
    qreal angle = line.angle();
    qreal distance = line.length();

    uchar speed = static_cast<uchar>(distance / mDistance * 63.0);

    do {
        //qDebug() << "joystickControl: " << angle << speed;
        if (angle > 345 || angle < 15) {//right
            emit panRight(speed);
            break;
        }
        if (angle > 45 && angle < 135) {//up
            emit tiltUp(speed);
            break;
        }
        if (angle > 135 && angle < 225) {//left
            emit panLeft(speed);
            break;
        }
        if (angle > 225 && angle < 315) {//down
            emit tiltDown(speed);
            break;
        }
        break;
    } while (true);
}
