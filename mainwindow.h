#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scenewidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void nextStep();
    void save();

protected:
    void initialize();
    void initializeMenuBar();


private:
    GraphWidget* scene;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
