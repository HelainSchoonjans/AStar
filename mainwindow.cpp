#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    scene(new GraphWidget()),
    ui(new Ui::MainWindow)
{
    initialize();
    initializeMenuBar();
    setCentralWidget(scene);
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    ui->setupUi(this);
    //nom, icone
    setWindowTitle(tr("Projet INFO3 - Energy distribution"));
    setWindowIcon(QIcon("icon.png"));
}


void MainWindow::initializeMenuBar()
{
    //barre de menu
    //QAction *newAction_, *testG_, *customG_;
    QMenu *menuFichier = menuBar()->addMenu(tr("File"));
    //newAction_ = menuFichier->addAction(tr("&New graph"));
    //newAction_->setShortcut(QKeySequence(tr("Ctrl+N", "hotkey for generating a new graph")));
    //testG_ = menuFichier->addAction(tr("Generate &test graph"));
    //testG_->setShortcut(QKeySequence(tr("Ctrl+T", "hotkey for generating a testGraph")));
    //customG_ = menuFichier->addAction(tr("Generate &custom graph"));
    //customG_->setShortcut(QKeySequence(tr("Ctrl+C", "shorcut for generating a custom graph")));
    QAction *actionSave = menuFichier->addAction(tr("&Save"));
    actionSave->setShortcut(QKeySequence(tr("Ctrl+S", "shortcut for saving")));
    QAction *actionQuitter = menuFichier->addAction(tr("&Quit"));
    actionQuitter->setShortcut(QKeySequence(tr("Ctrl+Q", "shortcut for quitting")));

    QMenu *menuLaunch = menuBar()->addMenu(tr("Launch"));
    QAction *actionPlay = menuLaunch->addAction(tr("&Step"));
    actionPlay->setShortcut(QKeySequence(tr("Ctrl+A", "shortcut for launching a new step of the algorithm")));

    QMenu *menuPlus = menuBar()->addMenu(tr("?"));
    QAction *actionQt = menuPlus->addAction(tr("About Qt"));
    //QAction *info = menuPlus->addAction(tr("About the graph"));

    //QObject::connect(newAction_, SIGNAL(triggered()), graph_, SLOT(newGraph()));
    //QObject::connect(customG_, SIGNAL(triggered()), graph_, SLOT(newCustomGraph()));
   // QObject::connect(testG_, SIGNAL(triggered()), graph_, SLOT(testGraph()));

    //QObject::connect(newAction_, SIGNAL(triggered()), this, SLOT(resetTimeLog()));
    //QObject::connect(customG_, SIGNAL(triggered()), this, SLOT(resetTimeLog()));
    //QObject::connect(testG_, SIGNAL(triggered()), this, SLOT(resetTimeLog()));

    QObject::connect(actionQuitter, SIGNAL(triggered()), qApp, SLOT(quit()));
    QObject::connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
    QObject::connect(actionQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    QObject::connect(actionPlay, SIGNAL(triggered()), this, SLOT(nextStep()));
    //QObject::connect(info, SIGNAL(triggered()), this, SLOT(information()));
}

void MainWindow::nextStep()
{
    Cell::nextStep();
}

#include <QFile>
#include <QTextStream>
#include <QFileDialog>

void MainWindow::save()
{
    QString fichier = QFileDialog::getSaveFileName(this, "Save a file", QString(), "Text(*.txt)");
    QFile file(fichier);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << scene->getWidth() <<" "<< scene->getHeigth();
    //iterator
    QList<Cell*>::iterator i;
    //for all cell
    for(i=Cell::cells.begin(); i!=Cell::cells.end(); i++)
    {
        if((*i)->getState()==wall)
        {
            out<< " "<<(*i)->getHorizontalPosition() <<" "<< (*i)->getVerticalPosition() ;
        }
    }

    // optional, as QFile destructor will already do it:
    file.close();
}
