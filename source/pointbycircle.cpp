#include "./lib/pointbycircle.h"
#include "ui_pointbycircle.h"

PointByCircle::PointByCircle(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PointByCircle)
{
    ui->setupUi(this);
}

PointByCircle::~PointByCircle()
{
    delete ui;
}

