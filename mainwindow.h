#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GeneralDataManager.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private slots:
    void on_system_management_button_clicked();

    void on_pushButton_clicked();

    void on_studentManageButton_clicked();

    void on_teacherManageButton_clicked();

    void on_studentManageButton_2_clicked();

   private:
    Ui::MainWindow* ui;
    data_manager::GeneralDataManager* data;
};
#endif  // MAINWINDOW_H
