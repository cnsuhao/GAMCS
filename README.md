# GAMCS 

GAMCS是"_Generalized Agent Model and Computer Simulation_"的缩写，意指“广义智能体模型与计算机模拟”。
这是一套我个人研究的理论模型，GAMCS是用面向对象语言C++对这个模型的描述和一种计算机实现。

关于这个模型的更多内容，参见项目网站。

## 安装指南

目前GAMCS的开发主要是在**Linux**平台完成，其他平台未经测试。

欢迎大家帮助在其他平台编译和测试。

## 依赖关系

### 必需的库

* graphviz, graphviz-devel (http://www.graphviz.org/Download.php)
* libmysqlclient, libmysqlclient-devel (http://dev.mysql.com/downloads/)

### 推荐工具

* xdot (https://github.com/jrfonseca/xdot.py) 这是一个简单方便，用来显示dot图格式的小软件
* gephi (http://www.gephi.org) 这是一个绘制和生成大型网络的软件

## 用CMake进行安装

安装完依赖库之后，打开终端，进入源码所在目录（即此文件所在目录），运行以下命令：

    mkdir build
    cd build
    cmake ../
    make
    sudo make install

你可以通过在运行`cmake ../`时提供选项`-DCMAKE_INSTALL_PREFIX=myprefix`来改变软件最终的安装路径。

## 项目网站

http://www.andy87.com/gamcs
