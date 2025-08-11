/*
Documentation
Name        : Vamsi T
Date        : 30/7/25
Description : MP3 Tag Reader project
*/

#include "mp3_tag_reader.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    TagOperationInfo tagopinfo;
    tagopinfo.fptr_mp3 = NULL;
    tagopinfo.fptr_new_mp3 = NULL;

    // 📌 Check for minimum argument count
    if (argc < 2)
    {
        print_usage(); // 📘 Show usage instructions
        return 0;
    }
    if (argc > 5)
    {
        printf("⚠️  Warning: Tag content may contain spaces. Enclose it in quotes\n");
        printf("📥 Example: ./a.out -e -t \"Vamsi Thummaluri\" sample.mp3\n");
        return 0;
    }

    tagopinfo.op_type = check_operation_type(argv);
    // 🆘 Handle --help operation
    if (tagopinfo.op_type == OP_HELP)
    {
        if (argc == 2)
            print_help(); // 📖 Display help content
        else
            print_usage();
        return 0;
    }

    // 👁️ View metadata operation
    if (tagopinfo.op_type == OP_VIEW)
    {
        if (read_and_validate_view_args(argv, &tagopinfo) == success)
        {
            if (view(&tagopinfo) == success)
            {
                // ✅ Successfully viewed tags
                close_files(&tagopinfo);
                printf("✅🎧 ALL TAGS SUCCESSFULLY DISPLAYED! 🎧✨\n");
            }
            else
            {
                close_files(&tagopinfo);
                fprintf(stderr, "❌ Failed to view tags.\n");
            }
        }
        else
        {
            fprintf(stderr, "❌ Invalid arguments for view operation.\n");
            print_usage();
        }
    }

    // ✏️ Edit metadata operation
    else if (tagopinfo.op_type == OP_EDIT)
    {
        // 📌 Ensure minimum required arguments are provided
        if (argc < 5)
        {
            print_usage(); // 📘 Display usage instructions to the user
            return 0;
        }

        // 🔍 Validate edit arguments
        if (read_and_validate_edit_args(argv, &tagopinfo) == success)
        {
            // 🛠️ Attempt to perform tag editing
            if (edit(&tagopinfo) == success)
            {
                close_files(&tagopinfo);
                printf("\n✅ Tag edited & Displayed successfully!\n");
            }
            else
            {
                close_files(&tagopinfo);
                fprintf(stderr, "\n❌ Error: Failed to edit the tag\n");
            }
        }
        else
        {
            fprintf(stderr, "\n❌ Error: Invalid arguments supplied for edit operation\n");
            print_usage();
        }
    }

    // ⚠️ Invalid operation
    else
    {
        fprintf(stderr, "⚠️ Unknown operation type. Use --help for guidance\n");
        print_usage();
    }

    return 0;
}

void print_usage()
{
    printf("-----------------------------------------------------------------------------------------------\n\n");
    printf("❌ ERROR: ./a.out : INVALID ARGUMENTS\n\n");
    printf("📌 USAGE GUIDE:\n");
    printf("   To view please pass like    : ./a.out -v <mp3filename>\n");
    printf("   To edit please pass like    : ./a.out -e -t/-a/-A/-m/-y/-c <changing text> <mp3filename>\n");
    printf("   To get help pass like       : ./a.out --help\n");
    // printf("\n💡 Tip: Use double quotes for values with spaces!\n");
    printf("\n-----------------------------------------------------------------------------------------------\n");
}

void print_help()
{
    printf("\n                                  🛠️  HELP MENU🛠️                                 \n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                           🎧 MP3 TAG READER & EDITOR🎧                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════════╝\n");

    printf("\n🧭 USAGE:\n");
    printf("  🔍 View tags : ./a.out -v <mp3_filename>\n");
    printf("  ✏️  Edit tags : ./a.out -e <option> <changing text> <mp3_filename>\n");
    printf("  🆘 Help       : ./a.out --help\n");

    printf("\n🎯 TAG OPTIONS FOR EDITING:\n");
    printf("  -t   ->  🎼 Title\n");
    printf("  -a   ->  🎤 Artist\n");
    printf("  -A   ->  💿 Album\n");
    printf("  -y   ->  🗓️  Year\n");
    printf("  -m   ->  💬 Comment\n");
    printf("  -c   ->  🎚️  Genre\n");

    printf("\n📂 EXAMPLES:\n");
    printf("  ./a.out -v mysong.mp3\n");
    printf("  ./a.out -e -t \"New Content\" song.mp3\n");

    printf("═══════════════════════════════════════════════════════════════════════════════════\n");
}
