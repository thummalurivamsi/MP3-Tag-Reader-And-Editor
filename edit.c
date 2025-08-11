
/*
Documentation
Name        : Vamsi T
Date        : 30/7/25
Description : MP3 Tag Reader project
*/

#include "mp3_tag_reader.h"
#include <stdio.h>

Status read_and_validate_edit_args(char *argv[], TagOperationInfo *tagopinfo)
{
    printf("🔍 Validating Arguments...\n");

    if (strcmp(argv[2], "-t") == 0)
    {
        tagopinfo->tag_index = 0;
    }
    else if (strcmp(argv[2], "-a") == 0)
    {
        tagopinfo->tag_index = 1;
    }
    else if (strcmp(argv[2], "-A") == 0)
    {
        tagopinfo->tag_index = 2;
    }
    else if (strcmp(argv[2], "-y") == 0)
    {
        tagopinfo->tag_index = 3;
    }
    else if (strcmp(argv[2], "-m") == 0)
    {
        tagopinfo->tag_index = 4;
    }
    else if (strcmp(argv[2], "-c") == 0)
    {
        tagopinfo->tag_index = 5;
    }
    else
    {
        fprintf(stderr, "\n❌ Invalid tag option: '%s'\n", argv[2]);
        return failure;
    }

    // ✅ Check if new value is passed
    if (argv[3])
    {
        // Accept any non-empty string
        if (strlen(argv[3]) == 0)
        {
            fprintf(stderr, "❌ Error: New value for tag cannot be empty\n");
            return failure;
        }

        switch (tagopinfo->tag_index) // 0
        {
        case 0:
        case 1:
        case 2:
        case 4:
        case 5:
            strcpy(tagopinfo->new_value, argv[3]);
            break;

        case 3:
            // Year should be 4 digits
            if (strlen(argv[3]) != 4 || strspn(argv[3], "0123456789") != 4)
            {
                fprintf(stderr, "❌ Error: Year must be a 4-digit number (ex-> 2025)\n");
                return failure;
            }
            strcpy(tagopinfo->new_value, argv[3]);
            break;

        default:
            fprintf(stderr, "❌ Unknown tag type. Cannot apply change\n");
            return failure;
        }
    }
    else
    {
        fprintf(stderr, "❌ Error: No new value provided for tag.n");
        return failure;
    }

    // Check if the filename is passed
    if (argv[4] == NULL)
    {
        fprintf(stderr, "❌ Error: No MP3 file specified\n");
        return failure;
    }
    // Validate .mp3 extension (basic check)
    const char *filename = argv[4];
    int len = strlen(filename);

    if (len < 5 || strcmp(&filename[len - 4], ".mp3") != 0)
    {
        fprintf(stderr, "❌ Error: Invalid file format. Please provide a valid .mp3 file\n");
        return failure;
    }

    // Save filename into structure
    tagopinfo->filename = argv[4];
    printf("✅ MP3 File: %s\n", tagopinfo->filename);
    printf("✅ Arguments validated successfully\n");
    printf("✅ Done\n\n");

    return success;
}
Status edit(TagOperationInfo *tagopinfo)
{
    printf("╔═══════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                           🎧  STARTING MP3 TAG EDITER...✨                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════════╝\n");

    if (open_mp3_file_edit(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to open MP3 file\n");
        return failure;
    }
    printf("✅ Done\n\n");

    if (check_id_and_version(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to detect ID3 tag/version\n");
        return failure;
    }
    printf("✅ Done\n\n");

    if (edit_mp3_tag(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to edit MP3 tags\n");
        return failure;
    }
    printf("✅ Done\n\n");

    if (rename_mp3_file(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to rename file\n");
        return failure;
    }
    printf("✅ Done\n\n");

    close_files(tagopinfo);
    // Update filename in struct
    strcpy(tagopinfo->filename, tagopinfo->filename);
    if (open_mp3_file_view(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to open MP3 file.\n");
        return failure;
    }
    printf("✅ Done\n\n");

    if (view_mp3_tags(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to view MP3 tags.\n");
        return failure;
    }
    printf("✅ Done\n\n");

    return success;
}

Status edit_mp3_tag(TagOperationInfo *tagopinfo)
{
    printf("🔧 Starting MP3 tag edit operation...\n");

    if (copy_first_part(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to copy first part of the file.\n");
        return failure;
    }
    printf("✅ Done\n\n");

    if (modify_tag(tagopinfo) != success) // 🔄 Fixed typo: mpdify_tag → modify_tag
    {
        fprintf(stderr, "❌ Failed to modify the tag.\n");
        return failure;
    }
    printf("✅ Done\n\n");

    if (copy_remaining(tagopinfo) != success)
    {
        fprintf(stderr, "❌ Failed to copy remaining part of the file.\n");
        return failure;
    }
    printf("✅ Done\n\n");

    printf("🎉 MP3 tag edit operation completed successfully\n");
    return success;
}

Status copy_first_part(TagOperationInfo *tagopinfo)
{
    printf("\n📁 Copying first part of the MP3 file...\n");
    rewind(tagopinfo->fptr_mp3); // Start of original MP3 file

    // Copy ID3v2 header (10 bytes)
    char ch;
    for (int i = 0; i < 10; i++)
    {
        if (fread(&ch, 1, 1, tagopinfo->fptr_mp3) != 1 ||
            fwrite(&ch, 1, 1, tagopinfo->fptr_new_mp3) != 1)
        {
            fprintf(stderr, "❌ Error reading/writing ID3 header byte.\n");
            return failure;
        }
    }

    // printf("🔢 tag_index: %d\n", tagopinfo->tag_index);

    for (int i = 0; i < tagopinfo->tag_index; i++)
    {
        char tag[5] = {0};
        if (fread(tag, 4, 1, tagopinfo->fptr_mp3) != 1)
        {
            fprintf(stderr, "❌ Error reading tag identifier.\n");
            return failure;
        }

        tag[4] = '\0'; // null-terminate for safety
        // printf("🏷️  Tag: %s\n", tag);

        // Check for padding or non-frame identifier
        if (tag[0] < 'A' || tag[0] > 'Z')
        {
            printf("🛑 Padding or invalid tag encountered. Stopping tag copy.\n");
            break;
        }

        // Read size
        unsigned char size_bytes[4];
        if (fread(size_bytes, 4, 1, tagopinfo->fptr_mp3) != 1)
        {
            fprintf(stderr, "❌ Error reading size for tag: %s\n", tag);
            return failure;
        }

        unsigned int size = convert_big_endian_to_little_endian(size_bytes);
        // printf("📏 Tag size bytes: %02X %02X %02X %02X\n", size_bytes[0], size_bytes[1], size_bytes[2], size_bytes[3]);
        // printf("📦 Decoded size: %u bytes\n", size);

        // Read flags
        unsigned char flags[2];
        if (fread(flags, 2, 1, tagopinfo->fptr_mp3) != 1)
        {
            fprintf(stderr, "❌ Error reading flags for tag: %s\n", tag);
            return failure;
        }

        // Write tag, size and flags
        fwrite(tag, 4, 1, tagopinfo->fptr_new_mp3);
        fwrite(size_bytes, 4, 1, tagopinfo->fptr_new_mp3);
        fwrite(flags, 2, 1, tagopinfo->fptr_new_mp3);

        // Allocate and copy content
        char *cont = malloc(size);
        if (!cont)
        {
            fprintf(stderr, "❌ Memory allocation failed.\n");
            return failure;
        }

        if (fread(cont, size, 1, tagopinfo->fptr_mp3) != 1)
        {
            fprintf(stderr, "❌ Error reading content for tag: %s\n", tag);
            free(cont);
            return failure;
        }

        if (fwrite(cont, size, 1, tagopinfo->fptr_new_mp3) != 1)
        {
            fprintf(stderr, "❌ Error writing content for tag: %s\n", tag);
            free(cont);
            return failure;
        }

        free(cont);
    }

    printf("✅ First part copied successfully.\n");
    return success;
}

void convert_int_to_big_endian(unsigned int value, unsigned char *bytes)
{
    bytes[0] = (value >> 24) & 0xFF;
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF;
}

Status modify_tag(TagOperationInfo *tagopinfo)
{
    printf("📁 Modifying tag...\n");

    printf("📝 Overwriting tag with new value: %s\n", tagopinfo->new_value);

    char tag[5] = {0};
    if (fread(tag, 4, 1, tagopinfo->fptr_mp3) != 1)
    {
        fprintf(stderr, "❌ Error reading tag identifier.\n");
        return failure;
    }
    tag[4] = '\0';
    // printf("🏷️  Tag: %s\n", tag);

    unsigned char size_bytes[4];
    if (fread(size_bytes, 4, 1, tagopinfo->fptr_mp3) != 1)
    {
        fprintf(stderr, "❌ Error reading size for tag: %s\n", tag);
        return failure;
    }

    unsigned int original_size = convert_big_endian_to_little_endian(size_bytes);
    // printf("📏 Tag size bytes: %02X %02X %02X %02X\n", size_bytes[0], size_bytes[1], size_bytes[2], size_bytes[3]);
    // printf("📦 Original tag size: %u bytes\n", original_size);

    unsigned char flags[3];
    if (fread(flags, 3, 1, tagopinfo->fptr_mp3) != 1)
    {
        fprintf(stderr, "❌ Error reading flags for tag: %s\n", tag);
        return failure;
    }

    // Prepare new tag value
    unsigned int new_size = strlen(tagopinfo->new_value);
    // printf("📦 new size: %u bytes\n", new_size);
    convert_int_to_big_endian(new_size + 1, size_bytes); // reuse same array to write new size

    // Write updated tag to new file
    fwrite(tag, 4, 1, tagopinfo->fptr_new_mp3);
    fwrite(size_bytes, 4, 1, tagopinfo->fptr_new_mp3);
    fwrite(flags, 3, 1, tagopinfo->fptr_new_mp3);
    fwrite(tagopinfo->new_value, new_size, 1, tagopinfo->fptr_new_mp3);

    printf("✅ Tag overwritten successfully\n");

    // Skip the original tag content in the input MP3
    if (fseek(tagopinfo->fptr_mp3, original_size - 1, SEEK_CUR) != 0)
    {
        fprintf(stderr, "❌ Failed to skip old tag content.\n");
        return failure;
    }

    return success;
}

Status copy_remaining(TagOperationInfo *tagopinfo)
{
    printf("📤 Copying remaining part of the MP3 file...\n");

    char ch;
    while (fread(&ch, 1, 1, tagopinfo->fptr_mp3) == 1)
    {
        if (fwrite(&ch, 1, 1, tagopinfo->fptr_new_mp3) != 1)
        {
            fprintf(stderr, "❌ Error writing content to new file.\n");
            return failure;
        }
    }

    // Check for read error (fread returns 0 on EOF or error)
    if (ferror(tagopinfo->fptr_mp3))
    {
        fprintf(stderr, "❌ Error reading from original MP3 file.\n");
        return failure;
    }

    printf("✅ Remaining part copied successfully\n");
    return success;
}
