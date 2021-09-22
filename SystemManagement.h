#ifndef SYSTEMMANAGEMENT_H
#define SYSTEMMANAGEMENT_H

#include <QSharedPointer>
#include <QStandardItemModel>
#include <QWidget>
//#include "SystemData.h"
#include "GeneralDataManager.h"
#include "QSqlTableModel"
namespace Ui {
class SystemManagement;
}

class SystemManagement : public QWidget {
    Q_OBJECT

   public:
    explicit SystemManagement(data_manager::GeneralDataManager* data,
                              QWidget* parent = nullptr);
    ~SystemManagement();

   private slots:
    void on_removeRowButton_clicked();

    void on_addRowButton_clicked();

    void on_staffTable_doubleClicked(const QModelIndex& index);

    void on_saveDataButton_clicked();

    void on_courseTable_doubleClicked(const QModelIndex& index);

   private:
    Ui::SystemManagement* ui;
    data_manager::GeneralDataManager* data;
    QSqlTableModel* studentModel = nullptr;
    QSqlTableModel* teacherModel = nullptr;
    QSqlTableModel* courseModel = nullptr;
    QStandardItemModel teachingRankModel;
};

#endif  // SYSTEMMANAGEMENT_H
