#include "../platform/application.h"
#include "../platform/platform.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int32_t
main(int32_t, char **)
{
#if defined(PLATFORM_WIN32)
	char dll_name[128] = "game";
#elif defined(PLATFORM_LINUX)
	char dll_name[128] = "libgame";
#endif

	platform_set_cwd_to_process_directory();

	Platform_Api api = platform_api_init(dll_name);
	Application *app = (Application *)api.api;

	if (application_init(app) == false)
	{
		platform_api_deinit(&api);
		return EXIT_FAILURE;
	}

	// Fake loop that updates 60 times per second.
	platform_sleep_set_period(1);
	while (true)
	{
		app = (Application *)platform_api_load(&api);
		if (application_update(app) == false)
			break;

		// Sleep for 16ms.
		platform_sleep((uint32_t)(1000.0f / 60.0f));
	}

	application_deinit(app);
	platform_api_deinit(&api);
	return EXIT_SUCCESS;
}