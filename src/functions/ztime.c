#include <stdio.h>
#include <sys/time.h>

void timeActual() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Convertir a milisegundos
    long milisegundos = tv.tv_usec / 1000;

    // Obtener la hora local
    struct tm* tiempoLocal = localtime(&tv.tv_sec);

    printf("Time: %02d:%02d:%02d:%03ld\n",
           tiempoLocal->tm_hour,
           tiempoLocal->tm_min,
           tiempoLocal->tm_sec,
           milisegundos);
}

