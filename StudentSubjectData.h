#ifndef STUDENTSUBJECTDATA_H
#define STUDENTSUBJECTDATA_H
#include <QDataStream>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QVector>
namespace student_subject {

struct StudentSubject {
   public:
    QString subjectName;
    QString teacher;
    QJsonObject encodeJson() const {
        QJsonObject ret;
        ret["subjectName"] = subjectName;
        ret["teacher"] = teacher;
        return ret;
    }
    static StudentSubject fromJson(QJsonObject obj) {
        StudentSubject stu;
        stu.subjectName = obj["subjectName"].toString();
        stu.teacher = obj["teacher"].toString();
        return stu;
    }
};

struct StudentSubjectData {
    QMap<QString, QVector<StudentSubject>> data;
    StudentSubjectData(const QDir& dataDir = QDir::currentPath()) {
        QFile dataFile(dataDir.absoluteFilePath("student_subject.json"));
        if (!dataFile.exists()) {
            return;
        }
        dataFile.open(QFile::ReadOnly);
        QJsonObject val;
        QDataStream stream(&dataFile);
        stream >> val;
        for (const auto& key : val.keys()) {
            auto array = val[key].toArray();
            for (const auto& entry : array) {
                data[key].append(StudentSubject::fromJson(entry.toObject()));
            }
        }
    }
    void saveFile(const QDir& dataDir = QDir::currentPath()) {
        QFile dataFile(dataDir.absoluteFilePath("student_subject.json"));
        dataFile.open(QFile::WriteOnly);
        QDataStream stream(&dataFile);
        QJsonObject obj;
        for (const auto& key : data.keys()) {
            QJsonArray arr;
            for (const auto& item : data[key]) {
                arr.append(item.encodeJson());
            }
            obj[key] = arr;
        }
        stream << obj;
    }
};
}  // namespace student_subject
#endif  // STUDENTSUBJECTDATA_H
