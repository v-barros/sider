SrcFiles=$(wildcard *.c)                                                                                                       
ObjFiles=$(patsubst %.c,%.o,$(SrcFiles))

server:$(ObjFiles)
	gcc -o sider-server $(ObjFiles) -pg

%.o:%.c
	gcc -c $< -o $@ -pg

.PHONY:clean all
clean:
	-rm -rf $(ObjFiles)