#include "TeacherManagement.h"
#include <QInputDialog>
#include <QMessageBox>
#include <cmath>
#include "Utility.h"
#include "ui_TeacherManagement.h"
TeacherManagement::TeacherManagement(
    data_manager::GeneralDataManager* dataManager,
    QWidget* parent)
    : QWidget(parent), ui(new Ui::TeacherManagement), dataManager(dataManager) {
    ui->setupUi(this);
    this->setMaximumSize(this->size());
    this->setMinimumSize(this->size());
    refreshData();
}

TeacherManagement::~TeacherManagement() {
    delete ui;
}

void TeacherManagement::refreshData() {
    model.clear();
    auto studentText = ui->studentSearch->text();
    auto classText = ui->classSearch->currentText();
    auto courseText = ui->courseSearch->currentText();
    auto teacherText = ui->teacherSearch->text();
    bool ok = true;
    int minScore =
        ui->minScore->text().size() != 0 ? ui->minScore->text().toInt(&ok) : 0;
    if (!ok) {
        QMessageBox::critical(this, "错误", "请输入合法的最低分");
        return;
    }
    ok = true;
    int maxScore = ui->maxScore->text().size() != 0
                       ? ui->maxScore->text().toInt(&ok)
                       : 100;
    if (!ok) {
        QMessageBox::critical(this, "错误", "请输入合法的最高分");
        return;
    }
    qDebug() << "student search=" << studentText;
    qDebug() << "class search=" << classText;
    qDebug() << "course search=" << courseText;
    qDebug() << "min score=" << minScore;
    qDebug() << "max score=" << maxScore;

    model.setHorizontalHeaderLabels(
        QStringList({"学号", "姓名", "班级", "课程ID", "课程名", "分数",
                     "授课教师", "学生评价"}));
    const auto wrapLike = [](const QString& s) {
        return QString("%%1%").arg(s);
    };
    bool searchTeacher = teacherText.trimmed() != "";
    QSqlQuery query(
        QString(
            "SELECT "
            "score.student_id, "
            "score.course_id, "
            "score.score, "
            "student.name, "
            "student.class_id, "
            "course.name AS course_name, "
            "selected_course.score AS student_comment_score, "
            "selected_course.comment AS student_comment,"
            "teacher.name AS teacher_name "
            "FROM score "
            "JOIN student ON student.id = score.student_id "
            "JOIN course ON course.id  = score.course_id "
            "LEFT JOIN selected_course ON selected_course.student_id = "
            "score.student_id AND selected_course.course_id = score.course_id "
            "LEFT JOIN teacher ON teacher.id = selected_course.teacher_id "
            "WHERE "
            "(student.name LIKE ? OR score.student_id LIKE "
            "?) "  //筛选姓名或学号
            "AND "
            "(score.score >= ? AND score.score <= ?) "  // 筛选分数
            "AND "
            "(student.class_id LIKE ?) "  //筛选班号
            "AND "
            "(course.name LIKE ?) "  // 筛选课程名
            )
            .append(searchTeacher ? QString("AND "
                                            "(teacher_name LIKE ?) ")
                                        .arg(wrapLike(teacherText))
                                  : "")
            .append(ui->willSort->checkState() == Qt::Checked
                        ? " ORDER BY score.score DESC"
                        : ""),
        this->dataManager->database);

    qDebug() << "student search: " << wrapLike(studentText);
    query.bindValue(0, wrapLike(studentText));
    query.bindValue(1, wrapLike(studentText));
    query.bindValue(2, minScore);
    query.bindValue(3, maxScore);
    query.bindValue(4, wrapLike(classText));
    query.bindValue(5, wrapLike(courseText));
    if (searchTeacher)
        query.bindValue(6, wrapLike(teacherText));
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", query.lastError().driverText());
        qDebug() << query.lastError();
        return;
    }
    ui->mainTable->setModel(&model);
    while (query.next()) {
        auto student_id = query.value("student_id").toString();
        auto course_id = query.value("course_id").toString();
        model.appendRow(QList<QStandardItem*>{
            new QStandardItem(student_id),
            new QStandardItem(query.value("name").toString()),
            new QStandardItem(query.value("class_id").toString()),
            new QStandardItem(course_id),
            new QStandardItem(query.value("course_name").toString()),
            new QStandardItem(
                QString::number(query.value("score").toLongLong())),
            new QStandardItem(query.value("teacher_name").isNull()
                                  ? "未知"
                                  : query.value("teacher_name").toString()),
            new QStandardItem("")});
        auto comment = query.value("student_comment");
        auto student_score = query.value("student_comment_score");
        //        break;
        if ((!comment.isNull()) && comment.toString().length() > 0) {
            //            qDebug() <<' comment << " " << student_score;

            //            qDebug() << "creating button..";
            QPushButton* button = new QPushButton;
            button->setText("查看评价");
            connect(button, &QPushButton::clicked, [=]() {
                QMessageBox::information(this, "评价详情",
                                         QString("评价分数: %1\n评价内容:\n%2")
                                             .arg(student_score.toInt())
                                             .arg(comment.toString()));
            });

            ui->mainTable->setIndexWidget(model.index(model.rowCount() - 1, 7),
                                          button);
        }
    }

    {
        auto curr = ui->classSearch->currentText();
        ui->classSearch->clear();
        ui->classSearch->addItems(loadClassList());
        ui->classSearch->setCurrentText(curr);
    }
    {
        auto curr = ui->courseSearch->currentText();
        ui->courseSearch->clear();
        ui->courseSearch->addItems(loadCourseList());
        ui->courseSearch->setCurrentText(curr);
    }
    ui->mainTable->resizeColumnsToContents();
}

