# How to build

To customize your build, you may change several variables that are consumed by make.

These variables, along with some explanations can be found in Makefile.conf.

You can edit that file, or create a file `local.conf` in the root of the repository, which will also be parsed by make.

A sample `local.conf` looks like this:

~~~
PREFIX=$(HOME)/code/libad/build

ENGINE_LIB=-L$(HOME)/code/engine/build/lib
ENGINE_INC=-I$(HOME)/code/engine/build/include
~~~

Now, run:

~~~
make all
make install
~~~
