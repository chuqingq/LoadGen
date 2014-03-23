g++ -o loadgen *.c \
-Llib/libuv/lib -luv \
-Ilib -Ilib/libjson/include -Llib/libjson/lib -ljson -DNDEBUG \
-lpthread -ldl \
-g -Wall -Werror -Wl,-export-dynamic
