#include "./lib/pointonplace.h"
#include "ui_pointonplace.h"

PointOnPlace::PointOnPlace(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PointOnPlace)
{
    ui->setupUi(this);

    connect(ui->m_okBtn,        &QPushButton::clicked, this, &PointOnPlace::okBtn);
    connect(ui->m_cancelBtn,    &QPushButton::clicked, this, &PointOnPlace::cancelBtn);
}

PointOnPlace::~PointOnPlace()
{
    delete ui;
}

void PointOnPlace::pointAndPlace(const QMap<int, QString> point, const QMap<int, QString> place)
{

    ui->m_placeBox->clear();
    ui->m_pointBox->clear();

            /*      Я честно не понимаю, почему и как это работает      */
    for (auto it = point.constBegin(); it != point.constEnd(); ++it) {
        if (!it.value().isEmpty()) {
            ui->m_pointBox->addItem(it.value(), QVariant(it.key()));
        }
    }

    for (auto it = place.constBegin(); it != place.constEnd(); ++it) {
        if (!it.value().isEmpty()) {
            ui->m_placeBox->addItem(it.value(), QVariant(it.key()));
        }
    }

    m_pointName = point;
    m_placeName = place;
    connect(ui->m_pointBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PointOnPlace::currentPointSlot);

    connect(ui->m_placeBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PointOnPlace::currentPlaceSlot);
}

void PointOnPlace::currentPointSlot(int index)
{
    m_pointIndex = ui->m_pointBox->itemData(index).toInt();
}

void PointOnPlace::currentPlaceSlot(int index)
{
    m_placeIndex = ui->m_placeBox->itemData(index).toInt();
}


void PointOnPlace::okBtn()
{
    emit pointOnPlace(m_pointIndex, m_placeIndex);
    close();
}

void PointOnPlace::cancelBtn()
{
    close();
}
