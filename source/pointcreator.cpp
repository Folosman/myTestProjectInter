#include "./lib/pointcreator.h"
#include "ui_pointcreator.h"

PointCreator::PointCreator(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PointCreator)
{
    ui->setupUi(this);
    connect(ui->m_okBtn, &QPushButton::clicked, this, &PointCreator::saveCoordinate);
    connect(ui->m_cancelBtn, &QPushButton::clicked, this, &PointCreator::cancelBtn);
}

PointCreator::~PointCreator()
{
    delete ui;
}

void PointCreator::saveCoordinate() {
    // Получаем имя точки
    QString name = ui->m_pointName->text();

    // Создаем вектор нормали на основе выбранной оси
    QVector3D normale;
    if (ui->m_xNormale->isChecked())
        normale = QVector3D(1, 0, 0);
    else if (ui->m_yNormale->isChecked())
        normale = QVector3D(0, 1, 0);
    else if (ui->m_zNormale_2->isChecked())
        normale = QVector3D(0, 0, 1);

    // Создаем вектор координат на основе введенных значений
    QVector3D tempVec(ui->m_xCoordinate->text().toFloat(),
                      ui->m_yCoordinate->text().toFloat(),
                      ui->m_zCoordinate->text().toFloat());

    // Отправляем сигнал с именем, координатами и нормалью
    emit coordinatSender(name, tempVec, normale);

    // Очищаем поля ввода после сохранения
    ui->m_xCoordinate->clear();
    ui->m_yCoordinate->clear();
    ui->m_zCoordinate->clear();
    ui->m_pointName->clear();

    // Закрываем окно
    close();
}

void PointCreator::cancelBtn()
{
    ui->m_xCoordinate->clear();
    ui->m_yCoordinate->clear();
    ui->m_zCoordinate->clear();

    ui->m_pointName->clear();
    close();
}
