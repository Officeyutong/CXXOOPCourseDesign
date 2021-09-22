# C++OOP课设 - 学生信息管理系统

一个毫无实用价值的应付作业的课设项目，全是屎山，需者自取。

使用的GUI框架为Qt5，其中也大量使用了Qt5的数据结构，使用的工具链为Windows - MSVC, statically linked

`StudentData.h`, `StudentSubjectData.h`, `SystemData.h`是为了把这个程序伪装成OOP程序而写的文件，实际中并未使用。数据存储后端使用SQLite（我搞不懂这个程序能跟OOP挂什么边）。

`GeneralDataManager`中所实现的功能为`从所给定的txt格式读取数据，转换到内存数据库`和`从数据库中读取数据，转换成给定的txt格式`，用以实现兼容。

请从合理的地方获取`module.txt`,`score.txt`,`staff.txt`,`student.txt`用以提供数据。