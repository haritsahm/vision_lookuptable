#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPainter>
#include <QFileDialog>
#include <QListView>
#include <QWidget>
#include <QObject>
#include <QShortcut>

#include "image_clip.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

  ImageClip *clipper;

  QGraphicsPixmapItem pixmapItem;

  QShortcut *space_bar;

signals:
  void setFile(QString path);
  void updateImage(bool status);
  void loadLUT(QString path);
  void saveLUT(bool status);
  void updateLUT();

public slots:
  void on_browse_btn_clicked();
  void on_load_file_clicked();
  void on_update_image_btn_clicked();
  void on_lut_browse_btn_clicked();
  void on_load_lut_btn_clicked();
  void on_save_lut_btn_clicked();
  void on_lut_item_box_activated(QString);
  void on_add_lut_btn_toggled();
  void on_rem_lut_btn_toggled();
  void on_lutview_check_stateChanged(int state);
  void setImage(QImage &image);
  void updateLoggingView();

protected:
  //    void paintEvent(QPainter *e, const QStyleOptionGraphicsItem *option);


};

#endif // MAINWINDOW_H
