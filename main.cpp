#include <QApplication>
#include "MultiSliderWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MultiSliderWidget w;
    w.show();

    return a.exec();
}
