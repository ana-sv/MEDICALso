balcao.o : balcao.c
	cc balcao.c -c

balcao_functions.o: balcao_functions.c balcao_utils.h
	gcc balcao_functions.c -c 

balcao: balcao.o balcao_functions.o 
	gcc balcao.o balcao_functions.o -o balcao

clean: 
	-rm -f *.o 


