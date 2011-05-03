#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //this->setWindowFlags(Qt::FramelessWindowHint);

//    if (!QSystemTrayIcon::isSystemTrayAvailable()) return;

//        trayMenu.addMenu(QIcon(":/images/radio_16.png"), "Stations");

//        trayMenu.addSeparator();

//        QAction * act = new QAction(this);
//        act->setIcon(QIcon(":/images/settings_16.png"));
//        act->setText(tr("Settings"));
//        act->setProperty("destination", QVariant("settings"));
//        trayMenu.addAction(act);

//        QAction * act2 = new QAction(this);
//        act2->setIcon(QIcon(":/images/close_16.png"));
//        act2->setText(tr("Exit"));
//        act2->setProperty("destination", QVariant("nowhere"));
//        trayMenu.addAction(act2);


//    trayItem.setContextMenu(&trayMenu);
//    trayItem.setIcon(QIcon(":/images/radio_16.png"));

//    trayItem.show();

//    connect(trayItem.contextMenu(), SIGNAL(triggered(QAction*)), this, SLOT(processAction(QAction*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::processAction(QAction *action)
//{
//    qDebug() << "From" << action->text() << "to" << action->property("destination").toString();
//}
