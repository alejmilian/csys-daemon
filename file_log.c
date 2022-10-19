#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <systemd/sd-journal.h>

#include <file_log.h>

#define _USE_32BIT_TIME_T 1

// Determina la cantidad de archivos a verificar
int files_to_process(char *files_list)
{
    FILE *stream;
    int count = 0;
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;

    stream = fopen(files_list, "r");
    if (stream == NULL)
    {
        perror("Fail while opening config file");
        exit(EXIT_FAILURE);
    }

    // Cuenta el numero de lineas del archivo
    while ((nread = getline(&line, &size, stream)) != -1)
    {
        if (nread != 0)
            ++count;
    }

    free(line);
    fclose(stream);

    return count;
}

// Obtiene la fecha de la última modificación de un archivo
time_t fmod_status(char *file_name)
{
    struct stat sb;
    if (stat(file_name, &sb) == -1)
    {
        perror("Fail while getting file info [Time of last data modification]");
        exit(EXIT_FAILURE);
    }

    return sb.st_mtime;
}

// Obtiene el id del usuario que ha modificado el archivo
uid_t user_status(char *file_name)
{
    struct stat sb;
    if (stat(file_name, &sb) == -1)
    {
        perror("Fail while getting file info [User ID of file]");
        exit(EXIT_FAILURE);
    }

    return sb.st_uid;
}

// Inicializa la estructura files_info con la información de cada archivo a
void init_files_info(pfile *files_info, char *files_list)
{
    FILE *stream;
    int count = 0;
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;

    files_info->tot_arch = files_to_process(files_list);
    files_info->arch = (char **)malloc(files_info->tot_arch * sizeof(char *));

    stream = fopen(files_list, "r");
    if (stream == NULL)
    {
        perror("Fail while opening config file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < files_info->tot_arch; i++)
    {
        nread = getline(&line, &size, stream);
        files_info->arch[i] = (char *)malloc(sizeof(char) * nread);
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        strcpy(files_info->arch[i], line);
    }

    free(line);
    fclose(stream);

    files_info->time_mod = (time_t *)malloc(files_info->tot_arch * sizeof(time_t));
    files_info->user_mod = (uid_t *)malloc(files_info->tot_arch * sizeof(uid_t));
    files_info->check_mod = (int *)malloc(files_info->tot_arch * sizeof(int));

    for (int i = 0; i < files_info->tot_arch; i++)
    {
        files_info->time_mod[i] = fmod_status(files_info->arch[i]);
        files_info->user_mod[i] = user_status(files_info->arch[i]);
        files_info->check_mod[i] = 0;
    }

    sd_journal_print(LOG_NOTICE,"Number of files to supervise: %d\n\n", files_info->tot_arch);
    for (int i = 0; i < files_info->tot_arch; i++)
    {
        sd_journal_print(LOG_NOTICE,"File:               %s\n", files_info->arch[i]);
        sd_journal_print(LOG_NOTICE,"Last modified time: %s", ctime(&files_info->time_mod[i]));
        sd_journal_print(LOG_NOTICE,"User ID:            %d\n\n", files_info->user_mod[i]);
    }
}

// Verifica si alguno de los archivos definidos han sido modificados
void chk_files(pfile *files_info)
{
    for (int i = 0; i < files_info->tot_arch; i++)
    {
        if (difftime(files_info->time_mod[i], fmod_status(files_info->arch[i])))
        {
            files_info->check_mod[i] = 1;
            files_info->time_mod[i] = fmod_status(files_info->arch[i]);
            sd_journal_print(LOG_NOTICE,"File %s was modified \n", files_info->arch[i]);
            sd_journal_print(LOG_NOTICE,"User ID: %d \n", files_info->user_mod[i]);
        }
    }
}
