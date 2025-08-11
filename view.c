/*
Documentation
Name        : Vamsi T
Date        : 30/7/25
Description : MP3 Tag Reader project
*/

#include "mp3_tag_reader.h"
#include <stdio.h>

OperationType check_operation_type(char *argv[])
{
    // Validate input
    if (argv == NULL || argv[1] == NULL)
        return OP_INVALID;

    // View operation
    if (strcmp(argv[1], "-v") == 0)
        return OP_VIEW;

    // Edit operation
    else if (strcmp(argv[1], "-e") == 0)
        return OP_EDIT;

    // Help flag
    else if (strcmp(argv[1], "--help") == 0)
        return OP_HELP;

    // Invalid or unsupported argument
    return OP_INVALID;
}

Status read_and_validate_view_args(char *argv[], TagOperationInfo *tagopinfo)
{
    printf("🔍 Validating Arguments...\n");
    // Check if the filename is passed
    if (argv[2] == NULL)
    {
        fprintf(stderr, "❌ Error: No MP3 file specified\n");
        return failure;
    }

    // Validate .mp3 extension (basic check)
    const char *filename = argv[2]; // sample.mp3
    int len = strlen(filename);     // 10b

    if (len < 5 || strcmp(&filename[len - 4], ".mp3") != 0)
    {
        fprintf(stderr, "❌ Error: Invalid file format. Please provide a valid .mp3 file\n");
        return failure;
    }

    // Save filename into structure
    tagopinfo->filename = argv[2];
    printf("✅ MP3 File: %s\n", tagopinfo->filename);
    printf("✅ Arguments validated successfully\n");
    printf("✅ Done\n\n");

    return success;
}

Status view(TagOperationInfo *tagopinfo)
{
    printf("╔═══════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                           🎧  STARTING MP3 TAG VIEWER...✨                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════════╝\n");

    if (open_mp3_file_view(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to open MP3 file.\n");
        return failure;
    }
    printf("✅ Done\n\n");

    if (check_id_and_version(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to detect ID3 tag/version\n");
        return failure;
    }
    printf("✅ Done\n\n");

    if (view_mp3_tags(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to view MP3 tags.\n");
        return failure;
    }
    printf("✅ Done\n\n");

    // printf("🏁 MP3 Tag Viewer Completed\n");
    return success;
}

Status check_id_and_version(TagOperationInfo *tagopinfo)
{
    char header[5];
    rewind(tagopinfo->fptr_mp3); // Go to start of the file

    if (fread(header, sizeof(char), 5, tagopinfo->fptr_mp3) != 5)
    {
        fprintf(stderr, "❌ Error: Could not read ID3 header.\n");
        return failure;
    }

    if (strncmp(header, "ID3", 3) == 0)
    {
        printf("🟢 ID3v2 tag found. Version: %d.%d\n", header[3], header[4]);
        return success;
    }

    printf("❌ No valid ID3 tag found. Aborting tag read\n");
    return failure;
}

Status view_mp3_tags(TagOperationInfo *tagopinfo)
{
    printf("🎼 Viewing MP3 Tags...\n\n");

    rewind(tagopinfo->fptr_mp3);              // Start of file
    fseek(tagopinfo->fptr_mp3, 10, SEEK_SET); // Skip ID3 header

    printf("═══════════════════════════════════════════════════════════════════════════════════\n");

    for (int i = 0; i < 6; i++)
    {
        char tag[5] = {0};
        if (fread(tag, 4, 1, tagopinfo->fptr_mp3) != 1) // TIT2
        {
            fprintf(stderr, "❌ Error reading tag identifier.\n");
            return failure;
        }
        tag[4] = '\0';

        // Padding check
        if (tag[0] < 'A' || tag[0] > 'Z')
            break;

        unsigned char size_bytes[4];
        if (fread(size_bytes, 4, 1, tagopinfo->fptr_mp3) != 1)
        {
            fprintf(stderr, "❌ Error reading size for tag: %s\n", tag);
            return failure;
        }
        unsigned int size = convert_big_endian_to_little_endian(size_bytes);

        fseek(tagopinfo->fptr_mp3, 3, SEEK_CUR); // Skip flags

        char *cont = (char *)malloc(size + 1); // char buffer[size];
        if (cont == NULL)
        {
            fprintf(stderr, "❌ Memory allocation failed.\n");
            return failure;
        }

        if (fread(cont, size - 1, 1, tagopinfo->fptr_mp3) != 1)
        {
            fprintf(stderr, "❌ Error reading content for tag: %s\n", tag);
            free(cont);
            return failure;
        }
        cont[size] = '\0';                      // Null-terminate
        compare_view_tags(tag, size - 1, cont); // Call your tag print handler

        free(cont);
    }

    printf("═══════════════════════════════════════════════════════════════════════════════════\n");
    printf("✅ MP3 Tag viewing completed\n");
    return success;
}

void compare_view_tags(char tag[], int size, char cont[])
{
    const char *display_labels[6] = {"🎼 Title     ", "🎤 Artist    ", "💿 Album     ", "📅 Year      ", "🎼 Genre     ", "💬 Comment   "};
    const char *tag_ids[6] = {"TIT2", "TPE1", "TALB", "TYER", "TCON", "COMM"};

    for (int i = 0; i < 6; i++)
    {
        if (strcmp(tag, tag_ids[i]) == 0)
        {

            printf(" %s: ", display_labels[i]);
            print(cont, size); // Calls print function for clean output
            printf("\n");
            break;
        }
    }
}

unsigned int convert_big_endian_to_little_endian(unsigned char *bytes)
{

    // General integer	(b0 << 24)
    // ID3 synchsafe int(b0 << 21)

    return (bytes[0] << 21) | (bytes[1] << 14) | (bytes[2] << 7) | (bytes[3]);
    // Because normal 32-bit integers can contain the byte 0xFF, which might confuse MP3 parsers by mimicking sync signals.
    // Synchsafe integers avoid this by ensuring no byte ever has its high bit set.
}

void print(const char *cont, int size)
{
    int start = 0;

    for (int i = start; i < size; i++)
    {
        unsigned char ch = cont[i];
        if (ch >= 32 && ch <= 126) // Printable characters
        {
            putchar(ch);
        }
        /*
        This condition excludes:
        \n (ASCII 10)
        \r (ASCII 13)
        \t (ASCII 9)
        */
        else if (ch == '\n' || ch == '\r' || ch == '\t') // Line breaks (\n, \r) Tab spaces (\t)
        {
            putchar(ch);
        }
        else
        {
            // Skip or replace unprintable characters
            putchar('.');
        }
    }
}
