#ifndef BUS_H
#define BUS_H

#include <stdbool.h>
#include <gdbm.h>
#include <time.h>

#define MAX_STRING 1024
#define ROWS 16
#define COLS 4
#define MAX_TRIPS 10
#define BUS_DATABASE "./bus_data.gdbm"
#define RES_DATABASE "./res_data.gdbm"
#define DIVIDER "==================================================\n"

typedef struct {
    char trip_id[MAX_STRING]; // of Trip -> trip_id
    int seat_row;
    char seat_col; // A-D
    int col_idx; // index representation of column
    char reservation_id[MAX_STRING]; // trip_id + reservation number order (ex: DAL-CHI-123-1)
    char first_name[MAX_STRING];
    char last_name[MAX_STRING];
} Reservation;

typedef struct {
    char seat_id[4]; // based on bus row (1-8) and letter seat of that row (A-D)
    bool is_reserved;
    Reservation reservation; // reservation that's holding this seat
} Seat;

typedef struct {
    char trip_id[MAX_STRING]; // ex: DAL-CHI-123
    char bus_number[4]; // three digit bus number
    char origin[MAX_STRING];
    char destination[MAX_STRING];
    Seat seats[ROWS][COLS]; // 16 rows of 4 seats labeled A, B, C, and D
} Trip;

extern Trip trips[MAX_TRIPS];
extern int trip_count;

// Reservation functions
void create_reservation();
void view_reservation();
void update_reservation();
void delete_reservation();
int save_res_to_db(Reservation *res);

// Trip functions
void create_trip();
void view_trip();
void update_trip();
void delete_trip();
Trip *get_trip();
int save_trip_to_db(Trip *trip);

// Utility functions
void seat_menu();
void format_seat_map(Trip *trip, char *out_str);
void print_res_keys();
void print_db_keys();
void print_db_keys_pretty();

#endif