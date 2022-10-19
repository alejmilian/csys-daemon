#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <systemd/sd-journal.h>
#include <unistd.h>

#include <file_log.h>

#define _USE_32BIT_TIME_T 1

int main()
{
    pfile files_info;
    init_files_info(&files_info, CONF_FILE_PATH);
    fflush(NULL);
    time_t daemon_time;
    daemon_time = time(NULL);
    fflush(NULL);
    sd_journal_print(LOG_NOTICE, "Checking files at:%s", (char *)ctime(&daemon_time));

    while (1)
    {
        fflush(NULL);
        chk_files(&files_info);
        sleep(TIME_BETWEEN_CHEKCS);
    }
}
