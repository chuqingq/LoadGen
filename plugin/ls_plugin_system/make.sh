g++ -fPIC -shared -o ls_plugin_system.so *.c \
-I../../ -I../../lib -L../../lib/libuv/lib -luv -DNDEBUG \
-Werror -Wall
