#include "./lib/settingwidget.h"
#include "ui_settingwidget.h"

SettingWidget::SettingWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingWidget)
{
    ui->setupUi(this);
    connect(ui->m_okBtn, &QPushButton::clicked, this, &SettingWidget::okBtn);
    connect(ui->m_cancelBtn, &QPushButton::clicked, this, &SettingWidget::cancelBtn);
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::okBtn()
{
    emit errorValue(ui->m_errorEdit->text().toFloat());
    ui->m_errorEdit->clear();
    close();
}

void SettingWidget::cancelBtn()
{
    ui->m_errorEdit->clear();
    close();
}
