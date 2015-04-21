#include "cell.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QMenu>
#include <algorithm>
#include <math.h>
#include <cassert>
#define NDEBUG

QList<Cell*> Cell::finalNodes;
QList<Cell*> Cell::seenNodes;
QList<Cell*> Cell::portalNodes;
int Cell::numberOfNodesSeen=0;
int Cell::numberOfNodesVisited=0;
HeuristicEstimate Cell::heuEstim=None;
bool Cell::useClosestFirst=true;
bool Cell::costHigherForDiagonalTravel=true;
bool Cell::jps=true;
double Cell::weight=1.0;
bool Cell::usePortalsChains=true;
bool Cell::useCthulhu=true;
QList<Cell*> Cell::cells;

Cell::Cell(int i, int j): QGraphicsObject(), neighbours(), siblingPortals()
  ,state(initialCell), previous(), posH(i), posV(j)
  , cost(1000000.0), portalEstimate(1000000.0)//, pheromones(-1.0)
{
    Cell::cells.append(this);
}

void Cell::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    if(event->button()==Qt::LeftButton)
        setState(wall);
    QGraphicsItem::mousePressEvent(event);
}

Cell* Cell::getPrevious()
{
    if(previous.empty())
        return NULL;
    return previous.first();
}
void Cell::setPrevious(Cell* c)
{
    previous.clear();
    previous.push_back(c);
}

double Cell::heuristicEstimate()
{
    /*if(heuEstim==None)
    {return 0.0;}
    else if(heuEstim==Manhattan)
    {return getManhattanDistance();}
    else if(heuEstim==Chebychev)
    {return getChebychevDistance();}
    else if(heuEstim==euclidian)
    {return getEuclidianDistance();}
    else if(heuEstim==euclidianSquared)
    {return getEuclidianDistanceSquared();}

    return 0.0;*/

    double distance=getDistance();

    if(usePortalsChains)
    {
        distance=std::min(distance, getPortalDistance());
    }

    return distance+cthuluhAjustement();
}

double Cell::getPortalDistance()const
{
    double minDistance=1000000.0;
    QList<Cell*>::const_iterator i;
    for (i = portalNodes.constBegin(); i != portalNodes.constEnd(); ++i)
    {
        if((**i).getState()!=visited)
            minDistance=std::min(getDistanceFrom(**i)+(**i).portalEstimate, minDistance);
    }


    return minDistance;
}

double Cell::getDistance()const
{
    double minDistance=1000000.0;
    QList<Cell*>::const_iterator i;
    for (i = finalNodes.constBegin(); i != finalNodes.constEnd(); ++i)
    {
        minDistance=std::min(getDistanceFrom(**i), minDistance);
    }
    return minDistance;
}


double Cell::getManhattanDistance()const
{
    double minDistance=1000000.0;
    QList<Cell*>::const_iterator i;
    for (i = finalNodes.constBegin(); i != finalNodes.constEnd(); ++i)
    {
        minDistance=std::min(getManhattanDistanceFrom(**i), minDistance);
    }
    return minDistance;
}

double Cell::getManhattanDistanceFrom(const Cell& c)const
{
    return weight*double(abs(posH-c.getHorizontalPosition())+abs(posV-c.getVerticalPosition()));
}

double Cell::getChebychevDistance()const
{
    double minDistance=1000000.0;
    QList<Cell*>::const_iterator i;
    for (i = finalNodes.constBegin(); i != finalNodes.constEnd(); ++i)
    {
        minDistance=std::min(getChebychevDistanceFrom(**i), minDistance);
    }
    return minDistance;
}

double Cell::getChebychevDistanceFrom(const Cell& c)const
{
    if(!costHigherForDiagonalTravel)
        return 1.0*double(std::max(abs(posH-c.getHorizontalPosition()),abs(posV-c.getVerticalPosition())));
    return weight*((1.5-2.0*1.0)*double(std::min(abs(posH-c.getHorizontalPosition()),abs(posV-c.getVerticalPosition())))+1.0*double(abs(posH-c.getHorizontalPosition())+abs(posV-c.getVerticalPosition())));

}

