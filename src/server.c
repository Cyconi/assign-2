#include "../include/filter.h"
#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INPUT_FIFO "input_fifo"
#define OUTPUT_FIFO "output_fifo"

#define BUFSIZE 1024

static void *handle_client_request(void *arg)
{
    char    buffer[BUFSIZE];
    ssize_t bytes_read;
    int     input_fd = *((int *)arg);
    free(arg);

    bytes_read = read(input_fd, buffer, sizeof(buffer) - 1);
    if(bytes_read > 0)
    {
        char       *response = (char *)malloc(BUFSIZE);
        char       *saveptr;
        int         output_fd;
        const char *conversion_type = strtok_r(buffer, ":", &saveptr);
        char       *client_string   = strtok_r(NULL, ":", &saveptr);
        buffer[bytes_read]          = '\0';

        if(conversion_type && client_string)
        {
            for(int i = 0; client_string[i]; i++)
            {
                if(strcmp(conversion_type, "upper") == 0)
                {
                    client_string[i] = upper_filter(client_string[i]);
                }
                else if(strcmp(conversion_type, "lower") == 0)
                {
                    client_string[i] = lower_filter(client_string[i]);
                }
                else
                {
                    client_string[i] = null_filter(client_string[i]);
                }
            }
            snprintf(response, BUFSIZE, "%s", client_string);
        }
        else
        {
            snprintf(response, BUFSIZE, "Invalid format.");
        }

        output_fd = open(OUTPUT_FIFO, O_WRONLY | O_CLOEXEC);
        if(output_fd >= 0)
        {
            write(output_fd, response, strlen(response));
            close(output_fd);
        }
        else
        {
            perror("Error opening output FIFO");
        }
        free(response);
    }
    close(input_fd);
    pthread_exit(NULL);
}

static void handle_sigint(int signal) __attribute__((noreturn));
static void handle_sigint(int signal)
{
    (void)signal;
    _exit(EXIT_SUCCESS);
}

int main(void)
{
    pthread_t client_thread;

    printf("Server is running...\n");
    signal(SIGINT, handle_sigint);

    while(1)
    {
        int *input_fd = (int *)malloc(sizeof(int));
        if(!input_fd)
        {
            perror("Error allocating memory for input_fd");
            exit(EXIT_FAILURE);
        }

        *input_fd = open(INPUT_FIFO, O_RDONLY | O_CLOEXEC);
        if(*input_fd >= 0)
        {
            printf("processing request...\n");
            pthread_create(&client_thread, NULL, handle_client_request, (void *)input_fd);
            pthread_detach(client_thread);
        }
        else
        {
            perror("Error opening input FIFO");
            free(input_fd);
            exit(EXIT_FAILURE);
        }
    }
}
