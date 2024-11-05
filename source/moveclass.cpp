#include "./lib/moveclass.h"
#include <QVector3D>
#include "ui_moveclass.h"

MoveClass::MoveClass(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MoveClass)
{
    ui->setupUi(this);

    connect(ui->m_okBtn, &QPushButton::clicked, this, &MoveClass::okBtn);
}

MoveClass::~MoveClass()
{
    delete ui;
}


void MoveClass::okBtn() {
    // Создаем вектор смещения из введенных значений координат
    QVector3D tempVec(ui->m_xCoordinate->text().toFloat(),
                      ui->m_yCoordinate->text().toFloat(),
                      ui->m_zCoordinate->text().toFloat());

    // Отправляем сигнал с вектором смещения
    emit moveSignal(tempVec);

    // Очищаем поля ввода после отправки сигнала
    ui->m_xCoordinate->clear();
    ui->m_yCoordinate->clear();
    ui->m_zCoordinate->clear();

    // Закрываем окно
    close();
}
