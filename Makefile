SrcFiles=$(wildcard *.c)                                                                                                       
ObjFiles=$(patsubst %.c,%.o,$(SrcFiles))

server:$(ObjFiles)
	gcc -O3 -o sider-server $(ObjFiles) -pg

%.o:%.c
	gcc -O3 -c $< -o $@ -pg 

.PHONY:clean all
clean:
	-rm -rf $(ObjFiles)