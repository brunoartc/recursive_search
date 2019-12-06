WORKING_DIR = $(pwd)
build:
	gcc -pthread `pkg-config --cflags glib-2.0` busca.c buscador.c `pkg-config --libs glib-2.0` -o pesquisador -g
