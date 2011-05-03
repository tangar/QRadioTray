#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

#include "Station.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    QSettings * set;
    QList <Station *> stationList;

    void loadSettings(QSettings *set);

public slots:
    // заполнение таблицы из списка станций
    void popullateTable(void);

    void removeRecord(void);
    void moveUp(void);
    void moveDown(void);
    void appendRecord(void);
    void editRecord(void);
private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
