#ifndef IMAGE_CLIP_H
#define IMAGE_CLIP_H
#include <QGraphicsScene>
#include <QPlainTextEdit>
#include <QStringListModel>
#include <QObject>
#include <QtWidgets>
#include <QListView>
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include <sstream>

class QGraphicsSceneMouseEvent;
class QGraphicsPixmapItem;
class QGraphicsRectItem;

#define WHITE 1
#define GREEN 2
#define BLACK 3
#define OTHER 0

class ImageClip : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(QPointF previousPosition READ previousPosition WRITE setPreviousPosition NOTIFY previousPositionChanged)

public:
    ImageClip(QObject* parent);
    QPointF previousPosition() const;
    void setArgument(int type, bool selection) { this->type = type; this->selection=selection;}
    void setSelection(bool selection) {this->selection=selection;}
    void setLutView(bool selection) { this->lut_view = selection;}

    QStringListModel* loggingModel()
    {
      return logging_model_;
    }
    void log(std::string msg);
    void clearLog();

signals:
    void previousPositionChanged(const QPointF previousPosition);
    void sendImage(QImage &image);
    void loggingUpdated();

public slots:
    void setPreviousPosition(const QPointF previousPosition);
    void loadFile(QString path);
    void updateImage(bool status);
    void loadLUT(QString path);
    void saveLUT(bool status);
    void updateImage();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void getData(cv::Rect &roi);
    void updateLUT();


private:
    QGraphicsRectItem* m_selection          {nullptr};
    QGraphicsPixmapItem* m_currentImageItem {nullptr};
    QPointF m_previousPosition;
    bool m_leftMouseButtonPressed           {false};
    cv::VideoCapture *cap;
    QStringListModel *logging_model_;

    int bins;
    int type;
    bool selection;
    bool lut_view;

    cv::Mat lut;
    cv::Mat frame;
    cv::Mat origin;


};

#endif
