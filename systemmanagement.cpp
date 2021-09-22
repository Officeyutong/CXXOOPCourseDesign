#include "SystemManagement.h"
#include <QDebug>
#include <QTabBar>
//#include <QWidgetItem>
#include <QComboBox>
#include <QMessageBox>
#include <QMetaObject>
#include <QModelIndex>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <algorithm>
#include "CourseEditForm.h"
#include "MyQSqlTableModel.h"
#include "ui_systemmanagement.h"
SystemManagement::SystemManagement(data_manager::GeneralDataManager* data,
                                   QWidget* parent)
    : QWidget(parent), ui(new Ui::SystemManagement) {
    //    using namespace system_data;
    this->data = data;
    ui->setupUi(this);
    // 禁止缩放
    this->setMaximumSize(this->size());
    this->setMinimumSize(this->size());
    ui->mainTab->setCurrentIndex(0);
    {
        studentModel = new MyQSqlTableModel(
            data->database, [](const auto& x) { return true; }, this);

        studentModel->setTable("student");
        studentModel->setEditStrategy(QSqlTableModel::OnFieldChange);
        studentModel->select();
        QStringList headers({"学号", "姓名", "班级", "专业"});
        for (int i = 0; i < headers.size(); i++) {
            studentModel->setHeaderData(i, Qt::Horizontal, headers[i]);
        }
        ui->studentTable->setModel(studentModel);
    }
    {
        teacherModel = new MyQSqlTableModel(
            data->database, [](const auto& x) { return true; }, this);

        teacherModel->setTable("teacher");
        teacherModel->setEditStrategy(QSqlTableModel::OnFieldChange);
        teacherModel->select();
        QStringList headers({"工号", "姓名"});
        for (int i = 0; i < headers.size(); i++) {
            teacherModel->setHeaderData(i, Qt::Horizontal, headers[i]);
        }
        ui->staffTable->setModel(teacherModel);
    }
    {
        courseModel = new MyQSqlTableModel(
            data->database, [](const auto& x) { return x != 4; }, this);

        courseModel->setTable("course");
        courseModel->setEditStrategy(QSqlTableModel::OnFieldChange);
        courseModel->select();

        QStringList headers(
            {"课程编号", "课程名称", "学分", "学时", "课程类别"});
        for (int i = 0; i < headers.size(); i++) {
            courseModel->setHeaderData(i, Qt::Horizontal, headers[i]);
        }
        ui->courseTable->setModel(courseModel);
    }
    {
        ui->teacherRankTable->setModel(&teachingRankModel);
        teachingRankModel.setHorizontalHeaderLabels(
            QStringList({"教师名", "教师ID", "平均分", "有效评价数"}));
        auto queryStr = QString(
            "SELECT "
            "teacher.name AS teacher_name, "
            "selected_course.teacher_id, "
            "AVG(selected_course.score) AS average_score, "
            "COUNT(selected_course.score) AS comment_count "
            "FROM "
            "selected_course "
            "JOIN teacher ON teacher.id = selected_course.teacher_id "
            "WHERE "
            "selected_course.comment IS NOT NULL "
            "AND "
            "selected_course.comment != '' "
            "GROUP BY selected_course.teacher_id "
            "ORDER BY average_score DESC ");
        qDebug() << queryStr;
        QSqlQuery query(queryStr, data->database);
        if (!query.exec()) {
            qDebug() << query.lastError();

        } else {
            while (query.next()) {
                teachingRankModel.appendRow(QList<QStandardItem*>(
                    {new QStandardItem(query.value("teacher_name").toString()),
                     new QStandardItem(query.value("teacher_id").toString()),
                     new QStandardItem(QString::number(
                         query.value("average_score").toDouble())),
                     new QStandardItem(QString::number(
                         query.value("comment_count").toUInt()))}));
            }
        }
    }
}

