#include "./lib/moveclass.h"
#include <QVector3D>
#include "ui_moveclass.h"

MoveClass::MoveClass(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MoveClass)
{
    ui->setupUi(this);

    connect(ui->m_okBtn, &QPushButton::clicked, this, &MoveClass::okBtn);
    connect(ui->m_cancelBtn, &QPushButton::clicked, this, &MoveClass::cancelBtn);
}

MoveClass::~MoveClass()
{
    delete ui;
}


void MoveClass::okBtn() {

    QVector3D tempVec(ui->m_xCoordinate->text().toFloat(),
                      ui->m_yCoordinate->text().toFloat(),
                      ui->m_zCoordinate->text().toFloat());

    emit moveSignal(tempVec);

    ui->m_xCoordinate->clear();
    ui->m_yCoordinate->clear();
    ui->m_zCoordinate->clear();

    close();
}


void MoveClass::pointData(const QMap<int, QString> point)
{

}

void MoveClass::currentPoint(int index)
{

}

void MoveClass::cancelBtn()
{
    ui->m_xCoordinate->clear();
    ui->m_yCoordinate->clear();
    ui->m_zCoordinate->clear();

    close();
}
