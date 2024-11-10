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

    emit moveSignal(tempVec, point);

    ui->m_xCoordinate->clear();
    ui->m_yCoordinate->clear();
    ui->m_zCoordinate->clear();

    close();
}


void MoveClass::pointData(const QMap<int, QString> point)
{
    ui->m_pointBox->clear();

    for (auto iterator = point.constBegin(); iterator != point.constEnd(); ++iterator) {
        if (!iterator.value().isEmpty()) {
            ui->m_pointBox->addItem(iterator.value(), QVariant(iterator.key()));
        }
    }



    connect(ui->m_pointBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MoveClass::currentPoint);
}

void MoveClass::currentPoint(int index)
{
    point = ui->m_pointBox->itemData(index).toInt();
}

void MoveClass::cancelBtn()
{
    ui->m_xCoordinate->clear();
    ui->m_yCoordinate->clear();
    ui->m_zCoordinate->clear();

    close();
}
