#include <stdint.h>
#include <stdbool.h>
#include <switch.h>

#include "nx/input.h"


bool g_touch_lock = false;


input_t get_input(void)
{
    hidScanInput();
    input_t input = {0};
    input.down = hidKeysDown(CONTROLLER_P1_AUTO);
    input.held = hidKeysHeld(CONTROLLER_P1_AUTO);
    input.t_count = hidTouchCount();
    hidTouchRead(&input.t_pos, 0);
    return input;
}

bool is_touch(void)
{
    input_t input = get_input();
    return input.t_count > 0;
}

int check_if_touch_yesno(const input_t *input)
{
    if (input->t_count)
    {
        for (uint16_t i = 0, x = 255; !g_touch_lock && i < 2; i++, x += 770 / 2)
        {
            if (input->t_pos.px > x && input->t_pos.px < x + (770 / 2) && input->t_pos.py > 210 + 295 - 72 && input->t_pos.py < 210 + 295)
            {
                g_touch_lock = true;
                return i;
            }
        }
        g_touch_lock = true;
    }
    else
    {
        g_touch_lock = false;
    }
    return -1;
}

bool check_if_touch_error(void)
{
    input_t input = get_input();
    if (input.t_count)
    {
        if (!g_touch_lock && input.t_pos.px > 455 && input.t_pos.px < 455 + 365 && input.t_pos.py > 470 && input.t_pos.py < 470 + 65)
            return true;
        g_touch_lock = true;
    }
    else
    {
        g_touch_lock = false;
    }

    if ((input.down & KEY_A || input.down & KEY_B))
        return true;
    return false;
}

int check_if_option(const input_t *input)
{
    if (input->t_count)
    {
        for (uint16_t i = 0, y = 300; !g_touch_lock && i < 3; i++, y += 125)
        {
            if (input->t_pos.px > 475 && input->t_pos.px < 475 + 720 && input->t_pos.py > y && input->t_pos.py < y + 70)
            {
                g_touch_lock = true;
                return i;
            }
        }
        g_touch_lock = true;
    }
    else
    {
        g_touch_lock = false;
    }
    return -1;
}

uint32_t move_cursor_up(uint32_t cursor, uint32_t cursor_max)
{
    return cursor == 0 ? cursor_max - 1 : --cursor;
}

uint32_t move_cursor_down(uint32_t cursor, uint32_t cursor_max)
{
    return cursor == cursor_max - 1 ? 0 : ++cursor;
}