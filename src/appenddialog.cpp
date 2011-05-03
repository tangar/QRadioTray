#include "appenddialog.h"
#include "ui_appenddialog.h"

AppendDialog::AppendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppendDialog)
{
    ui->setupUi(this);
}

AppendDialog::~AppendDialog()
{
    delete ui;
}

void AppendDialog::updateStation()
{
    station.name = ui->nameField->text();
    station.url = ui->urlField->text();
    station.description = ui->descriptionField->text();

    switch(ui->encodingBox->currentIndex())
    {
    case 0:
        station.encoding = "UTF-8";
        break;
    case 1:
        station.encoding = "Windows-1251";
        break;
    default:
        station.encoding = "UTF-8";
    }

    this->accept();
}

void AppendDialog::init(Station *stationPtr)
{
    ui->nameField->setText(stationPtr->name);
    ui->descriptionField->setText(stationPtr->description);
    ui->urlField->setText(stationPtr->url);

    if (stationPtr->encoding == "UTF-8")
        ui->encodingBox->setCurrentIndex(0);
    else if (stationPtr->encoding == "Windows-1251" )
        ui->encodingBox->setCurrentIndex(1);
    else
        ui->encodingBox->setCurrentIndex(0);
}
