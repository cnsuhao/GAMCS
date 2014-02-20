#GIMCS 

GIMCS stands for "_Generalized Intelligence Model and Computer Simulation_".

## Installation instructions for GIMCS

Currently GIMCS has only been compiled and tested on **Linux**. 
Help are needed to port to other platforms.

## Dependencies

### Required

* graphviz, graphviz-devel (http://www.graphviz.org/Download.php)
* libmysqlclient, libmysqlclient-devel (http://dev.mysql.com/downloads/)

### Recommended

* xdot (https://github.com/jrfonseca/xdot.py) [Its' an interactive viewer for graphs in dot format] 

## Installing using CMake

Let's call this directory 'source_dir' (where this README.md file is).
Before starting, create another directory which we will call 'build_dir'.

Do:

``` bash    
mkdir build
cd build
cmake ../
make
make install```

The "_make install_" step map require administrator privileges.

You can adjust the installation destination (the "prefix") 
by passing the `-DCMAKE_INSTALL_PREFIX=myprefix` option to cmake.

## Website

http://www.andy87.com/gimcs
