#ifndef MP3_TAG_READER_H
#define MP3_TAG_READER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    success,
    failure
} Status;

typedef enum
{
    OP_HELP,
    OP_VIEW,
    OP_EDIT,
    OP_INVALID
} OperationType;

// ID3v1 tag structure - 128 bytes
typedef struct
{
    char tag[3];         // Should contain "TAG"
    char title[30];      // Title
    char artist[30];     // Artist
    char album[30];      // Album
    char year[4];        // Year
    char comment[30];    // Comment
    unsigned char genre; // Genre byte
} ID3Tag;

// Holds user inputs and operational data
typedef struct
{
    OperationType op_type;
    int tag_index;
    char new_value[50];

    // original file name
    char *filename; // MP3 file name
    FILE *fptr_mp3;

    // new file name
    char *new_filename; // MP3 file name
    FILE *fptr_new_mp3;
} TagOperationInfo;

// Utility
void print_usage();
void print_help();

// Validation & Argument Handling
OperationType check_operation_type(char *argv[]);
Status read_and_validate_view_args(char *argv[], TagOperationInfo *tagopinfo);

// View Operation
Status check_id_and_version(TagOperationInfo *tagopinfo);
Status view_mp3_tags(TagOperationInfo *tagopinfo);
Status view(TagOperationInfo *tagopinfo);
void compare_view_tags(char tag[], int size, char cont[]);
unsigned int convert_big_endian_to_little_endian(unsigned char *bytes);
void print(const char *cont, int size);

// Edit Operation
Status edit_mp3_tag(TagOperationInfo *tagopinfo);
Status read_mp3_tag(TagOperationInfo *tagopinfo);
Status read_and_validate_edit_args(char *argv[], TagOperationInfo *tagopinfo);
Status edit(TagOperationInfo *tagopinfo);
void compare_edit_tags(char tag[], int size, char cont[], TagOperationInfo *tagopinfo);
void convert_int_to_big_endian(unsigned int value, unsigned char *bytes);
Status copy_first_part(TagOperationInfo *tagopinfo);
Status modify_tag(TagOperationInfo *tagopinfo);
Status copy_remaining(TagOperationInfo *tagopinfo);

// File I/O
Status open_mp3_file_view(TagOperationInfo *tagopinfo);
Status open_mp3_file_edit(TagOperationInfo *tagopinfo);
Status rename_mp3_file(TagOperationInfo *tagopinfo);
void close_files(TagOperationInfo *tagopinfo);
Status read_id3_tag(FILE *fp, ID3Tag *tag);

#endif // MP3_TAG_READER_H
