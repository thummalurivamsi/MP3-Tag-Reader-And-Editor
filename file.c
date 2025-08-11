/*
Documentation
Name        : Vamsi T
Date        : 30/7/25
Description : MP3 Tag Reader project
*/

#include "mp3_tag_reader.h"
#include <stdio.h>

Status open_mp3_file_view(TagOperationInfo *tagopinfo)
{
    if (tagopinfo == NULL || tagopinfo->filename == NULL || strlen(tagopinfo->filename) == 0)
    {
        fprintf(stderr, "❌ Error: Invalid tag operation info or filename is missing.\n");
        return failure;
    }

    printf("📂 Opening MP3 file: %s\n", tagopinfo->filename);

    // Open the file in binary read mode
    tagopinfo->fptr_mp3 = fopen(tagopinfo->filename, "r");

    if (tagopinfo->fptr_mp3 == NULL)
    {
        fprintf(stderr, "❌ Error: Unable to open file '%s'. Please check if the file exists and is accessible.\n", tagopinfo->filename);
        return failure;
    }

    printf("✅ File opened successfully\n");

    return success;
}

Status open_mp3_file_edit(TagOperationInfo *tagopinfo)
{
    if (tagopinfo == NULL || tagopinfo->filename == NULL || strlen(tagopinfo->filename) == 0)
    {
        fprintf(stderr, "❌ Error: Invalid tag operation info or filename is missing.\n");
        return failure;
    }

    printf("📂 Opening MP3 files: \n");

    // Open the original MP3 file in read+ mode
    tagopinfo->fptr_mp3 = fopen(tagopinfo->filename, "r+");
    if (tagopinfo->fptr_mp3 == NULL)
    {
        fprintf(stderr, "❌ Error: Unable to open file '%s'. Please check if the file exists and is accessible.\n", tagopinfo->filename);
        return failure;
    }
    printf("📄 Original MP3 file opened successfully: %s\n", tagopinfo->filename);

    // Set new filename for modified MP3
    tagopinfo->new_filename = "new.mp3";
    tagopinfo->fptr_new_mp3 = fopen(tagopinfo->new_filename, "w");
    if (tagopinfo->fptr_new_mp3 == NULL)
    {
        fprintf(stderr, "❌ Error: Unable to open file '%s'. Please check if the file exists and is accessible.\n", tagopinfo->filename);
        return failure;
    }
    printf("🆕 New MP3 file opened successfully: %s\n", tagopinfo->new_filename);

    printf("✅ All files opened successfully and ready for editing!\n");

    return success;
}

void close_files(TagOperationInfo *tagopinfo)
{
    printf("\n📁 Closing files... 🔄\n");

    int closed_any = 0;

    if (tagopinfo->fptr_mp3 != NULL)
    {
        printf("📂 Closing MP3 file: %s\n", tagopinfo->filename);
        fclose(tagopinfo->fptr_mp3);
        tagopinfo->fptr_mp3 = NULL;
        printf("✅ File closed successfully! 🎉\n");
        closed_any = 1;
    }

    if (tagopinfo->fptr_new_mp3 != NULL)
    {
        printf("📂 Closing modified MP3 file: %s\n", tagopinfo->new_filename);
        fclose(tagopinfo->fptr_new_mp3);
        tagopinfo->fptr_new_mp3 = NULL;
        printf("✅ Modified file closed successfully! 🎉\n");
        closed_any = 1;
    }

    if (!closed_any)
    {
        printf("⚠️  No files were open to close.\n");
        return;
    }

    printf("✅ All files closed successfully \n\n");
}

Status rename_mp3_file(TagOperationInfo *tagopinfo)
{
    printf("\n🔄 Renaming the file ...\n");

    if (remove(tagopinfo->filename) == 0)
    {
        printf("🗑️  Original MP3 file '%s' removed successfully\n", tagopinfo->filename);
    }
    else
    {
        perror("❌ Failed to remove original MP3 file");
        return failure;
    }

    if (rename(tagopinfo->new_filename, tagopinfo->filename) == 0)
    {
        printf("✏️  Renamed '%s' to 'sample.mp3' successfully.\n", tagopinfo->new_filename);
        printf("✅ File rename operation completed successfully\n");
        return success;
    }
    else
    {
        perror("❌ Failed to rename file");
        return failure;
    }
}
