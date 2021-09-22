#include "AddSelectedCourseForm.h"
#include <QMessageBox>
#include <QSqlQuery>
#include "Utility.h"
#include "ui_AddSelectedCourseForm.h"
AddSelectedCourseForm::AddSelectedCourseForm(
    data_manager::GeneralDataManager* dataManager,
    double availableCredits,
    QString studentNumber,
    std::function<void(void)> finishCallback,
    QWidget* parent)
    : QWidget(parent),
      ui(new Ui::AddSelectedCourseForm),
      dataManager(dataManager),
      availableCredits(availableCredits),
      studentNumber(studentNumber),
      finishCallback(finishCallback) {
    ui->setupUi(this);
    setMaximumSize(size());
    setMinimumSize(size());
    ui->mainTable->setModel(&model);
    setWindowTitle(QString("选课 - 可用学分: %1 - 双击列表项进行选择")
                       .arg(availableCredits));
    model.setHorizontalHeaderLabels(QStringList(
        {"课程ID", "课程名", "学分", "类型", "教师ID", "授课老师"}));
    QString queryString =
        "SELECT "
        "teacher_course.teacher_id, "
        "teacher_course.course_id, "
        "teacher.name AS teacher_name, "
        "course.name AS course_name, "
        "course.type, "
        "course.credit "
        "FROM teacher_course "
        "JOIN teacher ON teacher.id = teacher_course.teacher_id "
        "JOIN course ON course.id = teacher_course.course_id "
        "WHERE teacher_course.course_id NOT IN("
        "SELECT selected_course.course_id "
        "FROM selected_course "
        "WHERE selected_course.student_id = ? "
        ")";
    MY_DEBUG << "query string: " << queryString;
    QSqlQuery query(queryString, dataManager->database);
    query.addBindValue(studentNumber);
    if (!query.exec()) {
        MY_DEBUG << query.lastError();
        QMessageBox::critical(
            this, "错误",
            QString("加载数据失败: %1").arg(query.lastError().text()));
        close();
    }
    while (query.next()) {
        model.appendRow(QList<QStandardItem*>(
            {new QStandardItem(query.value("course_id").toString()),
             new QStandardItem(query.value("course_name").toString()),
             new QStandardItem(
                 QString("%1").arg(query.value("credit").toDouble())),
             new QStandardItem(query.value("type").toString()),
             new QStandardItem(query.value("teacher_id").toString()),
             new QStandardItem(query.value("teacher_name").toString())}));
    }
    ui->mainTable->resizeColumnsToContents();
}

AddSelectedCourseForm::~AddSelectedCourseForm() {
    delete ui;
}

void AddSelectedCourseForm::on_mainTable_doubleClicked(
    const QModelIndex& index) {
    auto row = index.row();
    auto courseId = model.item(row, 0)->text();
    auto teacherId = model.item(row, 4)->text();
    auto credit = model.item(row, 2)->text().toDouble();
    if (credit > availableCredits) {
        QMessageBox::critical(this, "错误", "剩余学分不足");
        return;
    }
    QSqlQuery query(
        "INSERT INTO selected_course (student_id,course_id,teacher_id) VALUES "
        "(?,?,?)",
        dataManager->database);
    query.addBindValue(studentNumber);
    query.addBindValue(courseId);
    query.addBindValue(teacherId);
    if (!query.exec()) {
        MY_DEBUG << query.lastError();
        QMessageBox::critical(
            this, "错误",
            QString("更新数据时出现错误: %1").arg(query.lastError().text()));
        return;
    }
    finishCallback();
    close();
}
