//
// Created by cheungxiongwei on 2022/3/29.
//

#include "static_resource.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QToolButton>

StaticResource& StaticResource::instance() {
    static StaticResource obj;
    return obj;
}

void StaticResource::print() {
    qDebug() << "static resource.";
}

std::unique_ptr<QPixmap>& StaticResource::image_001() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("001.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_002() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("002.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_003() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("003.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_004() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("004.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_005() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("005.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_006() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("006.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_007() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("007.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_008() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("008.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_009() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("009.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_010() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("010.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_011() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("011.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_012() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("012.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_013() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("013.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_014() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("014.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_015() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("015.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_016() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("016.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_017() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("017.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_018() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("018.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_019() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("019.jpg"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_020_01() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("020-01.png"));
    return obj;
}

std::unique_ptr<QPixmap>& StaticResource::image_020_02() {
    static std::unique_ptr<QPixmap> obj = std::make_unique<QPixmap>(resource("020-02.png"));
    return obj;
}

std::unique_ptr<QIcon>& StaticResource::image_021_hide() {
    static std::unique_ptr<QIcon> obj = std::make_unique<QIcon>(QPixmap(resource("021-hide.png")));
    return obj;
}

std::unique_ptr<QIcon>& StaticResource::image_021_show() {
    static std::unique_ptr<QIcon> obj = std::make_unique<QIcon>(QPixmap(resource("021-show.png")));
    return obj;
}

void StaticResource::setButtonToolIcon(const QString& pixmap_filename, QToolButton* button, StaticResource::SizeType type) {
    QIcon icon;
    icon.addFile(pixmap_filename);
    button->setIcon(icon);

    do {
        if (type == SizeType::px48) {
            button->setIconSize({48, 48});
            break;
        }
        if (type == SizeType::px96x48) {
            button->setIconSize({96, 48});
            break;
        }
        if (type == SizeType::px96) {
            button->setIconSize({96, 96});
            break;
        }
        break;
    } while (true);
}

void StaticResource::setButtonToolIcon(const QPixmap& pixmap, QToolButton* button, const QSize size) {
    QIcon icon;
    icon.addPixmap(pixmap,QIcon::Normal, QIcon::Off);
    button->setIcon(icon);
    button->setIconSize(size);
}

QString StaticResource::loadStyleSheetFromFilename(const QString& filename) {
    QFile fs(filename);
    if (fs.open(QFile::ReadOnly)) {
        QByteArray qss = fs.readAll();
        fs.close();
        return qss;
    }
    return "";
}

QString StaticResource::loadStyleSheetDefault() {
    return loadStyleSheetFromFilename(root + "/default.qss");
}

StaticResource::StaticResource() {
    root = qApp->applicationDirPath();
}

const QString StaticResource::resource(const QString& filename) const {
    return root + "/images/" + filename;
}

