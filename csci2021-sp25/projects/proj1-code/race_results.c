#include "race_results.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This is the (somewhat famous) djb2 hash
unsigned hash(const char *str) {
    unsigned hash_val = 5381;
    int i = 0;
    while (str[i] != '\0') {
        hash_val = ((hash_val << 5) + hash_val) + str[i];
        i++;
    }
    return hash_val % NUM_BUCKETS;
}

results_log_t *create_results_log(const char *log_name) {
    results_log_t *log = malloc(sizeof(results_log_t));
    if (log == NULL) {
        return NULL;
    }

    strcpy(log->name, log_name);
    log->size = 0;
    for (int i = 0; i < NUM_BUCKETS; i++) {
        log->buckets[i] = NULL;
    }

    return log;
}

const char *get_results_log_name(const results_log_t *log) {
    // Empty log case
    if (log == NULL) {
        return NULL;
    } else {
        return log->name;
    }
}

int add_participant(results_log_t *log, const char *name, unsigned age, unsigned time_seconds) {
    // Nonexistent log handling
    if (log == NULL) {
        return -1;
    }
    // Full log handling
    if (log->size >= NUM_BUCKETS) {
        return -1;
    }

    participant_t *new_participant = malloc(sizeof(participant_t));

    // Create a hash table index from the name's hash value
    int hash_value = hash(name);
    int hash_index = hash_value;

    // Assign parameters to struct attributes
    strcpy(new_participant->name, name);
    new_participant->age = age;
    new_participant->time_seconds = time_seconds;

    // Insert participant into corresponding bucket in hash table.
    // Perform linear probing if necessary.
    while (log->buckets[hash_index] != NULL && hash_index < NUM_BUCKETS + 1) {
        hash_index++;

        // In the case where wrap around is needed during probing
        if (hash_index + 1 == NUM_BUCKETS + 1) {
            hash_index = 0;
        }
        // If index meets original hash value, that means a full loop-around has been done and no
        // empty buckets are available
        if (hash_index == hash_value) {
            free(new_participant);    // Free memory reserved for new_participant
            return -1;
        }
    }

    // Final check for bucket, assign participant values to bucket
    if (log->buckets[hash_index] == NULL) {
        log->buckets[hash_index] = new_participant;
        log->size++;    // Incrememnt log size
    }

    return 0;
}

const participant_t *find_participant(const results_log_t *log, const char *name) {
    // Obtain the name's corresponding hash value
    int hash_value = hash(name);

    // Check if hash's value is in bounds
    if (hash_value > NUM_BUCKETS - 1 || hash_value < 0) {
        return NULL;
    }

    // Compares name at bucket to name passed in (to account for entries gone through linear
    // probing)
    int hash_index = hash_value;

    // Traverse table until bucket w/ matching name is found (or upper table bound is hit)
    while (hash_index < NUM_BUCKETS && log->buckets[hash_index] != NULL &&
           strcmp(log->buckets[hash_index]->name, name) != 0) {
        hash_index++;

        // Loop around once upper bound is met
        if (hash_index == NUM_BUCKETS) {
            hash_index = 0;
        }
        // Full loop is met, terminate process and return null
        if (hash_index == hash_value) {
            return NULL;
        }
    }

    // Null value cases
    if (hash_index >= NUM_BUCKETS || log->buckets[hash_index] == NULL) {
        return NULL;
    }

    // Return participant at bucket
    return log->buckets[hash_index];
}

void print_formatted_time(unsigned time_seconds) {
    unsigned hours = time_seconds / (60 * 60);
    time_seconds %= (60 * 60);
    unsigned minutes = time_seconds / 60;
    time_seconds %= 60;
    printf("%u:%02u:%02u", hours, minutes, time_seconds);
}

void print_results_log(const results_log_t *log) {
    // Attribute variables
    char name[NAME_LEN];
    int age;
    int time_seconds;

    // Print out log label
    printf("%s Results\n", log->name);

    // Go through table linearly
    int print_counter = 0;
    for (int i = 0; i <= NUM_BUCKETS; i++) {
        // Break out of loop early once all participant data has been printed
        if (print_counter == log->size) {
            break;
        }

        // For each occupied bucket, print out the details in bucket
        if (log->buckets[i] != NULL) {
            // Assign variables
            strcpy(name, log->buckets[i]->name);
            age = log->buckets[i]->age;
            time_seconds = log->buckets[i]->time_seconds;

            // Print attributes
            printf("Name: %s\n", name);
            printf("Age: %d\nTime: ", age);
            print_formatted_time(time_seconds);
            printf("\n");
            print_counter++;
        }
    }
}

