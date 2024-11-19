#include "./lib/circlebypoints.h"
#include "ui_circlebypoints.h"
#include "QMessageBox"
CircleByPoints::CircleByPoints(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CircleByPoints)
{
    ui->setupUi(this);
    connect(ui->m_okBtn, &QPushButton::clicked, this, &CircleByPoints::okBtn);
    connect(ui->m_cancelBtn, &QPushButton::clicked, this, &CircleByPoints::cancelBtn);
}

CircleByPoints::~CircleByPoints()
{
    delete ui;
}

void CircleByPoints::circleByPoint(QMap<int, QString> pointName)
{
    ui->m_point1->clear();
    ui->m_point2->clear();
    ui->m_point3->clear();

    for (auto it = pointName.constBegin(); it != pointName.constEnd(); ++it) {
        if (!it.value().isEmpty()) {
            ui->m_point1->addItem(it.value(), QVariant(it.key()));
            ui->m_point2->addItem(it.value(), QVariant(it.key()));
            ui->m_point3->addItem(it.value(), QVariant(it.key()));
        }
    }

    firstPoint = ui->m_point1->itemData(0).toInt();
    secondePoint = ui->m_point1->itemData(0).toInt();
    threedPoint = ui->m_point1->itemData(0).toInt();

    connect(ui->m_point1, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CircleByPoints::currentFirstPoint);

    connect(ui->m_point2, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CircleByPoints::currentSecondPoint);
    connect(ui->m_point3, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CircleByPoints::currentThreedPoint);
}

void CircleByPoints::currentFirstPoint(int index)
{
    firstPoint = ui->m_point1->itemData(index).toInt();
}

void CircleByPoints::currentSecondPoint(int index)
{
    secondePoint = ui->m_point2->itemData(index).toInt();
}

void CircleByPoints::currentThreedPoint(int index)
{
    threedPoint = ui->m_point3->itemData(index).toInt();
}

void CircleByPoints::okBtn()
{
    if(/*ui->m_radius->text().toFloat() <=0*/false)
    {
        // QMessageBox::critical(
        //     nullptr,
        //     "Ошибка",
        //     "Неверное значение радиуса.",
        //     QMessageBox::Ok
        //     );
        // return;
    }
    else
    {
        m_indexPoint = {firstPoint, secondePoint, threedPoint};
        emit sendPoint(m_indexPoint);
        close();
    }
}

void CircleByPoints::cancelBtn()
{
    close();
}
