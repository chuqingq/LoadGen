g++ -o loadgen *.c -Llib/libuv/lib -luv -ljsoncpp -lpthread -ldl -Ilib -g -Wall -Werror -Wl,-export-dynamic
