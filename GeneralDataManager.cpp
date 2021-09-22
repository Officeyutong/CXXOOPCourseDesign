#include "GeneralDataManager.h"
#include <QDataStream>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QRegExp>
#include <QSharedPointer>
#include <QSqlQuery>
#include <functional>
#include <stdexcept>
//#define SQL_ERROR(var)                                                     \
//    if (!var.exec()) {                                                     \
//        qDebug() << __FILE__ << ":" << __LINE__ << " " << var.lastError(); \
//    }
using namespace data_manager;
GeneralDataManager::GeneralDataManager()
    : database(QSqlDatabase::addDatabase("QSQLITE")),
      localDir(QDir::currentPath()) {
    database.setDatabaseName(":memory:");
    if (!database.open()) {
        throw std::runtime_error("Failed to open database");
    }
    database.exec(
        "CREATE TABLE student ("
        "id VARCHAR(20) PRIMARY KEY,"
        "name VARCHAR(20) NOT NULL,"
        "class_id VARCHAR(20) NOT NULL,"
        "major VARCHAR(20) NOT NULL"
        ")");
    qDebug() << database.lastError();

    database.exec(
        "CREATE TABLE course("
        "id VARCHAR(20) PRIMARY KEY,"
        "name VARCHAR(50) NOT NULL,"
        "credit NUMBER NOT NULL,"
        "class_hour INTEGER NOT NULL,"
        "type VARCHAR(20)"
        ")");
    qDebug() << database.lastError();

    database.exec(
        "CREATE TABLE teacher("
        "id VARCHAR(20) PRIMARY KEY,"
        "name VARCHAR(20) NOT NULL"
        ")");
    qDebug() << database.lastError();

    database.exec(
        "CREATE TABLE teacher_course("
        "teacher_id VARCHAR(20),"
        "course_id VARCHAR(20),"
        "PRIMARY KEY(teacher_id,course_id),"
        "FOREIGN KEY (teacher_id) REFERENCES teacher(id) ON DELETE CASCADE ON "
        "UPDATE CASCADE,"
        "FOREIGN KEY (course_id) REFERENCES course(id) ON DELETE CASCADE ON "
        "UPDATE CASCADE"
        ")");
    qDebug() << database.lastError();

    database.exec(
        "CREATE TABLE score("
        "student_id VARCHAR(40),"
        "course_id VARCHAR(40),"
        "score INTEGER,"
        "PRIMARY KEY(student_id,course_id),"
        "FOREIGN KEY (course_id) REFERENCES course(id) ON DELETE CASCADE ON "
        "UPDATE CASCADE,"
        "FOREIGN KEY (course_id) REFERENCES course(id) ON DELETE CASCADE ON "
        "UPDATE CASCADE"
        ")");
    qDebug() << database.lastError();
    database.exec(
        "CREATE TABLE selected_course("
        "student_id VARCHAR(40),"
        "course_id VARCHAR(40),"
        "teacher_id VARCHAR(40),"
        "comment LONGTEXT DEFAULT '',"
        "score INTEGER DEFAULT 0,"
        "PRIMARY KEY(student_id,course_id),"
        "FOREIGN KEY (student_id) REFERENCES student(id) ON DELETE CASCADE ON "
        "UPDATE CASCADE,"
        "FOREIGN KEY (teacher_id) REFERENCES teacher(id) ON DELETE CASCADE ON "
        "UPDATE CASCADE,"
        "FOREIGN KEY (course_id) REFERENCES course(id) ON DELETE CASCADE ON "
        "UPDATE CASCADE"
        ")");
    qDebug() << database.lastError();
    //    database.exec(
    //        "CREATE TABLE student_comment("
    //        "student_id VARCHAR(40),"
    //        "course_id VARCHAR(40),"
    //        "teacher_id VARCHAR(40),"
    //        "comment LONGTEXT,"
    //        "score INTEGER,"
    //        "PRIMARY KEY(student_id,course_id,teacher_id),"
    //        "FOREIGN KEY (student_id) REFERENCES student(id) ON DELETE CASCADE
    //        ON " "UPDATE CASCADE," "FOREIGN KEY (teacher_id) REFERENCES
    //        teacher(id) ON DELETE CASCADE ON " "UPDATE CASCADE," "FOREIGN KEY
    //        (course_id) REFERENCES course(id) ON DELETE CASCADE ON " "UPDATE
    //        CASCADE"
    //        ")");

    //    qDebug() << database.lastError();

    database.commit();
}
void GeneralDataManager::loadShit() {
    QDir dataDir = QDir::currentPath();
    const auto& makeTextStream = [&](const QString& filename) {
        QFile* file = new QFile(dataDir.absoluteFilePath(filename));
        if (!file->open(QFile::ReadOnly)) {
            throw std::runtime_error(
                QString("Failed to open " + filename).toStdString());
        }
        auto stream = QSharedPointer<QTextStream>(new QTextStream(file));
        return stream;
    };
    {
        auto studentStream = makeTextStream("student.txt");
        while (true) {
            auto line = studentStream->readLine();
            qDebug() << "student line recv" << line;
            if (line.startsWith("#END"))
                break;
            if (line.startsWith("#"))
                continue;
            auto splited = line.split(QRegExp("[\t ]+"));
            qDebug() << "insert student " << splited;
            QSqlQuery query;
            query.prepare("INSERT INTO student VALUES (?,?,?,?)");
            for (const auto& item : splited)
                if (item != "")
                    query.addBindValue(item);
            if (!query.exec()) {
                qDebug() << __FILE__ << ":" << __LINE__ << " "
                         << query.lastError();
            }
        }
    }
    QMap<QString, QString> nameToID;
    {
        auto courseStream = makeTextStream("module.txt");
        while (true) {
            auto line = courseStream->readLine();
            if (line.startsWith("#END"))
                break;
            if (line.startsWith("#"))
                continue;
            auto splited = line.split(QRegExp("[\t ]+"));
            qDebug() << "insert course " << splited;
            nameToID[splited[1]] = splited[0];
            QSqlQuery query;
            query.prepare("INSERT INTO course VALUES (?,?,?,?,?)");
            for (const auto& item : splited)
                if (item != "")
                    query.addBindValue(item);
            if (!query.exec()) {
                qDebug() << __FILE__ << ":" << __LINE__ << " "
                         << query.lastError();
            }
        }
    }
    {
        auto scoreStream = makeTextStream("score.txt");
        while (true) {
            auto line = scoreStream->readLine();
            if (line.startsWith("#END"))
                break;
            if (line.startsWith("#"))
                continue;
            auto splited = line.split(QRegExp("[\t ]+"));
            //            qDebug() << "insert score " << splited;
            if (!nameToID.contains(splited[2])) {
                qDebug() << "Invalid course for student: " << splited[2];
                continue;
            }
            QSqlQuery query;
            query.prepare("INSERT INTO score VALUES (?,?,?)");
            query.addBindValue(splited[0]);
            query.addBindValue(nameToID[splited[2]]);
            query.addBindValue(splited[3].toInt());
            if (!query.exec()) {
                qDebug() << __FILE__ << ":" << __LINE__ << " "
                         << query.lastError();
            }
        }
    }

    {
        auto teacherStream = makeTextStream("staff.txt");
        while (true) {
            auto line = teacherStream->readLine();
            if (line.startsWith("#END"))
                break;
            if (line.startsWith("#"))
                continue;
            auto splited = line.split(QRegExp("[\t ]+"));
            QSqlQuery query;
            query.prepare("INSERT INTO teacher VALUES (?,?)");
            for (int i = 0; i < 2; i++)
                query.addBindValue(splited[i]);
            if (!query.exec()) {
                qDebug() << __FILE__ << ":" << __LINE__ << " "
                         << query.lastError();
            }

            for (int i = 2; i < splited.length(); i++) {
                auto course = splited[i];
                if (course == "")
                    continue;
                if (!nameToID.contains(course)) {
                    qDebug() << "Invalid course: " << course;
                    continue;
                }
                query.prepare("INSERT INTO teacher_course VALUES (?,?)");
                query.addBindValue(splited[0]);
                query.addBindValue(nameToID[course]);
                if (!query.exec()) {
                    qDebug() << __FILE__ << ":" << __LINE__ << " "
                             << query.lastError();
                }
            }
        }
    }
    //    {
    //        QFile
    //        selectedCourse(dataDir.absoluteFilePath("selected_course.json"));
    //        if (selectedCourse.exists()) {
    //            selectedCourse.open(QFile::ReadOnly);
    //            QDataStream stream(&selectedCourse);
    //            QJsonArray arr;
    //            stream >> arr;
    //            for (const auto& item : arr) {
    //                QSqlQuery query("INSERT INTO selected_course VALUES
    //                (?,?,?)"); auto obj = item.toObject();
    //                query.addBindValue(obj["student_id"].toString());
    //                query.addBindValue(obj["course_id"].toString());
    //                query.addBindValue(obj["teacher_id"].toString());
    //                if (!query.exec()) {
    //                    qDebug() << __FILE__ << ":" << __LINE__ << " "
    //                             << query.lastError();
    //                }
    //            }
    //        }
    //    }
    {
        QFile studentComment(dataDir.absoluteFilePath("selected_course.json"));
        if (studentComment.exists()) {
            studentComment.open(QFile::ReadOnly);
            QDataStream stream(&studentComment);
            QJsonArray arr;
            stream >> arr;
            for (const auto& item : arr) {
                QSqlQuery query(
                    "INSERT INTO selected_course VALUES (?,?,?,?,?)");
                auto obj = item.toObject();
                query.addBindValue(obj["student_id"].toString());
                query.addBindValue(obj["course_id"].toString());
                query.addBindValue(obj["teacher_id"].toString());
                query.addBindValue(obj["comment"].toString());
                query.addBindValue(obj["score"].toInt());

                if (!query.exec()) {
                    qDebug() << __FILE__ << ":" << __LINE__ << " "
                             << query.lastError();
                }
            }
        }
    }

    {
        QSqlQuery query(
            "SELECT "
            "COUNT(*) FROM student");
        if (!query.exec()) {
            qDebug() << __FILE__ << ":" << __LINE__ << " " << query.lastError();
        } else {
            if (query.first()) {
                qDebug() << query.value(0).toInt() << "students loaded";
            }
        }
    }
}
void GeneralDataManager::exportShit() {
    QDir dataDir = QDir::currentPath();
    const auto& openFile = [&](const QString& filename) {
        QFile* file = new QFile(dataDir.absoluteFilePath(filename));
        if (!file->open(QFile::WriteOnly)) {
            throw std::runtime_error(
                QString("Failed to open " + filename).toStdString());
        }
        return QSharedPointer<QFile>(file);
    };
    {
        QSqlQuery query("SELECT * FROM student");
        if (!query.exec()) {
            qDebug() << __FILE__ << ":" << __LINE__ << " " << query.lastError();
        } else {
            auto file = openFile("student.txt");
            file->write(QString("#学号\t姓名\t班级\t专业\n").toLocal8Bit());
            while (query.next()) {
                file->write(QString("%1\t%2\t%3\t%4\n")
                                .arg(query.value(0).toString())
                                .arg(query.value(1).toString())
                                .arg(query.value(2).toString())
                                .arg(query.value(3).toString())
                                .toLocal8Bit());
            }
            file->write("#END");
        }
    }
    {
        QSqlQuery query("SELECT * FROM course");
        if (!query.exec()) {
            qDebug() << __FILE__ << ":" << __LINE__ << " " << query.lastError();
        } else {
            auto file = openFile("module.txt");
            file->write(
                QString("#编号\t名称\t学分\t学时\t类型\n").toLocal8Bit());
            while (query.next()) {
                file->write(QString("%1\t%2\t%3\t%4\t%5\n")
                                .arg(query.value(0).toString())
                                .arg(query.value(1).toString())
                                .arg(query.value(2).toString())
                                .arg(query.value(3).toString())
                                .arg(query.value(4).toString())
                                .toLocal8Bit());
            }
            file->write("#END");
        }
    }
    {
        QSqlQuery query("SELECT * FROM teacher");
        if (!query.exec()) {
            qDebug() << __FILE__ << ":" << __LINE__ << " " << query.lastError();
        } else {
            auto file = openFile("staff.txt");
            file->write(QString("#编号\t姓名\t课程\n").toLocal8Bit());
            while (query.next()) {
                auto id = query.value(0).toString();
                file->write(QString("%1\t%2\t%3\n")
                                .arg(id)
                                .arg(query.value(1).toString())
                                .arg(getCourseTaughtByTeacher(id).join(" "))
                                .toLocal8Bit());
            }
            file->write("#END");
        }
    }
    {
        auto file = openFile("score.txt");
        QSqlQuery query(
            "SELECT score.student_id, score.score, student.name, "
            "course.name "
            "FROM score "
            "JOIN student ON student.id=score.student_id "
            "JOIN course ON course.id=score.course_id");
        if (!query.exec()) {
            qDebug() << __FILE__ << ":" << __LINE__ << " " << query.lastError();
        } else {
            file->write(QString("#学号\t姓名\t课程名称\t成绩\n").toLocal8Bit());
            while (query.next()) {
                file->write(QString("%1\t%2\t%3\t%4\n")
                                .arg(query.value(0).toString())
                                .arg(query.value(2).toString())
                                .arg(query.value(3).toString())
                                .arg(query.value(1).toString())
                                .toLocal8Bit());
            }
            file->write("#END");
        }
    }
    {
        QSqlQuery query("SELECT * FROM selected_course");
        if (!query.exec()) {
            qDebug() << __FILE__ << ":" << __LINE__ << " " << query.lastError();
        } else {
            auto file = openFile("selected_course.json");
            QDataStream stream(file.data());
            QJsonArray array;
            while (query.next()) {
                QJsonObject obj;
                obj["student_id"] = query.value(0).toString();
                obj["course_id"] = query.value(1).toString();
                obj["teacher_id"] = query.value(2).toString();
                obj["comment"] = query.value(3).toString();
                obj["score"] = query.value(4).toInt();
                array.append(obj);
            }
            stream << array;
        }
    }
}

QStringList GeneralDataManager::getCourseTaughtByTeacher(
    const QString& teacher_id) {
    QSqlQuery query(
        "SELECT "
        "course.name,teacher_course.course_id FROM teacher_course JOIN "
        "course "
        "ON course.id=teacher_course.course_id WHERE "
        "teacher_course.teacher_id=? ");
    //    qDebug() << "query " << teacher_id;
    query.addBindValue(teacher_id);
    QStringList result;
    if (!query.exec()) {
        qDebug() << __FILE__ << ":" << __LINE__ << " " << query.lastError();
    }
    while (query.next()) {
        result.append(query.value(0).toString());
    }
    return result;
}
