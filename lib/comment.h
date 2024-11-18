#ifndef COMMENT_H
#define COMMENT_H

#include <QDialog>

    namespace Ui {
class Comment;
}

class Comment : public QDialog
{
    Q_OBJECT

public:
    explicit Comment(QWidget *parent = nullptr);
    ~Comment();

private:
    Ui::Comment *ui;


public:
    void acceptBtn();
    void cancelBtn();

signals:
    void commentText(const QString &text, const bool &trash);
};

#endif // COMMENT_H