void free_results_log(results_log_t *log) {
    if (log == NULL) {
        return;
    }
    // Go through each bucket for freeing, set log to NULL at the end
    for (int i = 0; i < NUM_BUCKETS; i++) {
        if (log->buckets[i] != NULL) {
            free(log->buckets[i]);
        }
    }
    free(log);
}

int write_results_log_to_text(const results_log_t *log) {
    char file_name[NAME_LEN + strlen(".txt")];
    strcpy(file_name, log->name);
    strcat(file_name, ".txt");

    FILE *f = fopen(file_name, "w");
    if (f == NULL) {
        return -1;
    }

    fprintf(f, "%u\n", log->size);
    for (int i = 0; i < NUM_BUCKETS; i++) {
        participant_t *participant = log->buckets[i];
        if (participant != NULL) {
            fprintf(f, "%s %u %u\n", participant->name, participant->age,
                    participant->time_seconds);
        }
    }

    fclose(f);
    return 0;
}

results_log_t *read_results_log_from_text(const char *file_name) {
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        return NULL;
    }

    // Create a new log from name (and truncate out the extension)
    char log_name[strlen(file_name) + 1];
    strcpy(log_name, file_name);
    log_name[strlen(log_name) - 4] = '\0';    // Nullify the last 4 chars (.txt)

    results_log_t *log = create_results_log(log_name);

    // Read number representing number of participants
    int participant_count;
    fscanf(f, "%d", &participant_count);

    // Assign participant values, add to log
    char name[NAME_LEN];
    int age;
    int time_seconds;

    for (int i = 0; i < participant_count; i++) {
        fscanf(f, "%s %d %d", name, &age, &time_seconds);
        add_participant(log, name, age, time_seconds);
    }

    fclose(f);
    return log;
}

int write_results_log_to_binary(const results_log_t *log) {
    // Make file name from log name
    char file_name[NAME_LEN + strlen(".bin") + 1];
    strcpy(file_name, log->name);
    strcat(file_name, ".bin");

    // Create file, check validity of file
    FILE *f = fopen(file_name, "wb");
    if (f == NULL) {
        return -1;
    }

    // Write integer representing log size
    fwrite(&log->size, sizeof(int), 1, f);

    // Scanning through the hash table, grab the index of each occupied bucket
    int indices[NUM_BUCKETS];    // Stores occupied bucket indices
    int p_counter = 0;           // For index array storing and early loop termination
                                 // (makes it more efficient I think)
    for (int i = 0; i < NUM_BUCKETS; i++) {
        if (log->buckets[i] != NULL) {
            indices[p_counter] = i;    // Take i (occupied index) and store it in indices
            p_counter++;
        }
    }

    // Go through each bucket in indices and write its data
    int curr_index;
    participant_t *curr_p;
    for (int i = 0; i < p_counter; i++) {
        // Get the current participant i'th index from indices represents
        curr_index = indices[i];
        curr_p = log->buckets[curr_index];

        // To avoid potenially operating on null participant_t
        if (curr_p != NULL) {
            // Write participant name size
            int name_len = strlen(curr_p->name);
            fwrite(&name_len, sizeof(int), 1, f);

            // Write participant name
            fwrite(curr_p->name, sizeof(char), name_len, f);

            // Write participant age
            fwrite(&curr_p->age, sizeof(int), 1, f);

            // Write time in seconds
            fwrite(&curr_p->time_seconds, sizeof(int), 1, f);
        }
    }
    fclose(f);
    return 0;
}

results_log_t *read_results_log_from_binary(const char *file_name) {
    // Create file object
    FILE *f = fopen(file_name, "rb");
    if (f == NULL) {
        return NULL;
    }
    // Create log
    char log_name[strlen(file_name) + 1];
    strcpy(log_name, file_name);
    log_name[strlen(log_name) - 4] = '\0';
    results_log_t *log = create_results_log(log_name);

    // Read number of participants
    int size;
    fread(&size, sizeof(int), 1, f);

    // Add each participant to log
    // Variables for participant details
    int curr_name_len;
    char name[NAME_LEN + 1];
    int age;
    int time_seconds;
    for (int i = 0; i < size; i++) {
        fread(&curr_name_len, sizeof(int), 1, f);       // Store name length
        fread(name, sizeof(char), curr_name_len, f);    // Read in name
        name[curr_name_len] = '\0';                     // Add on null terminator
        fread(&age, sizeof(int), 1, f);                 // Read in age
        fread(&time_seconds, sizeof(int), 1, f);        // Read in time in seconds

        // Safely copy name value to pass in
        char *name_copy =
            malloc(curr_name_len + 1);    // Allocate temporary memory for copy and null terminator
        strcpy(name_copy, name);
        add_participant(log, name_copy, age, time_seconds);
        free(name_copy);    // Free memory
    }
    fclose(f);
    return log;
}
