#ifndef APPENDDIALOG_H
#define APPENDDIALOG_H

#include <QDialog>
#include "Station.h"

namespace Ui {
    class AppendDialog;
}

class AppendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AppendDialog(QWidget *parent = 0);
    ~AppendDialog();

    Station station;

    void init(Station * stationPtr);

public slots:
    void updateStation();

private:
    Ui::AppendDialog *ui;
};

#endif // APPENDDIALOG_H
