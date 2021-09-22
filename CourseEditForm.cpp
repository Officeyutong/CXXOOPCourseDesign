#include "CourseEditForm.h"
#include <QCheckBox>
#include <QMessageBox>
#include <QPushButton>
#include <QSet>
#include <QString>
#include <QVariantList>
#include <functional>
#include "Utility.h"
#include "ui_CourseEditForm.h"
CourseEditForm::CourseEditForm(QWidget* parent,
                               const QString& teacher_id,
                               const QString& teacher_name,
                               data_manager::GeneralDataManager* dataManager,
                               std::function<void(void)> finishCallback)
    : QWidget(parent),
      ui(new Ui::CourseEditForm),
      dataManager(dataManager),
      teacher_id(teacher_id),
      finishCallback(finishCallback) {
    ui->setupUi(this);
    this->setMaximumSize(this->size());
    this->setMinimumSize(this->size());
    model = new QStandardItemModel(0, 3);
    model->setHorizontalHeaderLabels(QStringList({"课程ID", "课程名", "选中"}));
    ui->dataTable->setModel(model);
    ui->dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->teacher_id = teacher_id;
    ui->idEdit->setText(teacher_id);
    ui->nameEdit->setText(teacher_name);

    QSet<QString> selected;
    {
        QSqlQuery query(
            "SELECT course_id FROM teacher_course WHERE teacher_id=?");
        query.addBindValue(teacher_id);
        SQL_EXEC_AND_LOG(query);
        while (query.next()) {
            selected.insert(query.value(0).toString());
        }
    }
    {
        QSqlQuery query("SELECT id,name FROM course");
        SQL_EXEC_AND_LOG(query);
        while (query.next()) {
            bool checked = selected.contains(query.value(0).toString());
            auto dataItem = new QStandardItem(checked ? "选中" : "未选");
            model->appendRow(QList<QStandardItem*>(
                {new QStandardItem(query.value(0).toString()),
                 new QStandardItem(query.value(1).toString()), dataItem}));
            //            QCheckBox* checkBox = new QCheckBox("是否选中");
            QPushButton* button = new QPushButton;
            //            checkBox->setChecked(checked);
            button->setText(checked ? "取消选中" : "选中");
            connect(button, &QPushButton::clicked, [=]() {
                auto newState = (dataItem->text() != "选中");
                qDebug() << "new state" << newState;
                dataItem->setText(newState ? "选中" : "未选");
                button->setText(newState ? QString("取消选中")
                                         : QString("选中"));
            });
            ui->dataTable->setIndexWidget(
                model->index(model->rowCount() - 1, 2), button);
        }
        ui->dataTable->resizeColumnsToContents();
    }
}

CourseEditForm::~CourseEditForm() {
    delete ui;
}

void CourseEditForm::on_cancelButton_clicked() {
    close();
}

void CourseEditForm::on_saveButton_clicked() {
    {
        QSqlQuery query("UPDATE teacher SET id = ?, name = ? WHERE id = ?",
                        dataManager->database);
        query.addBindValue(ui->idEdit->text());
        query.addBindValue(ui->nameEdit->text());
        query.addBindValue(teacher_id);
        if (!query.exec()) {
            MY_DEBUG << query.lastError();
            QMessageBox::critical(
                this, "错误",
                QString("更新失败: ").arg(query.lastError().databaseText()));
            return;
        }
    }
    {
        QSqlQuery query("DELETE FROM teacher_course WHERE teacher_id = ?",
                        dataManager->database);
        query.addBindValue(teacher_id);
        SQL_EXEC_AND_LOG(query);
    }
    {
        QSqlQuery query(dataManager->database);
        query.prepare("INSERT INTO teacher_course VALUES (?,?)");
        QVariantList teachers, courses;
        for (int i = 0; i < model->rowCount(); i++) {
            auto course = model->item(i, 0)->text();
            auto checked = model->item(i, 2)->text() == "选中";
            if (checked) {
                teachers << teacher_id;
                courses << course;
                qDebug() << "select " << teacher_id << "," << course;
            }
        }
        query.addBindValue(teachers);
        query.addBindValue(courses);
        if (!query.execBatch()) {
            QMessageBox::critical(this, "错误", "更新数据失败!");
            qDebug() << query.lastError();
        } else {
            close();
        }
    }
    finishCallback();
}
