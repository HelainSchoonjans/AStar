#ifndef CELL_H
#define CELL_H
#include <QGraphicsObject>
#include <QList>

enum CellState {initialCell, path, visited, seen, wall, final, portal};
enum HeuristicEstimate {None, Manhattan, Chebychev, euclidian, euclidianSquared};


class Cell : public QGraphicsObject
{
public:
    Cell(int i, int j);
    ~Cell();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
    QColor getDarkColor();
    QColor getColor();
    void menuEvent(const QPoint& pos);
    void setState(CellState);
    CellState getState();

    Cell* getPrevious();
    void setPrevious(Cell*);

    static QList<Cell*> finalNodes;
    static QList<Cell*> seenNodes;
    static QList<Cell*> portalNodes;




    int getHorizontalPosition()const;
    int getVerticalPosition()const;

    static void nextStep();

    bool hasNoPredecessors();

    int expectedCost();
    double getCost()const;
    void setCost(double);

    double reconstructPath();
    double reconstructPreviousPath();
    bool isDestination()const;
    bool isPortal()const;

    void addNeighboor(Cell*);

    static HeuristicEstimate heuEstim;
    static bool costHigherForDiagonalTravel;
    static bool useClosestFirst;
    static bool jps;
    static double weight;
    static bool usePortalsChains;
    static bool useCthulhu;
    static QList<Cell*> cells;

    double heuristicEstimate();
    static double getDistanceOfNeighboors(Cell*, Cell*);

    Cell* getSiblingPortal();

    void addSiblingPortal(Cell* c);
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    static Cell* getLeastCostlyCell();
    static int numberOfNodesSeen;
    static int numberOfNodesVisited;

    double getChebychevDistanceFrom(const Cell& c)const;
    double getChebychevDistance()const;
    double getEuclidianDistanceSquaredFrom(const Cell& c)const;
    double getEuclidianDistanceSquared()const;
    double getEuclidianDistance()const;
    double getEuclidianDistanceFrom(const Cell& c)const;
    double getManhattanDistance()const;
    double getManhattanDistanceFrom(const Cell& c)const;
    double getDistanceFrom(const Cell& c)const;
    double getPortalDistance()const;

    double cthuluhAjustement();

    double getDistance()const;

    void mousePressEvent ( QGraphicsSceneMouseEvent * event );

    void recalculatePortalCosts();

private:

    QList<Cell*> neighbours;
    QList<Cell*> siblingPortals;
    CellState state;
    QList<Cell*> previous;
    int posH, posV;
    double cost;

public:
    double portalEstimate;

};

#endif // CELL_H
