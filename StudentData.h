#ifndef STUDENTDATA_H
#define STUDENTDATA_H
#include <QDebug>
#include <QDir>
#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <stdexcept>
#include "SystemData.h"
namespace student_data {
using CourseEntryPtr = QSharedPointer<struct CourseEntry>;
using CourseEntryList = QVector<CourseEntryPtr>;
using MapType = QMap<QString, QSharedPointer<struct Student>>;
struct CourseEntry : public QObject {
    Q_OBJECT
   public:
    QString studentNumber;
    QString studentName;
    QString courseName;
    int score;
    CourseEntry(const QString& studentNumber,
                const QString& studentName,
                const QString& className,
                int score)
        : studentNumber(studentNumber),
          studentName(studentName),
          courseName(className),
          score(score) {}
};
struct Student : public QObject {
    Q_OBJECT
   public:
    QString studentNumber;
    QString studentName;
    QVector<CourseEntryPtr> courses;
    Student(const QString& studentNumber, const QString& studentName)
        : studentNumber(studentNumber), studentName(studentName) {}
};
class StudentDataManager : public QObject {
    Q_OBJECT
   private:
    QSharedPointer<CourseEntryList> entries =
        (QSharedPointer<CourseEntryList>(new CourseEntryList()));
    QSharedPointer<MapType> byStudentNumber =
        QSharedPointer<MapType>(new MapType());

   public:
    StudentDataManager(const QDir& dataDir) {
        auto dataFilePath = dataDir.absoluteFilePath("score.txt");
        QFile dataFile(dataFilePath);
        QTextStream input(&dataFile);
        if (!dataFile.open(QFile::ReadOnly)) {
            throw std::invalid_argument(QString("Invalid data file: %1")
                                            .arg(dataFilePath)
                                            .toStdString());
        }
        auto& ref = *entries;
        QString line;
        do {
            line = input.readLine();
            if (line.startsWith("#END"))
                break;
            if (line.startsWith("#"))
                continue;
            auto tokens = line.split("\t");
            ref.append(CourseEntryPtr(new CourseEntry(
                tokens[0], tokens[1], tokens[2], tokens[3].toInt())));
            qDebug() << "received line " << line;
        } while (!line.startsWith("#END"));
        auto& map = *byStudentNumber;
        system_data::SystemData systemData;
        for (const auto& student : systemData.students) {
            map[student.getID()] = QSharedPointer<Student>(
                new Student(student.getID(), student.getName()));
        }
        for (auto entry : ref) {
            if (!map.contains(entry->studentNumber)) {
                qDebug() << "Ignored entry: " << entry->studentName << " "
                         << entry->studentNumber << " " << entry->courseName;
                continue;
            }
            auto& currStudent = *map[entry->studentNumber];
            currStudent.courses.append(entry);
        }
        dataFile.close();
    }
    StudentDataManager() : StudentDataManager(QDir::currentPath()) {}
    void saveData() { saveData(QDir::currentPath()); }
    void saveData(const QDir& dataDir) {
        auto dataFilePath = dataDir.absoluteFilePath("score.txt");
        QFile dataFile(dataFilePath);
        if (!dataFile.open(QFile::WriteOnly)) {
            throw std::invalid_argument(QString("Invalid data file: %1")
                                            .arg(dataFilePath)
                                            .toStdString());
        }
        dataFile.write(QString("#学号\t姓名\t课程名称\t成绩\n").toLocal8Bit());
        for (auto entry : *(this->entries)) {
            const auto& e = *entry;
            dataFile.write(QString("%1\t%2\t%3\t%4\n")
                               .arg(e.studentNumber)
                               .arg(e.studentName)
                               .arg(e.courseName)
                               .arg(e.score)
                               .toLocal8Bit());
        }
        dataFile.write("#END");
        dataFile.close();
    }
    QSharedPointer<CourseEntryList> getEntries() { return entries; }
    QSharedPointer<MapType> getMap() { return byStudentNumber; }
    void collectEntries() {
        entries.clear();
        for (const auto& student : byStudentNumber->values()) {
            entries->append(QVector<QSharedPointer<CourseEntry>>(
                student->courses.begin(), student->courses.end()));
        }
    }
};
}  // namespace student_data

#endif  // STUDENTDATA_H
