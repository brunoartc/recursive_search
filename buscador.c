#include "buscador.h"

struct find_insert {
        GHashTable *hashtable;
        gchar *file_name;
        char *texto;
};

pthread_mutex_t lock;

void *find_and_insert(void *_arg) {
    struct find_insert *spa = _arg;
    GArray *found;
    found = busca_em_arquivo(spa->texto, spa->file_name);
    pthread_mutex_lock(&lock);
    g_hash_table_insert(spa->hashtable, spa->file_name, found);
    pthread_mutex_unlock(&lock);
    return NULL;
}

GArray *busca_em_arquivo_glib(char *texto, char *nome_arquivo){
    gchar **content;
    gsize length;
    GError **err;
    GArray *aaa;
    aaa = g_array_sized_new( FALSE, FALSE, sizeof(GString), 1 );
    GString *ss;
    ss = g_string_new("");

    int *file_status;
    g_file_get_contents(nome_arquivo, content, NULL, err);
    
    GString *s;
    GString *response = g_string_new(*content);
    gchar *file_text = response->str;
    unsigned int file_len = response->len, text_len = strlen(texto);
    
    int done;

    for(int i=0; i<file_len - text_len; i++)
    {
        done = 1;
        for(int j=0; j<text_len; j++)
        {
            if(file_text[i + j] != texto[j])
            {
                done = 0;
                break;
            }
        }

        if(done)
        {
            ss = g_string_new("");
            g_string_append_printf( ss, "%d", i );
            g_array_append_val( aaa, *ss );
            
        }
    }
    return aaa;
}

GArray *busca_em_arquivo(char *texto, char *nome_arquivo){
    GArray *aaa;
    aaa = g_array_sized_new( FALSE, FALSE, sizeof(GString), 1 );
    GString *ss;
    char * buffer = 0;
    long length;
    FILE * f = fopen (nome_arquivo, "rb");



    if (f)
    {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length);
        if (buffer)
        {
            fread (buffer, 1, length, f);
        }
        fclose (f);
    }

    if (buffer)
    {
        char *file_text = buffer;
    unsigned int text_len = strlen(texto), file_len = length;
    int done = 0;


    for(int i=0; i<file_len - text_len; i++)
    {
        done = 1;
        for(int j=0; j<text_len; j++)
        {
            if(file_text[i + j] != texto[j])
            {
                done = 0;
                break;
            }
        }

        if(done)
        {
            printf("Arquivo %s:\n\tEncontrado na posição: %d\n",nome_arquivo, i);
            ss = g_string_new("");
            g_string_append_printf( ss, "%d", i );
            g_array_append_val( aaa, *ss );
        }
    }
    if (aaa->len < 1) printf("Arquivo %s:\n\tNão Encontrado\n",nome_arquivo);
    return aaa;
    }
}

GArray *lista_dir(char *dir){
    GArray *resp;
    resp = g_array_sized_new( FALSE, FALSE, sizeof(GString), 1 );

    DIR *dp;
    GString *s;
    struct dirent *ep;     
    dp = opendir (dir);
    if (dp != NULL)
    {
        while (ep = readdir (dp))
        if (ep->d_type != 4){
            
            s = g_string_new( ep->d_name );
            g_array_append_val( resp, *s );
        }
        

        (void) closedir (dp);
    }
    else
        perror ("Diretorio ou arquivo não encontrado");
    return resp;

}

GArray *lista_dir_recurse(char *dir_path, GArray *resp){
    GString *s;
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(dir_path);

    // Unable to open directory stream
    if (!dir)
        return resp;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            

            //s = g_string_new( dp->d_name );
            //g_array_append_val( resp, *s );

            // Construct new path from our base path
            strcpy(path, dir_path);
            strcat(path, "/");
            strcat(path, dp->d_name);


            if (dp->d_type != 4){

                //printf("%s\n", path);
            
                s = g_string_new( dp->d_name );
                g_array_append_val( resp, *s );
            }


            lista_dir_recurse(path, resp);
        }
    }

    closedir(dir);

    return resp;

}

GHashTable *busca_na_pasta(char *texto, char *dir){

    


    GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
    //g_hash_table_insert(hash, "Virginia", "Richmond");
    //g_hash_table_insert(hash, "Texas", "Austin");
    //g_hash_table_insert(hash, "Ohio", "Columbus");
    

    GArray *current_dir;
    current_dir = lista_dir(dir);

    GArray *resp;
    resp = g_array_sized_new( FALSE, FALSE, sizeof(GString), 1 );

    lista_dir_recurse(dir, resp);


    GString file_name;

    struct find_insert *arg = malloc(sizeof(struct find_insert) * resp->len);
    pthread_t *pids = malloc(sizeof(pthread_t) * resp->len);


    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        exit(3);
    }


    for (int i = 0; i < resp->len; i++)
    {
        file_name = g_array_index( resp, GString, i );
        //printf("\n%s\n", file_name.str);

        arg[i].file_name = file_name.str;

        arg[i].hashtable = hash;

        arg[i].texto = texto;


        pthread_create(&pids[i], NULL, find_and_insert, &arg[i]);

        
    }


    for (int i = 0; i < resp->len; i++) {
        pthread_join(pids[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("There are %d keys in the hash\n", g_hash_table_size(hash));
    //printf("The capital of Texas is %p\n", (char*) g_hash_table_lookup(hash, "Makefile"));
    //gboolean found_hash = g_hash_table_remove(hash, "Virginia");
    //printf("The value 'Virginia' was %sfound and removed\n", found_hash ? "" : "not ");
    //g_hash_table_destroy(hash); // remove gtable


    g_array_free (current_dir, TRUE);

    g_array_free (resp, TRUE);

    return hash;

}




