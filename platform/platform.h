#pragma once

#include "export.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PLATFORM_API_STATE
{
	PLATFORM_API_STATE_INIT,
	PLATFORM_API_STATE_DEINIT,
	PLATFORM_API_STATE_LOAD
} PLATFORM_API_STATE;

typedef void * (*platform_api_proc)(void *api, PLATFORM_API_STATE state);

typedef struct Platform_Api
{
	char filepath[4096];
	void *handle;
	void *api;
	int64_t last_write_time;
} Platform_Api;

PLATFORM_API Platform_Api
platform_api_init(const char *filepath);

PLATFORM_API void
platform_api_deinit(Platform_Api *self);

PLATFORM_API void *
platform_api_load(Platform_Api *self);

/**
 * @brief Sets current working directory to process directory.
 */
PLATFORM_API void
platform_set_cwd_to_process_directory();

PLATFORM_API bool
platform_file_copy(const char *from, const char *to);

PLATFORM_API bool
platform_file_delete(const char *filepath);

PLATFORM_API void
platform_sleep_set_period(uint32_t period);

PLATFORM_API void
platform_sleep(uint32_t milliseconds);

#ifdef __cplusplus
}
#endif