
#ifndef BUSCADOR_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define BUSCADOR_H 
#include <glib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

GArray *busca_em_arquivo(char *texto, char *nome_arquivo);

GArray *lista_dir(char *dir);

GArray *lista_dir_recurse(char *dir_path, GArray *resp);

GHashTable *busca_na_pasta(char *texto, char *dir);

#endif