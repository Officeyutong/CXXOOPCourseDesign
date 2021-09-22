#ifndef STUDENTMANAGEMENT_H
#define STUDENTMANAGEMENT_H

#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QWidget>
#include "GeneralDataManager.h"
namespace Ui {
class StudentManagement;
}

class StudentManagement : public QWidget {
    Q_OBJECT

   public:
    explicit StudentManagement(data_manager::GeneralDataManager* dataManager,
                               QWidget* parent = nullptr);
    ~StudentManagement();

   protected:
    bool eventFilter(QObject* target, QEvent* event);

   private slots:
    void on_studentListTable_doubleClicked(const QModelIndex& index);

    void on_pushButton_clicked();

    void on_addCourseButton_clicked();

   private:
    Ui::StudentManagement* ui;
    data_manager::GeneralDataManager* dataManager;
    QSqlTableModel* listModel;
    QStandardItemModel* detailModel;
    void handleStudentSearch();
    QString currentStudentNumber;
    void refreshStudentTable(const QString& studentNumber);
    double totalCredit;
};

#endif  // STUDENTMANAGEMENT_H
