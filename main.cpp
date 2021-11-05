#include <main_window.h>
#include <QApplication>
#include <QColorDialog>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

//  color.show();
  MainWindow w;
  w.show();

  return a.exec();
}
