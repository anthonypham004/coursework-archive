#include <stdio.h>
#include <string.h>

#include "race_results.h"

#define MAX_CMD_LEN 128

/*
 * This is in general *very* similar to the list_main file seen in lab 2
 * One big difference is the notion of switching between result logs in one run of the program.
 * You have to create or load a result log from a file before you can do things
 * like add, lookup, or write.
 * The code below has to check if result log is NULL before each operation is executed.
 * Also, the user has to explicitly clear the current result log before
 * they can create or load in a new one.
 */
int main(int argc, char **argv) {
    results_log_t *log = NULL;

    // Handle additional arguments on start
    if (argc > 1) {
        // Check for file extension, call appropriate function for reading
        // strstr checks for the presence of an extension
        if (strstr(argv[1], ".txt") != NULL) {
            log = read_results_log_from_text(argv[1]);
            if (log == NULL) {
                printf("Failed to read results log from text file\n");
            } else {
                printf("Results log loaded from text file\n");
            }
        } else if (strstr(argv[1], ".bin") != NULL) {
            log = read_results_log_from_binary(argv[1]);
            if (log == NULL) {
                printf("Failed to read results log from binary file\n");
            } else {
                printf("Results log loaded from binary file\n");
            }
        } else {
            printf("Error: Unknown results log file extension\n");
        }
    }

    printf("CSCI 2021 Race Results Log\n");
    printf("Commands:\n");
    printf("  create <name>:            creates a new log with specified name\n");
    printf("  log:                      shows the name of the active results log\n");
    printf("  add <name> <age> <time>:  adds a new participant\n");
    printf("  lookup <name>:            searches for a race participant by name\n");
    printf("  clear:                    resets current results log\n");
    printf("  print:                    shows all participants in active log\n");
    printf("  write_text:               saves results log to text file\n");
    printf("  read_text <file_name>:    loads results log from text file\n");
    printf("  write_bin:                saves results log to binary file\n");
    printf("  read_bin <file_name>:     loads results log from binary file\n");
    printf("  exit:                     exits the program\n");

    char cmd[MAX_CMD_LEN];
    while (1) {
        printf("results> ");
        if (scanf("%s", cmd) == EOF) {
            printf("\n");
            break;
        }

        if (strcmp("exit", cmd) == 0) {
            break;
        }

        else if (strcmp("create", cmd) == 0) {
            scanf("%s", cmd);    // Read in new log name
            if (log != NULL) {
                printf("Error: You already have an active results log.\n");
                printf("You can remove it with the \'clear\' command\n");
            } else {
                log = create_results_log(cmd);
                if (log == NULL) {
                    printf("Results log creation failed\n");
                }
            }
        }

        // TODO Add cases for other commands
        else if (strcmp("log", cmd) == 0) {    // Get current log's name
            // Nonexisting log case
            if (log == NULL) {
                printf("Error: You must create or load a results log first\n");
                // Print out the existing log's name
            } else {
                // (Made const bc the "not to be modified" made me think I should.)
                const char *name = get_results_log_name(log);
                printf("%s\n", name);
            }
        }

        else if (strcmp("add", cmd) == 0) {
            // Make variables for arguments, read in input
            char name[NAME_LEN];
            int age;
            char time[9];

            scanf("%s %d %s", name, &age, time);
            // Convert formatted time to time in seconds
            int hours;
            int minutes;
            int seconds;
            sscanf(time, "%d:%d:%d", &hours, &minutes, &seconds);
            seconds += (hours * 3600) + (minutes * 60);
            if (log == NULL) {
                printf("Error: You must create or load a results log first\n");
            } else {
                if (add_participant(log, name, age, seconds) == -1) {
                    printf("Error: Participant could not be added\n");
                }
            }

        }

        else if (strcmp("lookup", cmd) == 0) {
            scanf("%s", cmd);
            if (log == NULL) {
                printf("Error: You must create or load a results log first\n");
            } else {
                // Get participant
                const participant_t *participant = find_participant(log, cmd);

                if (participant == NULL) {
                    printf("No participant found with name \'%s\'\n", cmd);
                } else {
                    // Print participant details
                    printf("%s\n", participant->name);
                    printf("Age: %d\n", participant->age);
                    printf("Time: ");
                    print_formatted_time(participant->time_seconds);
                    printf("\n");
                }
            }

        }

        else if (strcmp("clear", cmd) == 0) {
            if (log == NULL) {
                printf("Error: No results log to clear\n");
            } else {
                free_results_log(log);
                log = NULL;
            }
        }

        else if (strcmp("print", cmd) == 0) {
            if (log == NULL) {
                printf("Error: You must create or load a results log first\n");
            } else {
                print_results_log(log);
            }
        }

        else if (strcmp("write_text", cmd) == 0) {
            if (log == NULL) {
                printf("Error: You must create or load a results log first\n");
            } else {
                int result = write_results_log_to_text(log);
                if (result == 0) {
                    // Construct file name for printing
                    char file_name[50];
                    strcpy(file_name, log->name);
                    strcat(file_name, ".txt");
                    printf("Results log successfully written to %s\n", file_name);
                } else {
                    printf("Failed to write results log to text file");
                }
            }
        }

        else if (strcmp("read_text", cmd) == 0) {
            scanf("%s", cmd);
            if (log != NULL) {
                printf("Error: You must clear current results log first\n");
            } else {
                log = read_results_log_from_text(cmd);
                if (log == NULL) {
                    printf("Failed to read results log from text file\n");
                } else {
                    printf("Results log loaded from text file\n");
                }
            }
        }

        else if (strcmp("write_bin", cmd) == 0) {
            if (log == NULL) {
                printf("Error: You must create or load a results log first\n");
            } else {
                int result = write_results_log_to_binary(log);
                if (result == 0) {
                    // Construct file name for printing
                    char file_name[50];
                    strcpy(file_name, log->name);
                    strcat(file_name, ".bin");
                    printf("Results log successfully written to %s\n", file_name);
                } else {
                    printf("Failed to write results log to binary file\n");
                }
            }
        }

        else if (strcmp("read_bin", cmd) == 0) {
            // Read second argument
            scanf("%s", cmd);

            // Check for already existing log
            if (log != NULL) {
                printf("Error: You must clear current results log first\n");
            } else {
                log = read_results_log_from_binary(cmd);
                if (log == NULL) {
                    printf("Failed to read results log from binary file\n");
                } else {
                    printf("Results log loaded from binary file\n");
                }
            }
        }

        else {
            printf("Unknown command %s\n", cmd);
        }
    }

    if (log != NULL) {
        free_results_log(log);
    }
    return 0;
}
