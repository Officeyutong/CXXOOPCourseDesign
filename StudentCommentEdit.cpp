#include "StudentCommentEdit.h"
#include <QMessageBox>
#include "ui_StudentCommentEdit.h"
StudentCommentEdit::StudentCommentEdit(QString studentID,
                                       QString courseID,
                                       std::function<void()> callback,
                                       data_manager::GeneralDataManager* data,
                                       QWidget* parent)
    : QWidget(parent),
      ui(new Ui::StudentCommentEdit),
      studentID(studentID),
      courseID(courseID),
      callback(callback),
      data(data) {
    ui->setupUi(this);
    this->setMaximumSize(this->size());
    this->setMinimumSize(this->size());
    QSqlQuery query(
        "SELECT comment,score FROM selected_course WHERE "
        "student_id = ? AND course_id = ?",
        this->data->database);
    query.addBindValue(studentID);
    query.addBindValue(courseID);
    if (!query.exec()) {
        qDebug() << query.lastError();
        return;
    }
    if (query.first()) {
        ui->scoreSpin->setValue(query.value("score").toInt());
        ui->commentEdit->setText(query.value("comment").toString());
    }
}

StudentCommentEdit::~StudentCommentEdit() {
    delete ui;
}

void StudentCommentEdit::on_pushButton_2_clicked() {
    this->close();
}

void StudentCommentEdit::on_pushButton_clicked() {
    if (ui->commentEdit->toPlainText().length() <= 0) {
        QMessageBox::critical(this, "错误", "请输入评价内容");
        return;
    }
    QSqlQuery query(
        "UPDATE selected_course SET comment = ?, score = ? WHERE student_id = "
        "? AND course_id = ?",
        data->database);
    query.addBindValue(ui->commentEdit->toPlainText());
    query.addBindValue(ui->scoreSpin->value());
    query.addBindValue(studentID);
    query.addBindValue(courseID);
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", query.lastError().text());
        qDebug() << query.lastError();
        return;
    }
    callback();
    this->close();
}
