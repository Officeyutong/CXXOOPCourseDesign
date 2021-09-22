#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H
//#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>
//#include <QVector>
#include <QtNumeric>
#include <functional>
#include <sstream>
#include <stdexcept>
//#include "QVector.h"
#include <QVector>
namespace system_data {

class Student {
    QString ID;
    QString name;
    QString classID;
    QString major;

   public:
    QString getID() const { return ID; }
    QString getName() const { return name; }
    QString getClassID() const { return classID; }
    QString getMajor() const { return major; }

    void setID(const QString& v) { ID = v; }
    void setName(const QString& v) { name = v; }
    void setClassID(const QString& v) { classID = v; }
    void setMajor(const QString& v) { major = v; }
    Student(const QString& ID = "",
            const QString& name = "",
            const QString& classID = "",
            const QString& major = "")
        : ID(ID), name(name), classID(classID), major(major) {}
};
class Staff {
    using CoursesType = QVector<QString>;
    QString ID;
    QString name;
    CoursesType courses;

   public:
    QString getID() const { return ID; }
    QString getName() const { return name; }
    QVector<QString> getCourses() const { return courses; }
    CoursesType& accessCourses() { return courses; }
    void setID(const QString& v) { ID = v; }
    void setName(const QString& v) { name = v; }
    Staff(const QString& ID = "",
          const QString& name = "",
          const CoursesType& courses = CoursesType())
        : ID(ID), name(name), courses(courses) {}
};
class Course {
    QString ID;
    QString name;
    double credit;
    int classHours;
    QString type;

   public:
    QString getID() const { return ID; }
    QString getName() const { return name; }
    double getCredit() const { return credit; }
    int getClassHours() const { return classHours; }
    QString getType() const { return type; }
    void setID(const QString& v) { ID = v; }
    void setName(const QString& v) { name = v; }
    void setCredit(double f) { credit = f; }
    void setClassHours(int v) { classHours = v; }
    void setType(const QString& t) { type = t; }
    Course(const QString& ID = "",
           const QString& name = "",
           double credit = 0,
           int classHours = 0,
           const QString& type = "")
        : ID(ID),
          name(name),
          credit(credit),
          classHours(classHours),
          type(type) {}
};

class SystemData {
   public:
    QVector<Student> students;
    QVector<Course> courses;
    QVector<Staff> staff;
    void handleInput(
        const QString& filename,
        std::function<void(const QVector<QString>& data)> handler) {
        QFile file(filename);
        if (!file.open(QFile::ReadOnly)) {
            throw std::runtime_error(
                QString("Failed to open: %1").arg(filename).toStdString());
        }
        QTextStream stream(&file);
        QString line;
        do {
            line = stream.readLine();
            if (line.startsWith("#END"))
                break;
            if (line.startsWith("#"))
                continue;
            QVector<QString> tokens;
            QString currTok = "";
            //因为不能用STL，所以我认为QStringList也不能用，所以人工parse
            for (int i = 0; i <= line.length();) {
                if (i == line.length() || line.at(i).isSpace()) {
                    tokens.append(currTok);
                    //                    qDebug() << currTok << "received";
                    currTok = "";
                    if (i == line.length())
                        break;
                    while (i < line.length() && line.at(i).isSpace())
                        i++;
                } else {
                    currTok += line[i];
                    i++;
                }
            }
            handler(tokens);
        } while (!line.startsWith("#END"));
    }
    void handleOutput(const QString& file, std::function<void(QFile&)> func) {
        QFile output(file);
        if (!output.open(QFile::WriteOnly)) {
            throw std::invalid_argument(
                QString("Invalie pathname: %1").arg(file).toStdString());
        }
        func(output);
    }
    SystemData(const QString& dataDir) {
        QDir data(dataDir);
        if (!data.exists()) {
            throw std::invalid_argument("Invalid pathname");
        }
        handleInput(data.absoluteFilePath("student.txt"),
                    [&](const QVector<QString>& tokens) {
                        students.append(Student(tokens.at(0), tokens.at(1),
                                                tokens.at(2), tokens.at(3)));
                    });
        handleInput(
            data.absoluteFilePath("staff.txt"),
            [&](const QVector<QString>& tokens) {
                QVector<QString> courses;
                for (int i = 2; i < tokens.size(); i++)
                    courses.append(tokens.at(i));
                staff.append(Staff(tokens.at(0), tokens.at(1), courses));
            });
        handleInput(data.absoluteFilePath("module.txt"),
                    [&](const QVector<QString>& tokens) {
                        courses.append(Course(
                            tokens.at(0), tokens.at(1), tokens.at(2).toDouble(),
                            tokens.at(3).toInt(), tokens.at(4)));
                    });
    }
    SystemData() : SystemData((QDir::currentPath())) {}
    void saveData(const QDir& dataDir) {
        handleOutput(dataDir.absoluteFilePath("student.txt"), [&](QFile& file) {
            file.write(QString("#学号(ID)\t姓名\t班级\t专业\n").toLocal8Bit());
            for (const auto& item : students) {
                file.write(QString("%1\t%2\t%3\t%4\n")
                               .arg(item.getID())
                               .arg(item.getName())
                               .arg(item.getClassID())
                               .arg(item.getMajor())
                               .toLocal8Bit());
            }
            file.write("#END");
        });
        handleOutput(dataDir.absoluteFilePath("module.txt"), [&](QFile& file) {
            file.write(QString("#课程编号\t课程名称\t学分\t学时\t课程类别\n")
                           .toLocal8Bit());
            for (const auto& item : courses) {
                file.write(QString("%1\t%2\t%3\t%4\t%5\n")
                               .arg(item.getID())
                               .arg(item.getName())
                               .arg(item.getCredit())
                               .arg(item.getClassHours())
                               .arg(item.getType())
                               .toLocal8Bit());
            }
            file.write("#END");
        });
        handleOutput(dataDir.absoluteFilePath("staff.txt"), [&](QFile& file) {
            file.write(QString("#工号（ID）\t姓名\t授课\n").toLocal8Bit());
            for (const auto& item : staff) {
                file.write(
                    QString("%1\t%2\t%3\n")
                        .arg(item.getID())
                        .arg(item.getName())
                        .arg(QStringList(item.getCourses().toList()).join(" "))
                        .toLocal8Bit());
            }
            file.write("#END");
        });
    }
    void saveData() { saveData(QDir::currentPath()); }
};
}  // namespace system_data

// QDataStream& operator<<(QDataStream& stream, system_data::Course course) {
//    stream << QString("Course{ID=%1,name=%2,credit=%3,hours=%4,type=%5}")
//                  .arg(course.getID())
//                  .arg(course.getName())
//                  .arg(course.getCredit())
//                  .arg(course.getType());
//    return stream;
//}
// QDataStream& operator<<(QDataStream& stream, system_data::Staff staff) {
//    stream << QString("Staff{ID=%1,name=%2,courses=%3}")
//                  .arg(staff.getID())
//                  .arg(staff.getName());
//    //                  .arg(staff.accessCourses());
//    //.arg(staff.accessCourses());
//    return stream;
//}

#endif  // SYSTEMDATA_H
