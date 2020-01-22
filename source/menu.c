#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <switch.h>

#include "benchmark.h"
#include "nx/input.h"
#include "nx/gc.h"


#define TITLE_HEADER    "Benchmark NX\n"
#define SPACERS         "-------------------------------------------------------------------------------"
#define OPTION_LIST_MAX 11

const char *g_option_list[] =
{
    "!READ:",
    "*Nand",
    "*SD card",
    "*Gamecard",

    "-",

    "!WRITE:",
    "*Nand",
    "*SD card",

    "-",

    "!MISC:",
    "*Exit",
};


uint8_t g_cursor = 0;
uint8_t g_list_cursor = 0;
bool g_gamecard_mounted = false;


void update_list_cursor(void)
{
    for (uint8_t i = 0, c_counter = 0; i < OPTION_LIST_MAX; i++)
    {
        char c = g_option_list[i][0];
        if (c == '-' || c == '!')
            continue;

        if (c_counter == g_cursor)
        {
            g_list_cursor = i;
            break;
        }
        else
        {
            c_counter++;
        }
    }
}

void print_option_list(void)
{
    for (uint8_t i = 0; i < OPTION_LIST_MAX; i++)
    {
        if (g_option_list[i][0] == '-')
        {
            printf("\n");
            continue;
        }

        if (g_option_list[i][0] == '!')
        {
            printf("\n%s\n\n", g_option_list[i] + 1);
            continue;
        }

        printf("%s%s\n\n", i == g_list_cursor ? "> " : "  ", g_option_list[i] + 1);
    }
    printf("\n");
}

void print_menu(void)
{
    printf("%s\n", TITLE_HEADER);
    printf("%s\n\n", SPACERS);
    print_option_list();
    printf("%s\n\n", SPACERS);
}

void print_display(const char *message)
{
    printf(message);
    consoleUpdate(NULL);
}

void print_lock(const char *message)
{
    print_display(message);
    print_display("Press B to continue...\n");
    while (appletMainLoop())
    {
        input_t input = get_input();
        if (input.down & KEY_B) break;
    }
}

void update_gamecard(void)
{
    if (g_gamecard_mounted != poll_gc() && g_gamecard_mounted ? unmount_secure_gc() : mount_secure_gc()) g_gamecard_mounted = !g_gamecard_mounted;
}

int handle_input(void)
{
    input_t input = get_input();

    if (input.down & KEY_UP)
    {
        g_cursor = move_cursor_up(g_cursor, 6);
        update_list_cursor();
    }
    
    if (input.down & KEY_DOWN)
    {
        g_cursor = move_cursor_down(g_cursor, 6);
        update_list_cursor();
    }

    if (input.down & KEY_A)
    {
        if (g_cursor == 5) return -1;
        if (g_cursor == 2 && !g_gamecard_mounted) return 0;
        if (!benchmark(g_cursor, 0x800000, 0x80000000))
        {
            print_lock("An error occured\n\n");
        }
    }

    if (input.down & KEY_B)
    {
        return -1;
    }

    return 0;
}

void menu(void)
{
    update_list_cursor();

    while (appletMainLoop())
    {
        update_gamecard();

        if (handle_input() == -1)
        {
            break;
        }

        consoleClear();
        print_menu();
        consoleUpdate(NULL);
    }
}