void TeacherManagement::on_searchButton_clicked() {
    refreshData();
}

QStringList TeacherManagement::loadClassList() {
    QStringList result;
    result.append("");
    QSqlQuery query(
        "SELECT DISTINCT "
        "student.class_id "
        "FROM student ",
        this->dataManager->database);
    if (!query.exec()) {
        MY_DEBUG << query.lastError();
        return result;
    }

    while (query.next()) {
        result.append(query.value(0).toString());
    }
    return result;
}
QStringList TeacherManagement::loadCourseList() {
    QStringList result;
    result.append("");
    QSqlQuery query(
        "SELECT DISTINCT "
        "course.name "
        "FROM score "
        "JOIN course ON course.id = score.course_id",
        this->dataManager->database);
    if (!query.exec()) {
        MY_DEBUG << query.lastError();
        return result;
    }

    while (query.next()) {
        result.append(query.value(0).toString());
    }
    return result;
}

void TeacherManagement::on_mainTable_doubleClicked(const QModelIndex& index) {
    auto row = index.row();
    auto model = index.model();
    assert(model == &this->model);
    if (row >= 0) {
        qDebug() << "row = " << row;
        qDebug() << "raw studentid " << model->data(model->index(row, 0));
        auto currScore = model->data(model->index(row, 5)).toString().toInt();
        auto studentID = model->data(model->index(row, 0)).toString();
        auto courseID = model->data(model->index(row, 3)).toString();
        qDebug() << "update student=" << studentID << " courseid=" << courseID
                 << " currscore=" << currScore;
        bool ok;
        auto val = QInputDialog::getInt(this, "请输入新分数", "分数", currScore,
                                        0, 100, 1, &ok);

        if (ok) {
            QSqlQuery query(
                "UPDATE score SET score = ? WHERE student_id = ? AND "
                "course_id "
                "= ?",
                this->dataManager->database);
            query.bindValue(0, val);
            query.bindValue(1, studentID);
            query.bindValue(2, courseID);
            if (!query.exec()) {
                qDebug() << query.lastError();
                QMessageBox::critical(this, "错误", query.lastError().text());
                return;
            } else {
                refreshData();
            }
        }
    }
}

void TeacherManagement::on_pushButton_clicked() {
    QList<int> scores;
    int64_t scoreSum = 0;
    int64_t count = 0;
    const auto totalCount = model.rowCount();
    for (int i = 0; i < totalCount; i++) {
        auto curr = model.data(model.index(i, 5)).toString().toInt();
        scoreSum += curr;
        count += curr >= 60;
        scores.append(curr);
    }
    double sqrDiff = 0;
    const double average = 1.0 * scoreSum / totalCount;
    const auto pow2 = [](double x) -> double { return x * x; };
    for (auto x : scores) {
        sqrDiff += pow2(x - average);
    }
    QMessageBox::information(this, "统计信息",
                             QString("平均分: %1\n标准差: %2\n及格率: %3%")
                                 .arg(average, 0, 'f', 2)
                                 .arg(sqrt(sqrDiff), 0, 'f', 2)
                                 .arg(100.0 * count / totalCount, 0, 'f', 2));
}
