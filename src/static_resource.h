//
// Created by cheungxiongwei on 2022/3/29.
//

#ifndef CAMERASOFTWARE_STATIC_RESOURCE_H
#define CAMERASOFTWARE_STATIC_RESOURCE_H

#include <QPixmap>
#include <QString>
#include <memory>

class QToolButton;
class StaticResource {
public:
    static StaticResource& instance();

    void print();

    // load pixmap
    std::unique_ptr<QPixmap>& image_001();

    std::unique_ptr<QPixmap>& image_002();

    std::unique_ptr<QPixmap>& image_003();

    std::unique_ptr<QPixmap>& image_004();

    std::unique_ptr<QPixmap>& image_005();

    std::unique_ptr<QPixmap>& image_006();

    std::unique_ptr<QPixmap>& image_007();

    std::unique_ptr<QPixmap>& image_008();

    std::unique_ptr<QPixmap>& image_009();

    std::unique_ptr<QPixmap>& image_010();

    std::unique_ptr<QPixmap>& image_011();

    std::unique_ptr<QPixmap>& image_012();

    std::unique_ptr<QPixmap>& image_013();

    std::unique_ptr<QPixmap>& image_014();

    std::unique_ptr<QPixmap>& image_015();

    std::unique_ptr<QPixmap>& image_016();

    std::unique_ptr<QPixmap>& image_017();

    std::unique_ptr<QPixmap>& image_018();

    std::unique_ptr<QPixmap>& image_019();

    std::unique_ptr<QPixmap>& image_020_01();

    std::unique_ptr<QPixmap>& image_020_02();

    std::unique_ptr<QIcon>& image_021_hide();
    std::unique_ptr<QIcon>& image_021_show();

    // helper function
    enum class SizeType
    {
        px48,
        px96x48,
        px96,
        px320
    };

    void setButtonToolIcon(const QString& pixmap_filename, QToolButton* button, StaticResource::SizeType type);

    void setButtonToolIcon(const QPixmap& pixmap, QToolButton* button, const QSize size);

    // load style css
    QString loadStyleSheetFromFilename(const QString& filename);

    // load default.qss
    QString loadStyleSheetDefault();

private:
    StaticResource();

    const QString resource(const QString& filename) const;

private:
    QString root;
};


#endif//CAMERASOFTWARE_STATIC_RESOURCE_H