double Cell::getEuclidianDistanceSquared()const
{
    double minDistance=1000000.0;
    QList<Cell*>::const_iterator i;
    for (i = finalNodes.constBegin(); i != finalNodes.constEnd(); ++i)
    {
        minDistance=std::min(getEuclidianDistanceSquaredFrom(**i), minDistance);
    }
    return minDistance;
}

double Cell::getDistanceFrom(const Cell& c)const
{
    if(heuEstim==None)
    {return 0.0;}
    else if(heuEstim==Manhattan)
    {return getManhattanDistanceFrom(c);}
    else if(heuEstim==Chebychev)
    {return getChebychevDistanceFrom(c);}
    else if(heuEstim==euclidian)
    {return getEuclidianDistanceFrom(c);}
    else if(heuEstim==euclidianSquared)
    {return getEuclidianDistanceSquaredFrom(c);}
    return 0.0;
}

double Cell::getEuclidianDistanceSquaredFrom(const Cell& c)const
{
    return weight*double(abs(posH-c.getHorizontalPosition())*abs(posH-c.getHorizontalPosition())+abs(posV-c.getVerticalPosition())*abs(posV-c.getVerticalPosition()));
}

double Cell::getEuclidianDistance()const
{
    double minDistance=1000000.0;
    QList<Cell*>::const_iterator i;
    for (i = finalNodes.constBegin(); i != finalNodes.constEnd(); ++i)
    {
        minDistance=std::min(getEuclidianDistanceFrom(**i), minDistance);
    }
    return minDistance;
}

double Cell::getEuclidianDistanceFrom(const Cell& c)const
{
    return weight*sqrt(abs(posH-c.getHorizontalPosition())*abs(posH-c.getHorizontalPosition())+abs(posV-c.getVerticalPosition())*abs(posV-c.getVerticalPosition()));
}


int Cell::getHorizontalPosition()const
{return posH;}
int Cell::getVerticalPosition()const
{return posV;}

Cell::~Cell()
{
    while(!Cell::finalNodes.isEmpty())
        Cell::finalNodes.pop_back();
    while(!Cell::seenNodes.isEmpty())
        Cell::seenNodes.pop_back();
    Cell::cells.removeOne(this);
    //previous=NULL;
}

QRectF Cell::boundingRect() const
 {
     qreal adjust = 2;
     return QRectF(-10 - adjust, -10 - adjust,
                   23 + adjust, 23 + adjust);
 }

void Cell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
 {
     painter->setPen(Qt::NoPen);
     painter->setBrush(Qt::darkGray);
     painter->drawRect(-7, -7, 20, 20);

     QRadialGradient gradient(-3, -3, 10);
     if (option->state & QStyle::State_Sunken) {
         gradient.setCenter(3, 3);
         gradient.setFocalPoint(3, 3);
         gradient.setColorAt(1, QColor(getColor()).light(120));
         gradient.setColorAt(0, QColor(getDarkColor()).light(120));
     } else {
         gradient.setColorAt(0, getColor());
         gradient.setColorAt(1, getDarkColor());
     }
     painter->setBrush(gradient);
     painter->setPen(QPen(Qt::black, 0));
     painter->drawRect(-10, -10, 20, 20);
 }

QColor Cell::getColor()
{
    if(state==initialCell)
    {
        return Qt::red;
    }
    else if(state==path)
    {
        return Qt::green;
    }
    else if(state==visited)
    {
        return Qt::blue;
    }
    else if(state==seen)
    {
        return Qt::yellow;
    }
    else if(state==wall)
    {
        return Qt::black;
    }
    else if(state==final)
    {
        return Qt::cyan;
    }
    else if(state==portal)
    {
        return Qt::magenta;
    }
    else
    {
        qDebug()<<"Invalid cell state";
        return Qt::gray;
    }
}

QColor Cell::getDarkColor()
{
    if(state==initialCell)
    {
        return Qt::darkRed;
    }
    else if(state==path)
    {
        return Qt::darkGreen;
    }
    else if(state==visited)
    {
        return Qt::darkBlue;
    }
    else if(state==seen)
    {
        return Qt::darkYellow;
    }
    else if(state==wall)
    {
        return Qt::black;
    }
    else if(state==final)
    {
        return Qt::darkCyan;
    }
    else if(state==portal)
    {
        return Qt::darkMagenta;
    }
    else
    {
        qDebug()<<"Invalid cell state";
        return Qt::darkGray;
    }
}

