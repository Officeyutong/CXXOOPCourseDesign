#ifndef MYQSQLTABLEMODEL_H
#define MYQSQLTABLEMODEL_H

#include <QSqlTableModel>
#include <functional>
class MyQSqlTableModel : public QSqlTableModel {
   public:
    MyQSqlTableModel(QSqlDatabase db,
                     std::function<bool(int)> checker,
                     QObject* parent);
    Qt::ItemFlags flags(const QModelIndex& index) const;

   private:
    std::function<bool(int)> checker;
};

#endif  // MYQSQLTABLEMODEL_H
