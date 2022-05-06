//
// Created by cheungxiongwei on 2022/3/30.
//

#ifndef CAMERASOFTWARE_KEEP_RATIO_WIDGET_H
#define CAMERASOFTWARE_KEEP_RATIO_WIDGET_H

#include <QWidget>
#include <QByteArray>

class KeepRatioWidget : public QWidget {
    Q_OBJECT
public:
    // ratio = width / height
    // ratio = 16.0 / 9.0 => 16:9
    // ratio = 4.0 / 3.0 => 4:3
    KeepRatioWidget(qreal ratio = 1.0, QWidget* parent = nullptr);
    ~KeepRatioWidget() = default;

protected:
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
private:
    inline int heightForWidth(int width) const;
    inline int widthForHeight(int height) const;
private:
    qreal ratio = 1.0;
};

#endif//CAMERASOFTWARE_KEEP_RATIO_WIDGET_H
