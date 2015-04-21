#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QGraphicsView>
#include "cell.h"
#include <QVector>

class GraphWidget : public QGraphicsView
{
public:
    GraphWidget(QWidget *parent = 0, int w=20, int h=20);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void setDiagonals(bool);
    int getWidth()const;
    int getHeigth()const;
protected:
    void wheelEvent(QWheelEvent *event);
    void scaleView(qreal scaleFactor);
    void initializeCells(QGraphicsScene *scene);
    void launchDialogBoxes();

    bool loadQuestion();

    bool loadFrom(QString);
private:
    int width, heigth;
    QVector<QVector <Cell*> > cells;
    bool diagonals;
};

#endif // SCENEWIDGET_H
