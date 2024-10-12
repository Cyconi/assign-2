#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INPUT_FIFO "input_fifo"
#define OUTPUT_FIFO "output_fifo"

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    int         opt;
    const char *client_string   = NULL;
    const char *conversion_type = NULL;
    int         input_fd;
    int         output_fd;
    char        buffer[BUFSIZE];
    char        response[BUFSIZE];
    ssize_t     bytes_read;

    // Parse command-line arguments
    while((opt = getopt(argc, argv, "s:f:")) != -1)
    {
        switch(opt)
        {
            case 's':
                client_string = optarg;
                break;
            case 'f':
                conversion_type = optarg;
                break;
            default:
                printf("Usage: %s -s <string> -f <conversion>\n", argv[0]);
                printf("<conversion> -> \"upper\", \"lower\"\n");
                exit(EXIT_FAILURE);
        }
    }

    if(client_string == NULL || conversion_type == NULL)
    {
        printf("Usage: %s -s <string> -f <conversion>\n", argv[0]);
        printf("<conversion> -> \"upper\", \"lower\"\n");
        exit(EXIT_FAILURE);
    }

    // Open the input FIFO for writing
    input_fd = open(INPUT_FIFO, O_WRONLY | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(input_fd < 0)
    {
        perror("Error opening input FIFO");
        exit(EXIT_FAILURE);
    }

    // Format the message as "conversion:string"
    snprintf(buffer, sizeof(buffer), "%s:%s", conversion_type, client_string);

    // Send the formatted string to the server
    write(input_fd, buffer, strlen(buffer));
    close(input_fd);

    // Open the output FIFO for reading the response
    output_fd = open(OUTPUT_FIFO, O_RDONLY | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(output_fd < 0)
    {
        perror("Error opening output FIFO");
        exit(EXIT_FAILURE);
    }

    // Read the response from the server
    bytes_read = read(output_fd, response, sizeof(response) - 1);
    if(bytes_read > 0)
    {
        response[bytes_read] = '\0';
        printf("Processed String: %s\n", response);
    }
    else
    {
        perror("Error reading from output FIFO");
    }

    close(output_fd);

    return 0;
}
