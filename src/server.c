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

// Function to handle client requests
static void *handle_client_request(void *arg)
{
    char    buffer[BUFSIZE];
    ssize_t bytes_read;
    int     input_fd = *((int *)arg);
    free(arg);

    // Read the request from the input FIFO
    bytes_read = read(input_fd, buffer, sizeof(buffer) - 1);
    if(bytes_read > 0)
    {
        char       *response = malloc(BUFSIZE);
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
            }
            snprintf(response, BUFSIZE, "%s", client_string);
        }
        else
        {
            snprintf(response, BUFSIZE, "Invalid format.");
        }

        // Write the processed string to the output FIFO
        output_fd = open(OUTPUT_FIFO, O_WRONLY | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(output_fd >= 0)
        {
            write(output_fd, response, strlen(response));
            close(output_fd);
        }
        else
        {
            perror("Error opening output FIFO");
        }
        free(response);    // Free allocated response buffer
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
    // Set up signal handling for graceful termination
    signal(SIGINT, handle_sigint);

    while(1)
    {
        int *input_fd = malloc(sizeof(int));
        if(!input_fd)    // Check for successful malloc
        {
            perror("Error allocating memory for input_fd");
            exit(EXIT_FAILURE);
        }

        *input_fd = open(INPUT_FIFO, O_RDONLY | O_CLOEXEC);
        if(*input_fd >= 0)
        {
            printf("processing request...\n");
            // Create a new thread to handle the request
            pthread_create(&client_thread, NULL, handle_client_request, (void *)input_fd);
            pthread_detach(client_thread);    // No need to join the thread
        }
        else
        {
            perror("Error opening input FIFO");
            free(input_fd);    // Free allocated input_fd if opening fails
            exit(EXIT_FAILURE);
        }
    }
}
