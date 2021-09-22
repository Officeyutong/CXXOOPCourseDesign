#include "StudentManagement.h"
#include <QKeyEvent>
#include <QLayout>
#include <QMessageBox>
#include <QSet>
#include <QSqlField>
#include <QSqlRecord>
#include <QTabBar>
#include "AddSelectedCourseForm.h"
#include "StudentCommentEdit.h"
#include "Utility.h"
#include "ui_StudentManagement.h"
StudentManagement::StudentManagement(
    data_manager::GeneralDataManager* dataManager,
    QWidget* parent)
    : QWidget(parent), ui(new Ui::StudentManagement), dataManager(dataManager) {
    ui->setupUi(this);
    this->setMaximumSize(this->size());
    this->setMinimumSize(this->size());
    ui->mainTab->tabBar()->hide();
    {
        detailModel = new QStandardItemModel;
        detailModel->setHorizontalHeaderLabels(
            QStringList({"课程ID", "课程名", "教师名", "课程类型", "学分",
                         "分数", "", ""}));
        ui->studentDetailTable->setModel(detailModel);
        ui->studentDetailTable->setEditTriggers(
            QAbstractItemView::NoEditTriggers);
        ui->studentDetailTable->horizontalHeader()->setSectionResizeMode(
            QHeaderView::Stretch);
    }

    {
        listModel = new QSqlTableModel(this, dataManager->database);
        listModel->setTable("student");
        ui->studentListTable->setEditTriggers(
            QAbstractItemView::NoEditTriggers);
        ui->studentListTable->setModel(listModel);
        ui->studentListTable->setColumnHidden(2, true);
        ui->studentListTable->setColumnHidden(3, true);
        listModel->setHeaderData(0, Qt::Horizontal, "学号");
        listModel->setHeaderData(1, Qt::Horizontal, "姓名");
        listModel->select();
    }

    ui->studentEdit->installEventFilter(this);
    ui->mainTab->setCurrentIndex(0);
    handleStudentSearch();
}

StudentManagement::~StudentManagement() {
    delete ui;
    delete listModel;
    delete detailModel;
}

bool StudentManagement::eventFilter(QObject* target, QEvent* event) {
    if (target == ui->studentEdit && event->type() == QEvent::KeyPress) {
        auto keyEvent = dynamic_cast<QKeyEvent*>(event);
        qDebug() << "handling event";
        if (keyEvent->key() == Qt::Key_Return) {
            handleStudentSearch();
        }
    }
    return QWidget::eventFilter(target, event);
}

void StudentManagement::handleStudentSearch() {
    auto text = ui->studentEdit->text();
    auto filter =
        QString("id LIKE '%%1%' OR name LIKE '%%2%'").arg(text).arg(text);
    qDebug() << "search " << filter;
    listModel->setFilter(filter);
    if (!listModel->select()) {
        qDebug() << listModel->lastError();
    }
}

void StudentManagement::on_studentListTable_doubleClicked(
    const QModelIndex& index) {
    auto row = index.row();
    this->currentStudentNumber =
        listModel->record(row).field(0).value().toString();

    ui->mainTab->setCurrentIndex(1);
    refreshStudentTable(this->currentStudentNumber);
}

void StudentManagement::on_pushButton_clicked() {
    ui->mainTab->setCurrentIndex(0);
}

void StudentManagement::refreshStudentTable(const QString& studentNumber) {
    QString queryString =
        "SELECT "
        "selected_course.student_id ,"
        "selected_course.course_id, "
        "selected_course.teacher_id, "
        "teacher.name AS teacher_name, "
        "course.name AS course_name, "
        "course.type AS course_type, "
        "course.credit, "
        "score.score "
        "FROM selected_course "
        "JOIN teacher "
        "ON teacher.id = selected_course.teacher_id "
        "JOIN course "
        "ON course.id = selected_course.course_id "
        "LEFT JOIN score "
        "ON "
        "(score.student_id = selected_course.student_id "
        "AND "
        "score.course_id = selected_course.course_id) "
        "WHERE selected_course.student_id = ?";
    qDebug() << queryString;
    QSqlQuery query(queryString);
    query.addBindValue(studentNumber);
    if (!query.exec()) {
        MY_DEBUG << query.lastError();
        QMessageBox::critical(
            this, "错误",
            QString("刷新数据失败: %1").arg(query.lastError().text()));
        return;
    }
    detailModel->setRowCount(0);
    double& totalCredit = this->totalCredit;
    totalCredit = 0;
    while (query.next()) {
        auto score = query.value("score");
        qDebug() << "score=" << score;
        auto credit = query.value("credit").toDouble();
        auto teacher_id = query.value("teacher_id").toString();
        auto course_id = query.value("course_id").toString();
        detailModel->appendRow(QList<QStandardItem*>(
            {new QStandardItem(course_id),
             new QStandardItem(query.value("course_name").toString()),
             new QStandardItem(query.value("teacher_name").toString()),
             new QStandardItem(query.value("course_type").toString()),
             new QStandardItem(QString("%1").arg(credit)),
             new QStandardItem(score.isNull()
                                   ? "未批阅"
                                   : QString("%1").arg(score.toInt()))}));
        totalCredit += credit;
        {
            QPushButton* button = new QPushButton;
            button->setText("退选");
            connect(button, &QPushButton::clicked, [=]() {
                QSqlQuery query(
                    "DELETE FROM selected_course WHERE student_id = ? AND "
                    "course_id = ? AND teacher_id = ?");
                query.addBindValue(studentNumber);
                query.addBindValue(course_id);
                query.addBindValue(teacher_id);
                if (!query.exec()) {
                    MY_DEBUG << query.lastError();
                    QMessageBox::critical(
                        this, "错误",
                        QString("操作失败: %1").arg(query.lastError().text()));
                    return;
                } else {
                    refreshStudentTable(studentNumber);
                }
            });
            ui->studentDetailTable->setIndexWidget(
                detailModel->index(detailModel->rowCount() - 1, 6), button);
        }
        {
            QPushButton* button = new QPushButton;
            button->setText("评价");
            connect(button, &QPushButton::clicked, [=]() {
                auto window = new StudentCommentEdit(
                    studentNumber, course_id,
                    [&]() { refreshStudentTable(studentNumber); }, dataManager);
                window->setWindowModality(Qt::ApplicationModal);
                window->show();
            });
            ui->studentDetailTable->setIndexWidget(
                detailModel->index(detailModel->rowCount() - 1, 7), button);
        }
    }
    ui->totalCreditLabel->setText(QString("%1").arg(totalCredit));
    ui->availableCreditLabel->setText(QString("%1").arg(50 - totalCredit));
}

void StudentManagement::on_addCourseButton_clicked() {
    if (totalCredit >= 50) {
        QMessageBox::critical(this, "错误", "你已经选择了50学分的课程");
        return;
    }
    auto window = new AddSelectedCourseForm(
        dataManager, 50 - totalCredit, currentStudentNumber,
        [=]() { refreshStudentTable(currentStudentNumber); });
    window->setWindowModality(Qt::ApplicationModal);
    window->show();
}
