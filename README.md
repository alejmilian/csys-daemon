# Sistemas Computacionales

---

## Tarea Extraclase

### Watcher

Para la creación del archivo ejecutable `watcher` ejecute el siguiente comando en el directorio donde se encuentre el archivo `Makefile`:

```shell
make
```

Esto generará las biblioteca estática `libfile_log.a` así como el fichero objeto `file_log.o`.

En caso de solo necesitar el fichero ejecutable introduzca el siguiente comando:

```shell
make clean
```

> El comando anterior elimina los ficheros objeto así como las bibliotecas necesarias para el proceso de compilación del ejecutable `watcher`

### Puesta a punto del servicio

Una vez generado el archivo ejecutable es momento de instalar el servicio deseado.
Para ello realice los siguientes pasos:

- Dentro del archivo `file_chkc.service` cambie **path_to_executable** por el directorio donde se encuentre ubicado `watcher`
- Copie el archivo `file_chkd.service` dentro del directorio */etc/systemd/system/*:

```shell
sudo cp file_chkd.service /etc/systemd/system/
```

- Notificar a systemd que existe un nuevo archivo **.service**

```shell
sudo systemctl daemon-reload
```

> Tanto para iniciar, detener y ver estado de la unidad puede emplear el **Shell script** `filechkd` mediante el siguiente listado de comandos.
>
> **Iniciar la unidad**
>
>```shell
> ./filechkd start
> ```
>
> **Detener la unidad**
>
>```shell
> ./filechkd stop
> ```
>
> **Ver el estado de la unidad**
>
>```shell
> ./filechkd status
> ```

> **Nota**:<br>
> Puede emplear los archivos ubicados dentro del directorio *res* para la realización de las pruebas pertinentes.

### Makefile

```makefile
CC=gcc
 
static: main.c libfile_log.a file_log.h
$(CC) -o watcher main.c -I. -L. -lfile_log -lsystemd
 
 
file_log.o: file_log.c
$(CC) -c file_log.c -o file_log.o -I. -L. -lsystemd
 
libfile_log.a: file_log.o
ar -rv libfile_log.a file_log.o
 
 
# Clean all created files
clean:
rm -r *.o *.a
```

### filechkd

```bash
#!/bin/bash
 
service=file_chkd.service
 
start() {
   sudo systemctl start ${service}
}
 
stop() {
   sudo systemctl stop ${service}
}
 
status() {
   sudo echo $(systemctl is-active ${service})
}
 
case "$1" in
   start)
      start
      ;;
   stop)
      stop
      ;;
   status)
      status
      ;;
   *)
      echo "Usages: $0 {start|stop|status}"
esac
 
exit 0
```
