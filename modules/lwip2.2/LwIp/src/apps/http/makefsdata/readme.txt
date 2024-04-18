A_os fixed
This directory contains a script ('makefsdata') to create C code suitable for
httpd for given html pages (or other files) in the ../fs directory.

example : perl makefsdata -f A_os_Default
The folder is the pointer to un upper directory

The generated file will overwrite the ../fsdata.c file.

Images and others should be supported as well

Please note : the generated fsdata.c must not be included in compilation

