#include <stdbool.h>
#include <switch.h>

#include "nx/gc.h"
#include "nx/fs.h"

#include "util/dir.h"
#include "util/file.h"
#include "util/util.h"
#include "util/error.h"
#include "util/log.h"



FsDeviceOperator g_dop = {0};
FsGameCardHandle g_gc_handle = {0};
char g_gc_mount_path[0x300] = {0};


bool init_gc(void)
{
    if (R_FAILED(fs_open_device_operator(&g_dop)))
    {
        write_log("failed to mount gcop\n");
        //ui_display_error_box(ErrorCode_Init_Gc);
        return false;
    }
    return true;
}

void exit_gc(void)
{
    fs_close_device_operator(&g_dop);
    unmount_gc();
}

bool poll_gc(void)
{
    return fs_is_gamecard_inserted(&g_dop);
}

bool mount_gc(void)
{
    if (R_FAILED(fs_get_gamecard_handle_from_device_operator(&g_dop, &g_gc_handle)))
    {
        write_log("failed to get gc handle\n");
        //ui_display_error_box(ErrorCode_Mount_Handle);
        return false;
    }

    if (!fs_mount_gamecard_partition(g_gc_mount_path, g_gc_handle, FsGameCardPartition_Secure))
    {
        write_log("failed to mount gc\n");
        //ui_display_error_box(ErrorCode_Mount_Partition);
        return false;
    }

    return true;
}

bool mount_secure_gc(void)
{
    if (R_FAILED(fs_get_gamecard_handle_from_device_operator(&g_dop, &g_gc_handle)))
    {
        write_log("failed to get gc handle\n");
        return false;
    }

    if (!fs_mount_gamecard_secure(g_gc_handle))
    {
        write_log("failed to mount gc\n");
        return false;
    }

    return true;
}

void __unmount_gc(void)
{
    fs_close_gamecard_handle(&g_gc_handle);
    *g_gc_mount_path = 0;
}

bool unmount_gc(void)
{
    __unmount_gc();
    fs_unmount_device(g_gc_mount_path);
    return true;
}

bool unmount_secure_gc(void)
{
    __unmount_gc();
    fs_unmount_device("GcApp");
    return true;
}

const char *return_gc_mount_path(void)
{
    return g_gc_mount_path;
}