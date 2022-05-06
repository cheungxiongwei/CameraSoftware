#include <QApplication>
#include "window.h"

int main(int argc, char* argv[]) {

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    //format.setOption(QSurfaceFormat::DebugContext, true);
    QSurfaceFormat::setDefaultFormat(format);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication a(argc, argv);

    StaticResource::instance().print();
    StaticResource::instance().image_001();

    qSetMessagePattern("%{time h:mm:ss.zzz}|%{file}|%{line}|%{function}|%{message}");
    a.setStyleSheet(StaticResource::instance().loadStyleSheetDefault());
    Window w;
    w.show();

    return QApplication::exec();
}
