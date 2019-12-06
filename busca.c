#include "buscador.h"

int main(int argc, char const *argv[])
{

    if (argc < 3){
        printf("%s", "too few arguments");
        exit(3);
    }

    DIR *dp;

    char *ban;
    char texto[256], dir[256];

    strcpy(texto, argv[2]);

    strcpy(dir, argv[1]);

    struct stat path_stat;
    stat(dir, &path_stat);
    if (S_ISREG(path_stat.st_mode)){

    } else {
        GHashTable *hash = busca_na_pasta(texto , dir);

        g_hash_table_destroy(hash);
    }

    



    
    

    
    

}
