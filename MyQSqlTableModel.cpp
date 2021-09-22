#include "MyQSqlTableModel.h"
MyQSqlTableModel::MyQSqlTableModel(QSqlDatabase db,
                                   std::function<bool(int)> checker,
                                   QObject* parent)
    : QSqlTableModel(parent, db), checker(checker) {}
Qt::ItemFlags MyQSqlTableModel::flags(const QModelIndex& index) const {
    auto row = QSqlTableModel::flags(index);
    if (checker(index.column())) {  //允许编辑
        return row;
    } else {
        return row & (~Qt::ItemIsEditable);
    }
}
