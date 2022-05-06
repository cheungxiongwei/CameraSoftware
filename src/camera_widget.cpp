//
// Created by cheungxiongwei on 2022/3/29.
//

#include "camera_widget.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QPainter>
#include <QDebug>

CameraWidget::CameraWidget(QWidget* parent)
    : QOpenGLWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAutoFillBackground(false);
}

CameraWidget::~CameraWidget() {
    makeCurrent();
}

void CameraWidget::newData(const QImage& data) {
    mImage = data;
    update();
}

void CameraWidget::initializeGL() {
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
}
void CameraWidget::resizeGL(int w, int h) {
    Q_UNUSED(w)
    Q_UNUSED(h)
}
void CameraWidget::paintGL() {
    if (!mImage.isNull()) {
        auto scale_image = mImage.scaled(width(), height(), Qt::KeepAspectRatio);
        int w = scale_image.width();
        int h = scale_image.height();
        int x = (width() - w) / 2;
        int y = (height() - h) / 2;

        QPainter painter(this);
        painter.drawImage(QRect{x, y, w, h}, scale_image, QRect{0, 0, w, h});
    }
}
