#include "./lib/comment.h"
#include "ui_comment.h"
// #include "./lib/mainwindow.h"


Comment::Comment(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Comment)
{
    ui->setupUi(this);

    // MainWindow mainWindow;
    // QObject::connect(this, &Comment::commentText, &mainWindow, &MainWindow::commentSlot);

    connect(ui->m_accpetBtn, &QPushButton::clicked, this, &Comment::acceptBtn);
    connect(ui->m_cancelBtn, &QPushButton::clicked, this, &Comment::cancelBtn);
}

Comment::~Comment()
{
    delete ui;
}

void Comment::acceptBtn()
{
    emit commentText(ui->m_commentLine->text());
    qDebug() << ui->m_commentLine->text();
    close();
}

void Comment::cancelBtn()
{
    close();
}
