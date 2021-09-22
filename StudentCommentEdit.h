#ifndef STUDENTCOMMENTEDIT_H
#define STUDENTCOMMENTEDIT_H

#include <QWidget>
#include <functional>
#include "GeneralDataManager.h"
namespace Ui {
class StudentCommentEdit;
}

class StudentCommentEdit : public QWidget {
    Q_OBJECT

   public:
    explicit StudentCommentEdit(QString studentID,
                                QString courseID,
                                std::function<void()> callback,
                                data_manager::GeneralDataManager* data,
                                QWidget* parent = nullptr);
    ~StudentCommentEdit();

   private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

   private:
    Ui::StudentCommentEdit* ui;
    QString studentID, courseID;
    std::function<void()> callback;
    data_manager::GeneralDataManager* data;
};

#endif  // STUDENTCOMMENTEDIT_H