SystemManagement::~SystemManagement() {
    delete ui;
    delete studentModel;
    delete courseModel;
    delete teacherModel;
}

void SystemManagement::on_removeRowButton_clicked() {
    QTableView* tables[] = {ui->studentTable, ui->staffTable, ui->courseTable};
    QTableView* currTable = tables[ui->mainTab->currentIndex()];
    auto selectedRows = currTable->selectionModel()->selectedRows();
    if (selectedRows.size() != 1)
        return;
    auto currRow = selectedRows[0];
    currTable->model()->removeRows(currRow.row(), 1);
    dynamic_cast<QSqlTableModel*>(currTable->model())->select();
}

void SystemManagement::on_addRowButton_clicked() {
    QTableView* tables[] = {ui->studentTable, ui->staffTable, ui->courseTable};
    QTableView* currTable = tables[ui->mainTab->currentIndex()];
    auto currModel = dynamic_cast<QSqlTableModel*>(currTable->model());
    long long maxVal = 0;
    for (int i = 0; i < currModel->rowCount(); i++) {
        auto currVal =
            currModel->record(i).field(0).value().toString().toLongLong();
        maxVal = std::max(maxVal, currVal);
    }
    auto nextKey = QString("%1").arg(maxVal + 1);
    if (ui->mainTab->currentIndex() == 2) {
        QSqlQuery query("INSERT INTO course VALUES(?,?,?,?,?)");
        query.addBindValue(nextKey);
        query.addBindValue("新建课程");
        query.addBindValue(1);
        query.addBindValue(18);
        query.addBindValue("选修");
        SQL_EXEC_AND_LOG(query);
        courseModel->select();
        courseModel->selectRow(courseModel->rowCount() - 1);
        return;
    }
    QSqlRecord record = currModel->record();
    record.setValue(0, nextKey);
    switch (ui->mainTab->currentIndex()) {
        case 0: {
            record.setValue(1, "新建学生");
            record.setValue(2, "123456");
            record.setValue(3, "计算机");
            break;
        }
        case 1: {
            record.setValue(1, "新建教师");
            break;
        }
            //        case 2: {
            //            record.setValue(1, "新建课程");
            //            record.setValue(2, 1);
            //            record.setValue(3, 18);
            //            record.setValue(4, "选修");
            //            break;
            //        }
    }
    if (!currModel->insertRecord(-1, record)) {
        qDebug() << "Inserting error:" << currModel->lastError();
    } else {
        currModel->submitAll();
        currModel->select();
        currTable->selectRow(currTable->model()->rowCount() - 1);
    }
}

void SystemManagement::on_staffTable_doubleClicked(const QModelIndex& index) {
    auto row = index.row();
    auto teacher_id = teacherModel->record(row).field(0).value().toString();
    auto teacher_name = teacherModel->record(row).field(1).value().toString();

    auto window = new CourseEditForm(nullptr, teacher_id, teacher_name, data,
                                     [=]() { teacherModel->select(); });
    window->setWindowModality(Qt::ApplicationModal);
    window->show();
}

void SystemManagement::on_saveDataButton_clicked() {
    try {
        data->exportShit();
        QMessageBox::information(this, "信息", "保存成功!");
    } catch (const std::exception& ex) {
        QMessageBox::critical(this, "错误",
                              QString("保存失败: %1").arg(ex.what()));
    }
}

void SystemManagement::on_courseTable_doubleClicked(const QModelIndex& index) {
    if (index.column() != 4) {
        return;
    }
    auto row = index.row();
    auto id = courseModel->record(row).field(0).value().toString();
    auto nextState =
        courseModel->record(row).field(4).value().toString() == "选修" ? "必修"
                                                                       : "选修";
    QSqlQuery query("UPDATE course SET type = ? WHERE id = ?");
    query.addBindValue(nextState);
    query.addBindValue(id);
    SQL_EXEC_AND_LOG(query);
    courseModel->select();
    courseModel->selectRow(row);
}
