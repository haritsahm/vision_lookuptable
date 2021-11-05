#include "image_clip.h"
#include <QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsRectItem>
#include <iostream>


ImageClip::ImageClip(QObject* parent)
{
  cap = new cv::VideoCapture;
  bins = 64;
  lut = cv::Mat(1, bins*bins*bins, CV_8UC1, cv::Scalar(0));
  std::cout <<lut.size()<< std::endl;
  type = GREEN;
  selection = true;
  lut_view = false;
  logging_model_ = new QStringListModel;
}

/**
 * Qt mouse press event.
 * 
 * Set the first rectangle point and draw the rectangle color
 */

void ImageClip::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() & Qt::LeftButton)
  {
    m_leftMouseButtonPressed = true;
    setPreviousPosition(event->scenePos());

    m_selection = new QGraphicsRectItem();
    m_selection->setBrush(QBrush(QColor(158,182,255,96)));
    m_selection->setPen(QPen(QColor(158,182,255,200),1));
    addItem(m_selection);
  }

  QGraphicsScene::mousePressEvent(event);
}

/**
 * Qt mouse move event.
 * 
 * Draw the rectangle by following mouse movement.
 */

void ImageClip::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (m_leftMouseButtonPressed)
  {
    auto dx = event->scenePos().x() - m_previousPosition.x();
    auto dy = event->scenePos().y() - m_previousPosition.y();
    auto left = qMin(m_previousPosition.x(), event->scenePos().x());
    auto top = qMin(m_previousPosition.y(), event->scenePos().y());

    m_selection->setRect(left, top, qAbs(dx),qAbs(dy));
  }
  QGraphicsScene::mouseMoveEvent(event);
}

/**
 * Qt mouse release event.
 * 
 * Get the second rectangle coordinate and create ROI
 */

void ImageClip::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() & Qt::LeftButton)
  {
    m_leftMouseButtonPressed = false;

    QRect selectionRect = m_selection->boundingRect().toRect();
    cv::Rect roi(selectionRect.x(), selectionRect.y(), selectionRect.width(), selectionRect.height() );
    getData(roi);
    delete m_selection;
  }
  QGraphicsScene::mouseReleaseEvent(event);
}

void ImageClip::setPreviousPosition(const QPointF previousPosition)
{
  if (m_previousPosition == previousPosition)
    return;

  m_previousPosition = previousPosition;
  emit previousPositionChanged(m_previousPosition);
}

QPointF ImageClip::previousPosition() const
{
  return m_previousPosition;
}

/**
 * Assign selected labels to all values in ROI.
 */

void ImageClip::getData(cv::Rect &roi)
{
  cv::Mat img_roi;
  if (roi.x > origin.cols) roi.x = origin.cols;
  if (roi.y > origin.rows) roi.y = origin.rows;
  if ((roi.x + roi.width) > origin.cols) roi.width = roi.x + roi.width - origin.cols;
  if ((roi.y + roi.height) > origin.rows) roi.height = roi.y + roi.height - origin.rows;
  img_roi = origin(roi);
  for(int i = 0; i < img_roi.rows; i++)
    for(int j = 0; j < img_roi.cols; j++)
    {
      // Reduce the color value 256 -> 64
      cv::Vec3b intensity = img_roi.at<cv::Vec3b>(i, j);
      uchar h = intensity.val[0] /4;
      uchar s = intensity.val[1] /4;
      uchar v = intensity.val[2] /4;

      if(selection)
        switch(type)
        {
        case GREEN : lut.at<uchar>(0, h+s*bins+v*bins*bins)=GREEN; break;
        case WHITE : lut.at<uchar>(0, h+s*bins+v*bins*bins)=WHITE; break;
        case BLACK : lut.at<uchar>(0,  h+s*bins+v*bins*bins)=BLACK; break;
        case OTHER : lut.at<uchar>(0, h+s*bins+v*bins*bins)=OTHER; break;
        }
      else
        lut.at<uchar>(0, h+s*bins+v*bins*bins)=OTHER;
    }

  //    std::stringstream text;
  //    text << "Data Saved " << " x : " << std::to_string(roi.x) << " y : " << std::to_string(roi.y);
  //    log(text.str());
}

void ImageClip::loadFile(QString path)
{
  if(cap->isOpened())
  {
    cap->release();
    cap->open(path.toStdString());
  }
  else
    cap->open(path.toStdString());
}

void ImageClip::updateImage()
{
  updateImage(true);
}

void ImageClip::updateImage(bool status)
{
  QImage vid_frame;
  if(cap->isOpened())
  {

    if (status)
    {
      cap->read(frame);
      cv::resize(frame, frame, cv::Size(480, 360));
      frame.copyTo(origin);
      cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
      vid_frame = QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    }

    if(lut_view)
      updateLUT();
    else
      emit sendImage(vid_frame);
  }


}

void ImageClip::updateLUT()
{
  cv::Mat imageLut;
  origin.copyTo(imageLut);
  cv::Vec3b colors;


  for(int i = 0; i < imageLut.rows; i++)
    for(int j = 0; j < imageLut.cols; j++)
    {
      cv::Vec3b val = imageLut.at<cv::Vec3b>(i,j);

      int h = val.val[0] / 4;
      int s = val.val[1] / 4;
      int v = val.val[2] / 4;

      uchar space = lut.at<uchar>(0,h+s*bins+v*bins*bins);


      if(space == OTHER)
      {
        colors.val[0]=255;
        colors.val[1]=0;
        colors.val[2]=0;
        imageLut.at<cv::Vec3b>(i,j) = colors;
      }
      else if(space == GREEN)
      {
        colors.val[0]=0;
        colors.val[1]=255;
        colors.val[2]=0;
        imageLut.at<cv::Vec3b>(i,j) = colors;
      }
      else if(space == WHITE)
      {
        colors.val[0]=255;
        colors.val[1]=255;
        colors.val[2]=255;
        imageLut.at<cv::Vec3b>(i,j) = colors;
      }
      else if(space == BLACK)
      {
        colors.val[0]=0;
        colors.val[1]=0;
        colors.val[2]=0;
        imageLut.at<cv::Vec3b>(i,j) = colors;
      }

    }

  cv::cvtColor(imageLut, imageLut, cv::COLOR_BGR2RGB);
  QImage vid_frame(imageLut.data, imageLut.cols, imageLut.rows, imageLut.step, QImage::Format_RGB888);

  emit sendImage(vid_frame);
}

void ImageClip::loadLUT(QString path)
{
  cv::FileStorage storage(path.toStdString(), cv::FileStorage::READ);
  storage["lut"] >> lut;
  storage.release();

  std::stringstream text;
  text << "Data Imported from " << path.toStdString();
  log(text.str());

}

void ImageClip::saveLUT(bool status)
{
  std::string filename = "lut.xml";
  cv::FileStorage storage(filename, cv::FileStorage::WRITE);
  storage << "lut" << lut;
  storage.release();


  std::stringstream text;
  text << "Data Exported to " << filename;
  log(text.str());

}

void ImageClip::log(std::string msg)
{
  logging_model_->insertRows(logging_model_->rowCount(), 1);
  std::stringstream logging_model_msg;

  logging_model_msg << "[DEBUG] " << msg;

  QVariant new_row(QString(logging_model_msg.str().c_str()));
  logging_model_->setData(logging_model_->index(logging_model_->rowCount() - 1), new_row);
  emit loggingUpdated();  // used to readjust the scrollbar
}

void ImageClip::clearLog()
{
  if (logging_model_->rowCount() == 0)
    return;

  logging_model_->removeRows(0, logging_model_->rowCount());
}