void Cell::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
 {
    menuEvent(event->screenPos());
 }

void Cell::menuEvent(const QPoint& pos)
{
    QMenu menu;
    QAction *setSeenAction = menu.addAction(tr("Initial node."));
    QAction *setDestinationAction = menu.addAction(tr("Destination node."));
    QAction *setWallAction = menu.addAction(tr("Transform into wall."));
    QAction *setPortalAction = menu.addAction(tr("Transform into portal."));

    QAction *selectedAction = menu.exec(pos);
    if(selectedAction==setSeenAction)
    {
        setState(seen);
        setCost(0.0);
    }
    else if(selectedAction==setDestinationAction)
    {
        setState(final);
    }
    else if(selectedAction==setWallAction)
    {
        setState(wall);
    }
    else if(selectedAction==setPortalAction)
    {
        setState(portal);
    }
}



void Cell::setState(CellState c)
{
    if(state!=c)
    {
        Cell::seenNodes.removeOne(this);
        //case seen
        if(c==seen)
        {
            Cell::seenNodes.push_back(this);
            if(state==initialCell)
            {
                state=seen;
            }
            numberOfNodesSeen++;
            update();
            return;
        }
        //case wall
        //case final
        else if(c==final)
        {
            Cell::finalNodes.push_back(this);
            recalculatePortalCosts();
        }
        //case portal
        else if(c==portal)
        {
            static Cell* firstPortal=NULL;
            if(firstPortal==NULL)
                firstPortal=this;
            else
            {
                //firstPortal->addNeighboor(this);
                firstPortal->addSiblingPortal(this);
                //addNeighboor(firstPortal);
                addSiblingPortal(firstPortal);

                Cell::portalNodes.push_back(firstPortal);
                Cell::portalNodes.push_back(this);
                recalculatePortalCosts();

                firstPortal=NULL;
            }
        }
        else if(c==visited)
            numberOfNodesVisited++;
        state=c;
        update();
    }
}

void Cell::addSiblingPortal(Cell* c)
{
    siblingPortals.push_back(c);
    addNeighboor(c);
}

Cell* Cell::getSiblingPortal()
{
    //compare coordonnates
    if(!siblingPortals.empty())
    {
        return *(siblingPortals.begin());
    }
    qDebug()<<"Cell::getSiblingPortal:: other portal not found";
    assert(false);
    return NULL;
}

void Cell::recalculatePortalCosts()
{
    if(usePortalsChains and !portalNodes.empty())
    {
        bool changed=true;
        while(changed)
        {
            changed=false;
            //calculate in comparaison of  final
            QList<Cell*>::iterator i;
            for (i = portalNodes.begin(); i != portalNodes.end(); ++i)
            {
                //distance to final
                double distFinal=(**i).heuristicEstimate();
                if((**i).portalEstimate>distFinal)
                {
                    //change value
                    (**i).portalEstimate=distFinal;
                    changed=true;
                    //idem for dest of portal +1
                    if((**i).getSiblingPortal()!=NULL && (**i).getSiblingPortal()->portalEstimate>(**i).portalEstimate+1.0)
                        (**i).getSiblingPortal()->portalEstimate=(**i).portalEstimate+1.0;

                }
            }
        }
    }
}

