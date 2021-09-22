#ifndef COURSEEDITFORM_H
#define COURSEEDITFORM_H

#include <QStandardItemModel>
#include <QString>
#include <QWidget>
#include <functional>
#include "GeneralDataManager.h"
namespace Ui {
class CourseEditForm;
}

class CourseEditForm : public QWidget {
    Q_OBJECT

   public:
    explicit CourseEditForm(QWidget* parent,
                            const QString& teacher_id,
                            const QString& teacher_name,
                            data_manager::GeneralDataManager* dataManager,
                            std::function<void(void)> finishCallback);
    ~CourseEditForm();

   private slots:
    void on_cancelButton_clicked();

    void on_saveButton_clicked();

   private:
    Ui::CourseEditForm* ui;
    QStandardItemModel* model;
    data_manager::GeneralDataManager* dataManager;
    QString teacher_id;
    std::function<void(void)> finishCallback;
    //    const QString& teacher_name;
};

#endif  // COURSEEDITFORM_H
