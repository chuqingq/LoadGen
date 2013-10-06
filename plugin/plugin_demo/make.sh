g++ -fPIC -shared -o libplugin_demo.so *.c -I../../ -I../../lib -L../../lib/libuv/lib -luv -Werror -Wall