void Cell::nextStep()
{
    //take node with lowest expected cost
    Cell* cell=getLeastCostlyCell();
    if(cell==NULL)
    {
        qDebug()<<"There is no starting cells.";
        return ;
    }
    //if goal; reconstruct path
    if(cell->isDestination())
    {
        seenNodes.clear();//remove all nodes, no more progression
        qDebug()<<"Solution total cost: "<<cell->reconstructPath();
        qDebug()<<"Number of cells seen: "<<numberOfNodesSeen;
        qDebug()<<"Number of cells visited: "<<numberOfNodesVisited;
    }
    else
    {
        //remove from seen set
        seenNodes.removeOne(cell);
        //mark as visited
        cell->setState(visited);
        //for each neihgboor
        QList<Cell*>::const_iterator i;
        for (i = cell->neighbours.constBegin(); i != cell->neighbours.constEnd(); ++i)
        {
            //if has worst cost or has not been seen:
                //recalculate cost, set previous
                //add to seen
            if((*i)->getState()!=wall)
            {
                if((*i)->getCost()>cell->getCost()+getDistanceOfNeighboors(cell, (*i)))
                {
                    (*i)->setCost(cell->getCost()+getDistanceOfNeighboors(cell, (*i)));
                    (*i)->setPrevious(cell);
                    (*i)->setState(seen);
                }

            }
        }
    }
}

double Cell::getCost()const
{
    return cost;
}
void Cell::setCost(double c)
{
    cost=c;
}

CellState Cell::getState()
{
    return state;
}

bool Cell::isPortal()const
{
    return state==portal;
}

bool Cell::isDestination()const
{
    return state==final;
}

Cell* Cell::getLeastCostlyCell()
{
    Cell* leastCostly=NULL;
    QList<Cell*>::const_iterator i;
    for (i = seenNodes.constBegin(); i != seenNodes.constEnd(); ++i)
    {
        if(leastCostly==NULL || (*i)->expectedCost()<leastCostly->expectedCost() ||  (useClosestFirst /*and (*i)->expectedCost()<=leastCostly->expectedCost()*/ and (*i)->heuristicEstimate()<leastCostly->heuristicEstimate()))
            leastCostly=(*i);
    }
    return leastCostly;
}

int Cell::expectedCost()
{
    return cost+heuristicEstimate();
}

/*
function reconstruct_path(came_from, current_node)
     if current_node in came_from
         p := reconstruct_path(came_from, came_from[current_node])
         return (p + current_node)
     else
         return current_node
*/

double Cell::reconstructPath()
{
    if(!(state==final))
        setState(path);
    if(hasNoPredecessors())
    {
        return 0.0;
    }
    else
    {
        return reconstructPreviousPath() + getDistanceOfNeighboors(this, getPrevious());
    }
}

double Cell::reconstructPreviousPath()
{
    return getPrevious()->reconstructPath();
}

bool Cell::hasNoPredecessors()
{
    return previous.empty();
}

void Cell::addNeighboor(Cell* c)
{
    neighbours.push_back(c);
}

double Cell::getDistanceOfNeighboors(Cell* c1, Cell* c2)
{
    if(Cell::costHigherForDiagonalTravel)
    {
        /*if((c1->getHorizontalPosition()==c2->getHorizontalPosition()-1 and c1->getVerticalPosition()==c2->getVerticalPosition()) or
                (c1->getHorizontalPosition()==c2->getHorizontalPosition()+1 and c1->getVerticalPosition()==c2->getVerticalPosition()) or
                (c1->getHorizontalPosition()==c2->getHorizontalPosition() and c1->getVerticalPosition()==c2->getVerticalPosition()+1) or
                (c1->getHorizontalPosition()==c2->getHorizontalPosition() and c1->getVerticalPosition()==c2->getVerticalPosition()-1)
                 )*/
        if(c1->getManhattanDistanceFrom(*c2)==1.0)
            return 1.0;
        else if(c1->getChebychevDistanceFrom(*c2)==1.0)
            return 1.5;
        //portal
        else return 1.0;
    }
    else return 1.0;
    //test diagonal acivated
    //case normal
    //case diagonal
    //case error
    //case portal

}

double Cell::cthuluhAjustement()
{
    double ajustement=0.0;
    if(useCthulhu and !isDestination() and !(isPortal()))
    {
        ajustement=-1.0;
        bool freeNeigh=false;
        QList<Cell*>::const_iterator i;
        for (i = neighbours.constBegin(); i != neighbours.constEnd(); ++i)
        {
            if((*i)->getState()==visited)
            {
                ajustement+=1.0;
            }
            else if((*i)->getState()!=wall)
            {
                freeNeigh=true;
            }
        }

        if(!freeNeigh)
            return 1000.0;
    }
    return ajustement;
}
