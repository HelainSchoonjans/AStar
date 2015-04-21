#include "scenewidget.h"
#include <cmath>
#include <QWheelEvent>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QFileDialog>


GraphWidget::GraphWidget(QWidget *parent, int w, int h): QGraphicsView(parent), width(w), heigth(h), cells(), diagonals(true)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-2000, -2000, 4000, 4000);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Grid"));
    setDragMode(QGraphicsView::ScrollHandDrag);
    launchDialogBoxes();



}

void GraphWidget::launchDialogBoxes()
{


    //movement
    int reponse = QMessageBox::question(this, "Movements", "Do you want to enable diagonal travel ?", QMessageBox ::Yes | QMessageBox::No);

    if (reponse == QMessageBox::No)
    {
        diagonals=false;
    }

    QStringList items;
    items << tr("None") << tr("Manhattan distance") << tr("Chebychev") << tr("Eucidian distance") << tr("Eucidian squared distance");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Heuristic estimation"),
                                              tr("Choose an heuristic estimation or the distances to the destination points:"), items, 0, false, &ok);

    if (ok && !item.isEmpty())
    {
        if(item=="None")
        {
            Cell::heuEstim=None;
        }
        else if(item=="Manhattan distance")
        {
            Cell::heuEstim=Manhattan;
        }
        else if(item=="Chebychev")
        {
            Cell::heuEstim=Chebychev;
        }
        else if(item=="Eucidian distance")
        {
            Cell::heuEstim=euclidian;
        }
        else if(item=="Eucidian squared distance")
        {
            Cell::heuEstim=euclidianSquared;
        }
    }

    //movement
    reponse = QMessageBox::question(this, "Movements", "Do you want to enable greedy search?", QMessageBox ::Yes | QMessageBox::No);

    if (reponse == QMessageBox::No)
    {
        Cell::useClosestFirst=false;
    }

    reponse = QMessageBox::question(this, "Optimisation", "Do you want to enable cthuluh optimisation?", QMessageBox ::Yes | QMessageBox::No);

    if (reponse == QMessageBox::No)
    {
        Cell::useCthulhu=false;
    }
    reponse = QMessageBox::question(this, "Portals", "Do you want to enable portal optimisation?", QMessageBox ::Yes | QMessageBox::No);

    if (reponse == QMessageBox::No)
    {
        Cell::usePortalsChains=false;
    }

    loadQuestion();
}

void GraphWidget::initializeCells(QGraphicsScene *scene)
{
    cells.resize(width);
    for(int i=0; i<width; i++)
    {
        cells[i].resize(heigth);
        for(int j=0; j<heigth; j++)
        {
            cells[i][j]=new Cell(i, j);
            scene->addItem(cells[i][j]);
            cells[i][j]->setPos(-width*10+20*i, -heigth*10+20*j);
        }
    }
    for(int i=0; i<width; i++)
    {
        for(int j=0; j<heigth; j++)
        {
            if(j>0)
                cells[i][j]->addNeighboor(cells[i][j-1]);
            if(i>0)
                cells[i][j]->addNeighboor(cells[i-1][j]);
            if(i<width-1)
                cells[i][j]->addNeighboor(cells[i+1][j]);
            if(j<heigth-1)
                cells[i][j]->addNeighboor(cells[i][j+1]);
            if(diagonals)
            {
                if(j>0 and i>0)
                    cells[i][j]->addNeighboor(cells[i-1][j-1]);
                if(i>0 and j<heigth-1)
                    cells[i][j]->addNeighboor(cells[i-1][j+1]);
                if(i<width-1 and j>0)
                    cells[i][j]->addNeighboor(cells[i+1][j-1]);
                if(j<heigth-1 and i<width-1)
                    cells[i][j]->addNeighboor(cells[i+1][j+1]);
            }
        }
    }

}

#include <iostream>
#include <string>
#include <fstream>

bool GraphWidget::loadQuestion()
{
    //box: do you want to load a file?
    int reponse = QMessageBox::question(this, "Map", "Do you want to load an existing map ?", QMessageBox ::Yes | QMessageBox::No);

    if (reponse == QMessageBox::No)
    {
        initializeCells(scene());
        return false;
    }
    //choose file
    QString fichier = QFileDialog::getOpenFileName(this, "Open an existing map", QString(), "Text (*.txt)");
    //if success
    if(loadFrom(fichier))
        return true;
    return false;
}

bool GraphWidget::loadFrom(QString filename)
{
    //load size

    std::ifstream fichier(filename.toStdString().c_str(), std::ios::in);  // on ouvre en lecture

    if(fichier)  // si l'ouverture a fonctionné
    {
            int contenu;  // déclaration d'une chaîne qui contiendra la ligne lue
            fichier >> contenu;
            width=contenu;
            fichier >> contenu;
            heigth=contenu;

            initializeCells(scene());

            while(!fichier.eof())
            {
                int i, j;
                fichier >> i;
                if(fichier.eof())
                {
                    qDebug()<<"Error when loading the file. Invalid syntax.";
                    return false;
                }

                fichier >> j;
                cells[i][j]->setState(wall);
            }

            fichier.close();
    }
    else
    {
        qDebug()<<"File opening fail";
        return false;
    }

    //set walls
    //return true; if success
    return false;
}


void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersect(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);
}

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, event->delta() / 240.0));
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

int GraphWidget::getWidth()const
{
    return width;
}
int GraphWidget::getHeigth()const
{
    return heigth;
}
