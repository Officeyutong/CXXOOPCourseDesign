#ifndef TEACHERMANAGEMENT_H
#define TEACHERMANAGEMENT_H

#include <QStandardItemModel>
#include <QWidget>
#include "GeneralDataManager.h"
namespace Ui {
class TeacherManagement;
}

class TeacherManagement : public QWidget {
    Q_OBJECT

   public:
    explicit TeacherManagement(data_manager::GeneralDataManager* dataManager,
                               QWidget* parent = nullptr);
    ~TeacherManagement();

   private slots:
    void on_searchButton_clicked();

    void on_mainTable_doubleClicked(const QModelIndex& index);

    void on_pushButton_clicked();

   private:
    Ui::TeacherManagement* ui;
    data_manager::GeneralDataManager* dataManager;
    QStandardItemModel model;
    void refreshData();
    QStringList loadClassList();
    QStringList loadCourseList();
};

#endif  // TEACHERMANAGEMENT_H
