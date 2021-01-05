cc -Wall -o nova_aplicacao nova_aplicacao.c -lpthread
cc -Wall -shared -o novo_my_semaphore.so novo_my_semaphore.c -ldl -fPIC
LD_PRELOAD=./novo_my_semaphore.so ./nova_aplicacao
