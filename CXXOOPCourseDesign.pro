QT       += core gui
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AddSelectedCourseForm.cpp \
    CourseEditForm.cpp \
    GeneralDataManager.cpp \
    MyQSqlTableModel.cpp \
    StudentCommentEdit.cpp \
    StudentManagement.cpp \
    SystemManagement.cpp \
    TeacherManagement.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AddSelectedCourseForm.h \
    CourseEditForm.h \
    GeneralDataManager.h \
    MyQSqlTableModel.h \
    MyVector.h \
    StudentCommentEdit.h \
    StudentData.h \
    StudentManagement.h \
    StudentSubjectData.h \
    SystemData.h \
    SystemManagement.h \
    TeacherManagement.h \
    Utility.h \
    mainwindow.h

FORMS += \
    AddSelectedCourseForm.ui \
    CourseEditForm.ui \
    StudentCommentEdit.ui \
    StudentManagement.ui \
    TeacherManagement.ui \
    mainwindow.ui \
    systemmanagement.ui
QMAKE_CXXFLAGS += /utf-8
QMAKE_CXXFLAGS_RELEASE += /O1
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
