#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <switch.h>

#include "menu.h"

#include "nx/fs.h"
#include "nx/gc.h"

#include "util/dir.h"
#include "util/file.h"
#include "util/log.h"


#define APP_DIR     "sdmc:/switch/benchmark_nx"
#define APP_PATH    "sdmc:/switch/benchmark_nx/benchmark_nx.nro"
//#define DEBUG


bool app_init(void)
{
	#ifdef DEBUG
	init_log();
	#endif

	consoleInit(NULL);
	if (!init_gc())
		return false;
	if (!fs_mount_nand_partition(FsBisPartitionId_User))
		return false;

	return true;
}

void app_exit(void)
{
	#ifdef DEBUG
	exit_log();
	#endif

	consoleExit(NULL);
	exit_gc();
	fs_umount_all_devices();
}

void setup_app_dir(const char *nro)
{
    if (strcmp(APP_PATH, nro) == 0)
        return;
    if (!check_if_dir_exists(APP_DIR))
        create_dir(APP_DIR);
    if (check_if_file_exists(APP_PATH))
        delete_file(APP_PATH);
    move_file(nro, APP_PATH);
}

int main(int argc, char *argv[])
{
	// init everything.
	if (!app_init())
		return 0;
	
	// setup the app dir and move the nro to the folder.
	setup_app_dir(argv[0]);

	// goto the menu.
	menu();

	// cleanup before exiting.
	app_exit();
	return 0;
}