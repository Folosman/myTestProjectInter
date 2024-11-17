#include "./lib/placeform.h"
#include "ui_placeform.h"

PlaceForm::PlaceForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaceForm)
{
    ui->setupUi(this);
    connect(ui->m_okBtn, &QPushButton::clicked, this, &PlaceForm::okBtn);
}

PlaceForm::~PlaceForm()
{
    delete ui;
}

void PlaceForm::pointData(const QMap<int, QString> pointName)
{
    ui->m_firstPoint->clear();
    ui->m_secondPoint->clear();
    ui->m_threedPoint->clear();

    for (auto it = pointName.constBegin(); it != pointName.constEnd(); ++it) {
        if (!it.value().isEmpty()) {
            ui->m_firstPoint->addItem(it.value(), QVariant(it.key()));
            ui->m_secondPoint->addItem(it.value(), QVariant(it.key()));
            ui->m_threedPoint->addItem(it.value(), QVariant(it.key()));
        }
    }

    firstPoint = ui->m_firstPoint->itemData(0).toInt();
    secondPoint = ui->m_secondPoint->itemData(0).toInt();
    threedPoint = ui->m_threedPoint->itemData(0).toInt();

    connect(ui->m_firstPoint, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlaceForm::currentFirstPoint);

    connect(ui->m_secondPoint, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlaceForm::currentSecondePoint);
    connect(ui->m_threedPoint, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlaceForm::currentThreedPoint);
}

void PlaceForm::currentFirstPoint(int index)
{
    firstPoint = ui->m_firstPoint->itemData(index).toInt();
}

void PlaceForm::currentSecondePoint(int index)
{
    secondPoint = ui->m_secondPoint->itemData(index).toInt();
}

void PlaceForm::currentThreedPoint(int index)
{
    threedPoint = ui->m_threedPoint->itemData(index).toInt();
}

void PlaceForm::okBtn()
{
    m_indexPoint = {firstPoint, secondPoint, threedPoint};
    emit sendPoint(m_indexPoint);
    close();
}
