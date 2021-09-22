#ifndef GENERALDATAMANAGER_H
#define GENERALDATAMANAGER_H
#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>
#include <QVector>
#include <stdexcept>

#define SQL_EXEC_AND_LOG(var)                              \
    {                                                      \
        if (!var.exec())                                   \
            qDebug() << __FILE__ << ":" << __LINE__ << " " \
                     << query.lastError();                 \
    }

namespace data_manager {

class GeneralDataManager {
   public:
    QSqlDatabase database;
    const QDir localDir;
    GeneralDataManager();
    void loadShit();
    void exportShit();
    QStringList getCourseTaughtByTeacher(const QString& teacher_id);
};

}  // namespace data_manager

#endif  // GENERALDATAMANAGER_H
