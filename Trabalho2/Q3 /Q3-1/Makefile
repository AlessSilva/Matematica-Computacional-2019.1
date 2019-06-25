CC = g++
CFLAGS1 = -c -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -DILOSTRICTPOD
CFLAGS2 =    -m64 -O -O0 -fPIC -fexceptions -DNDEBUG -DIL_STD -DILOSTRICTPOD -Wall
LIBS =  -lm 
INCLUDES = -I/home/alessandro/cplex/include/ -I/home/alessandro/concert/include/
OBJS = Q1PL.o
SRCS = Q1PL.cpp 
LIBS = -L/home/alessandro/cplex/lib/x86-64_linux/static_pic -lilocplex -lcplex -L/home/alessandro/concert/lib/x86-64_linux/static_pic -lconcert -lm -lpthread
HDRS =  
EXEC = PL

compila: Q1PL.cpp
#para compilar no lia no cplex versão 11
	${CC} ${CFLAGS1} ${INCLUDES} ${SRCS} -std=c++11
	${CC} ${CFLAGS2} ${INCLUDES} ${OBJS} -o ${EXEC} ${LIBS}

executa: compila
	./PL ./instances/a
