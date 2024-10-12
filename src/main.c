#include "../include/filter.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int         opt;
    const char *input_file  = NULL;
    const char *output_file = NULL;
    int         filter_type = 0;
    int         input_fd;
    int         output_fd;
    char        buffer;
    ssize_t     bytes_read;

    // Parse command-line arguments
    while((opt = getopt(argc, argv, "i:o:f:")) != -1)
    {
        switch(opt)
        {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'f':
                if(strcmp(optarg, "upper") == 0)
                {
                    filter_type = 1;
                }
                else if(strcmp(optarg, "lower") == 0)
                {
                    filter_type = 2;
                }
                else if(strcmp(optarg, "null") == 0)
                {
                    filter_type = 0;
                }
                else
                {
                    printf("Invalid filter function: %s\n", optarg);
                    printf("-f filter -> \"upper\" \"lower\" \"null\"\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                printf("Usage: %s -i input_file -o output_file -f filter\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(input_file == NULL || output_file == NULL)
    {
        printf("Usage: %s -i input_file -o output_file -f filter\n", argv[0]);
        printf("-f filter -> \"upper\" \"lower\" \"null\"\n");
        exit(EXIT_FAILURE);
    }

    input_fd = open(input_file, O_RDWR | O_CLOEXEC);
    if(input_fd < 0)
    {
        printf("Cannot find input file!\nCreating input file with name %s...\n", input_file);
        input_fd = open(input_file, O_CREAT | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(input_fd < 0)
        {
            printf("Error opening input file\n");
            exit(EXIT_FAILURE);
        }
    }

    if(access(output_file, F_OK) == 0)
    {
        if(remove(output_file) != 0)
        {
            printf("Error deleting old output file\n");
            exit(EXIT_FAILURE);
        }
    }

    output_fd = open(output_file, O_WRONLY | O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(output_fd < 0)
    {
        printf("Error creating output file\n");
        close(input_fd);
        exit(EXIT_FAILURE);
    }

    while((bytes_read = read(input_fd, &buffer, 1)) > 0)
    {
        char transformed_char;
        switch(filter_type)
        {
            case 1:
                transformed_char = upper_filter(buffer);
                break;
            case 2:
                transformed_char = lower_filter(buffer);
                break;
            default:
                transformed_char = null_filter(buffer);
        }

        if(write(output_fd, &transformed_char, 1) != 1)
        {
            printf("Error writing to output file\n");
            close(input_fd);
            close(output_fd);
            exit(EXIT_FAILURE);
        }
    }

    if(bytes_read < 0)
    {
        printf("Error reading input file\n");
    }

    // Close files
    close(input_fd);
    close(output_fd);

    return EXIT_SUCCESS;
}
