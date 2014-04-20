OBJS=ls_config.o ls_master.o ls_session.o ls_task_script.o ls_task_var.o \
	main.o ls_logger.o ls_plugin.o ls_task_callmodel.o ls_task_setting.o \
	ls_worker.o

loadgen: $(OBJS)
	g++ -o $@ $^ -Llib/libuv/lib -luv -Llib/libjson/lib -ljson -DNDEBUG -lpthread -ldl -g -Wall -Werror -Wl,-export-dynamic

%.o: %.c %.h
	g++ -c $< -o $@ -DNDEBUG -g -Wall -Werror -Wl,-export-dynamic

clean:
	rm loadgen $(OBJS)
