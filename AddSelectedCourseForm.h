#ifndef ADDSELECTEDCOURSEFORM_H
#define ADDSELECTEDCOURSEFORM_H

#include <QStandardItemModel>
#include <QWidget>
#include <functional>
#include "GeneralDataManager.h"
namespace Ui {
class AddSelectedCourseForm;
}

class AddSelectedCourseForm : public QWidget {
    Q_OBJECT

   public:
    explicit AddSelectedCourseForm(
        data_manager::GeneralDataManager* dataManager,
        double availableCredits,
        QString studentNumber,
        std::function<void(void)> finishCallback,
        QWidget* parent = nullptr);
    ~AddSelectedCourseForm();

   private slots:
    void on_mainTable_doubleClicked(const QModelIndex& index);

   private:
    Ui::AddSelectedCourseForm* ui;
    data_manager::GeneralDataManager* dataManager;
    double availableCredits;
    QString studentNumber;
    QStandardItemModel model;
    std::function<void(void)> finishCallback;
};

#endif  // ADDSELECTEDCOURSEFORM_H
