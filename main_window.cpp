#include "main_window.h"
#include <iostream>
#include <ui_gui.h>
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  this->setWindowTitle("Vision Lookup Table");

  clipper = new ImageClip(this);
  space_bar = new QShortcut(this);   // Initialize the object
  space_bar->setKey(Qt::Key_Space);    // Set the key code

  ui->graphicsView->setScene(clipper);
  ui->add_lut_btn->setChecked(true);
  ui->log_view->setModel(clipper->loggingModel());

  qRegisterMetaType<QImage>("QImage");
  QObject::connect(clipper, SIGNAL(sendImage(QImage &)), this, SLOT(setImage(QImage &)));
  QObject::connect(this, SIGNAL(setFile(QString)), clipper, SLOT(loadFile(QString)));
  QObject::connect(this, SIGNAL(updateImage(bool)), clipper, SLOT(updateImage(bool)));
  QObject::connect(this, SIGNAL(saveLUT(bool)), clipper, SLOT(saveLUT(bool)));
  QObject::connect(this, SIGNAL(loadLUT(QString)), clipper, SLOT(loadLUT(QString)));
  QObject::connect(clipper, SIGNAL(loggingUpdated()), this, SLOT(updateLoggingView()));
  QObject::connect(space_bar, SIGNAL(activated()), clipper, SLOT(updateImage()));
}

MainWindow::~MainWindow()
{
  //    delete ui;
  this->close();
}

/*
 * FILE VIEWER
 */

void MainWindow::on_browse_btn_clicked()
{
  QString filepath = QFileDialog::getOpenFileName(this, tr("Open File"), "~", "All Files (*.*)");
  ui->path_list->setText(filepath);
}

void MainWindow::on_load_file_clicked()
{
  if(!ui->path_list->text().isEmpty())
  {
    QString path = ui->path_list->text();
    emit setFile(path);
  }

}

void MainWindow::on_update_image_btn_clicked()
{
  emit updateImage(true);
}

void MainWindow::setImage(QImage &image)
{
  ui->graphicsView->scene()->clear();
  ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(image));
  ui->graphicsView->update();
}

/*
 * LUT TABLE
 */

void MainWindow::on_lutview_check_stateChanged(int state)
{
  bool selection;
  selection = ui->lutview_check->isChecked() ? true : false;

  clipper->setLutView(selection);

  std::stringstream text;
  text << "Lut Viewer : " << std::boolalpha << selection;
  clipper->log(text.str());

}

void MainWindow::on_lut_browse_btn_clicked()
{
  QString filepath = QFileDialog::getOpenFileName(this, tr("Open File"), "~", "XML File (*.xml)");
  ui->lut_path->setText(filepath);
}

void MainWindow::on_load_lut_btn_clicked()
{
  if(!ui->path_list->text().isEmpty())
  {
    QString path = ui->lut_path->text();
    emit loadLUT(path);
  }
}

void MainWindow::on_save_lut_btn_clicked()
{
  emit saveLUT(true);
}

void MainWindow::on_add_lut_btn_toggled()
{
  bool selection;
  if(ui->add_lut_btn->isChecked())
    selection = true;

  clipper->setSelection(selection);
  std::stringstream arg;
  arg << "Selection : " << std::boolalpha << selection;
  clipper->log(arg.str());

}

void MainWindow::on_rem_lut_btn_toggled()
{
  bool selection;
  if(ui->rem_lut_btn->isChecked())
    selection = false;

  clipper->setSelection(selection);
  std::stringstream arg;
  arg << "Selection : " << std::boolalpha << selection;
  clipper->log(arg.str());

}

void MainWindow::on_lut_item_box_activated(QString)
{
  std::string text = ui->lut_item_box->currentText().toStdString();
  int type = 0;

  if(text == "Green")
    type = GREEN;
  else if(text == "White")
    type = WHITE;
  else if(text == "Black")
    type = BLACK;
  else if(text == "Other")
    type = OTHER;

  bool selection;
  if(ui->add_lut_btn->isChecked())
    selection = true;
  else if (ui->rem_lut_btn->isChecked())
    selection = false;

  clipper->setArgument(type, selection);
  std::stringstream arg;
  arg << "Type : " << text << "Selection : " << std::boolalpha << selection;
  clipper->log(arg.str());
}

/*
 * LOG VIEWER
 */

void MainWindow::updateLoggingView()
{
  ui->log_view->scrollToBottom();
}
