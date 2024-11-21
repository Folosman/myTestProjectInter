#include "./lib/mainwindow.h"
#include "./lib/comment.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Comment comment;

    QObject::connect(&comment, &Comment::commentText, &w, &MainWindow::commentSlot);


    w.setWindowTitle("Интерпритатор");
    // w.setMaximumSize(800, 600);
    w.setMinimumSize(800, 600);
    w.show();
    return a.exec();
}
