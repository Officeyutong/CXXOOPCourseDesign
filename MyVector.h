#ifndef MYVECTOR_H
#define MYVECTOR_H
#include <inttypes.h>
//#include <QDataStream>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <stdexcept>
template <class T>
class MyVector {
   private:
    T* data = nullptr;
    size_t len = 0, cap = 0;
    void ensureCapacity(size_t newLen) {
        if (newLen <= cap)
            return;
        auto next = newLen * 2;
        auto newData = new T[next];
        std::copy(data, data + len, newData);
        delete[] data;
        this->data = newData;
    }
    void tryToShrink() {
        if (len == 0)
            return;
        if (len * 4 <= cap * 3) {  //收缩条件
            auto nextCap = len;
            auto newData = new T[nextCap];
            std::copy(data, data + len, newData);
            delete[] data;
            this->data = newData;
            this->cap = this->len;
        }
    }
    void testIndex(size_t index) const {
        if (index < 0 || index >= len) {
            throw std::invalid_argument(
                QString("Index %1 is not in range [0, %2)")
                    .arg(index)
                    .arg(len)
                    .toStdString());
        }
    }

   public:
    MyVector(size_t initCap = 10) {
        if (initCap <= 0) {
            throw std::invalid_argument("initCap must be greater than 0");
        }
        data = new T[initCap];
        len = 0;
        cap = initCap;
    }
    MyVector(const std::initializer_list<T>& data) {
        len = cap = data.size();
        this->data = new T[len];
        std::copy(data.begin(), data.end(), this->data);
    }
    void append(const T& val) {
        ensureCapacity(len + 1);
        this->data[len] = val;
        len++;
    }
    T pop() {
        if (len == 0) {
            throw std::invalid_argument("Empty vector!");
        }
        auto ret = this->data[len - 1];
        len--;
        tryToShrink();
        return ret;
    }
    T& operator[](size_t index) {
        testIndex(index);
        return data[index];
    }

    T eraseAny(size_t pos) {
        testIndex(pos);
        auto ret = data[pos];
        for (size_t i = pos; i < len; i++) {
            data[i] = data[i + 1];
        }
        len--;
        tryToShrink();
    }
    void insertAt(size_t pos, const T& val) {
        testIndex(pos);
        ensureCapacity(len + 1);
        for (size_t i = len; i > pos; i--) {
            data[i] = data[i - 1];
        }
        data[pos] = val;
    }
    T* begin() { return data; }
    T* end() { return data + len; }
    size_t size() const { return len; }
    const T& at(size_t index) const {
        testIndex(index);
        return data[index];
    }
    MyVector(const MyVector<T>& rhs) {
        this->len = rhs.len;
        this->cap = rhs.cap;
        this->data = new T[this->cap];
        for (size_t i = 0; i < len; i++)
            data[i] = rhs.at(i);
    }
    MyVector<T>& operator=(const MyVector<T>& rhs) {
        if (this != &rhs) {
            delete[] data;
            this->len = rhs.len;
            this->cap = rhs.cap;
            this->data = new T[this->cap];
            for (size_t i = 0; i < len; i++)
                data[i] = rhs.at(i);
        }
        return *this;
    }
    MyVector(const MyVector<T>&& rhs) {
        this->len = rhs.len;
        this->cap = rhs.cap;
        this->data = rhs.data;
    }
    ~MyVector() { delete[] data; }
};
// template <class T>
// QDataStream& operator<<(QDataStream& dbg, const MyVector<T>& vec) {
//    dbg << "MyVector[";
//    for (size_t i = 0; i < vec.size(); i++) {
//        dbg << vec.at(i);
//        if (i != vec.size() - 1)
//            dbg << ",";
//    }
//    dbg << "]";
//    return dbg;
//}
// template <class T>
// QDebug operator<<(QDebug dbg, const MyVector<T>& vec) {
//    dbg << QString("%1").arg(vec);
//    return dbg;
//}
#endif  // MYVECTOR_H
