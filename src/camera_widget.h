//
// Created by cheungxiongwei on 2022/3/29.
//

#ifndef CAMERASOFTWARE_CAMERA_WIDGET_H
#define CAMERASOFTWARE_CAMERA_WIDGET_H

#include <QImage>
#include <QOpenGLWidget>

class CameraWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit CameraWidget(QWidget* parent = nullptr);
    virtual ~CameraWidget();

    virtual QSize minimumSizeHint() const {
        return {160, 90};
    }

    virtual QSize sizeHint() const {
        return {160, 90};
    }

signals:
public slots:
    void newData(const QImage& data);

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

private:
    QImage mImage;
};



#endif//CAMERASOFTWARE_CAMERA_WIDGET_H
