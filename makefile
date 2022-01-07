cliente.o : cliente.c
	cc cliente.c -c

cliente : cliente.o 
	gcc cliente.o -o cliente

balcao.o : balcao.c utils.h
	cc balcao.c -c

balcao_functions.o : balcao_functions.c 
	gcc balcao_functions.c -c 

balcao: balcao.o balcao_functions.o 
	gcc balcao.o balcao_functions.o -o balcao

clean: 
	-rm -f *.o 


