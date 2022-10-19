// #include <sys/stat.h>
#ifndef _FILE_LOG
#define _FILE_LOG
#define MAXLEN 255

#define CONF_FILE_PATH "/etc/sec-info/file.conf"
#define TIME_BETWEEN_CHEKCS 10

typedef struct
{
    int tot_arch;       // Cantidad de archivos a verificar
    char **arch;        // Listado de nombres de los archivos
    time_t *time_mod;  // Listado de fechas de la última modificación
    uid_t *user_mod;   // Listado de IDs del usuario que modificó el archivo
    int *check_mod;     // Listado de flags que indica si fue modificado
} file_info;

typedef file_info pfile;

// Determina la cantidad de archivos a verificar
int files_to_process(char *);

// Obtiene la fecha de la última modificación de un archivo
time_t fmod_status(char *);

// Obtiene el id del usuario que ha modificado el archivo
uid_t user_status(char *);

// Inicializa la estructura file_info con la información de cada archivo a
void init_files_info(pfile*, char *);

// Verifica si alguno de los archivos definidos han sido modificados
void chk_files(pfile*);

#endif
