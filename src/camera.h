//
// Created by cheungxiongwei on 2022/3/29.
//

#ifndef CAMERASOFTWARE_CAMERA_H
#define CAMERASOFTWARE_CAMERA_H

#include <QObject>
#include <QImage>
#include <QString>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class Camera_;
class Camera : public QObject{
Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);
    virtual ~Camera();
    bool open(const QString &url);
    void close();
signals:
    void newData(const QImage &data);

private:
    void run();

private:
    Q_DISABLE_COPY(Camera)
    std::unique_ptr<Camera_> d;
    std::thread mThread;
    std::mutex mMutex;
    std::condition_variable mCond;
};



#endif//CAMERASOFTWARE_CAMERA_H
