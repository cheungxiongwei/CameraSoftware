//
// Created by cheungxiongwei on 2022/3/29.
//

#ifndef CAMERASOFTWARE_CAMERA_RECORD_H
#define CAMERASOFTWARE_CAMERA_RECORD_H

#include <QObject>
#include <functional>
#include <thread>

class CameraRecord_;
class CameraRecord : public QObject {
    Q_OBJECT
public:
    explicit CameraRecord(std::function<bool(qreal)> func,QObject *parent = nullptr);
    ~CameraRecord();
    int open(const std::string &in_filename,const std::string &out_filename);
    void stop();
signals:
    void timestampChanged(qreal timestamp);
private:
    int run(const std::string& in_filename, const std::string& out_filename);
private:
    std::function<bool(qreal)> mCallbackFunc;
    std::thread mThread;
};



#endif//CAMERASOFTWARE_CAMERA_RECORD_H
