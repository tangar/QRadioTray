#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSystemTrayIcon trayItem;
    QMenu trayMenu;

private:
    Ui::MainWindow *ui;

//public slots:
//    void processAction(QAction *action);
};

#endif // MAINWINDOW_H
