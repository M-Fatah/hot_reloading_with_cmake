#include "platform.h"

#include <assert.h>
#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

static char current_executable_directory[PATH_MAX] = {};

inline static void
_string_concat(const char *a, const char *b, char *result)
{
	while (*a != '\0')
		*result++ = *a++;

	while (*b != '\0')
		*result++ = *b++;
}

Platform_Api
platform_api_init(const char *filepath)
{
	Platform_Api self = {};

	// Relative path: libgame.so
	char src_relative_path[128] = {};
	_string_concat(filepath, ".so", src_relative_path);

	// Relative path: libgame.so.tmp
	char dst_relative_path[128] = {};
	_string_concat(src_relative_path, ".tmp", dst_relative_path);

	// Absolute path: %current_executable_directory% + libgame.so
	char src_absolute_path[PATH_MAX] = {};
	_string_concat(current_executable_directory, src_relative_path, src_absolute_path);

	// Absolute path: %current_executable_directory% + libgame.so.tmp
	char dst_absolute_path[PATH_MAX] = {};
	_string_concat(current_executable_directory, src_relative_path, dst_absolute_path);

	[[maybe_unused]] bool copy_successful = platform_file_copy(src_relative_path, dst_relative_path);
	assert(copy_successful && "[PLATFORM]: Failed to copy library.");

	self.handle = ::dlopen(dst_absolute_path, RTLD_LAZY);
	assert(self.handle && "[PLATFORM]: Failed to load library.");

	platform_api_proc proc = (platform_api_proc)::dlsym(self.handle, "platform_api");
	assert(proc && "[PLATFORM]: Failed to get proc platform_api.");

	self.api = proc(nullptr, PLATFORM_API_STATE_INIT);
	assert(self.api && "[PLATFORM]: Failed to get api.");

	struct stat file_stat = {};
	[[maybe_unused]] int32_t stat_result = ::stat(src_relative_path, &file_stat);
	assert(stat_result == 0 && "[PLATFORM]: Failed to get file attributes.");

	self.last_write_time = file_stat.st_mtime;
	::strcpy(self.filepath, src_absolute_path);

	return self;
}

void
platform_api_deinit(Platform_Api *self)
{
	if (self->api)
	{
		platform_api_proc proc = (platform_api_proc)::dlsym(self->handle, "platform_api");
		assert(proc && "[PLATFORM]: Failed to get proc platform_api.");
		self->api = proc(self->api, PLATFORM_API_STATE_DEINIT);
	}

	::dlclose(self->handle);
}

void *
platform_api_load(Platform_Api *self)
{
	char dst_absolute_path[PATH_MAX] = {};
	_string_concat(self->filepath, ".tmp", dst_absolute_path);

	struct stat file_stat = {};
	int32_t stat_result = ::stat(self->filepath, &file_stat);
	assert(stat_result == 0 && "[PLATFORM]: Failed to get file attributes.");

	int64_t last_write_time = file_stat.st_mtime;
	if ((last_write_time == self->last_write_time) || (stat_result != 0))
		return self->api;

	::dlclose(self->handle);

	platform_file_delete(dst_absolute_path);

	platform_sleep(100);

	bool copy_result = platform_file_copy(self->filepath, dst_absolute_path);

	self->handle = ::dlopen(dst_absolute_path, RTLD_LAZY);
	assert(self->handle && "[PLATFORM]: Failed to load library.");

	platform_api_proc proc = (platform_api_proc)::dlsym(self->handle, "platform_api");
	assert(proc && "[PLATFORM]: Failed to get proc platform_api.");

	self->api = proc(self->api, PLATFORM_API_STATE_LOAD);
	assert(self->api && "[PLATFORM]: Failed to get api.");

	// If copying failed we don't update last write time so that we can try copying it again in the next frame.
	if (copy_result == true)
		self->last_write_time = last_write_time;

	return self->api;
}

void
platform_set_cwd_to_process_directory()
{
	char module_path_relative[PATH_MAX + 1];
	::memset(module_path_relative, 0, sizeof(module_path_relative));

	char module_path_absolute[PATH_MAX + 1];
	::memset(module_path_absolute, 0, sizeof(module_path_absolute));

	[[maybe_unused]] int64_t module_path_relative_length = ::readlink("/proc/self/exe", module_path_relative, sizeof(module_path_relative));
	assert((module_path_relative_length != -1 && module_path_relative_length < (int64_t)sizeof(module_path_relative)) && "[PLATFORM]: Failed to get relative path of the current executable.");

	[[maybe_unused]] char *path_absolute = ::realpath(module_path_relative, module_path_absolute);
	assert(path_absolute == module_path_absolute && "[PLATFORM]: Failed to get absolute path of the current executable.");

	char *last_slash = module_path_absolute;
	char *iterator = module_path_absolute;
	while (*iterator++)
	{
		if (*iterator == '/')
			last_slash = ++iterator;
	}
	*last_slash = '\0';

	[[maybe_unused]] int32_t result = ::chdir(module_path_absolute);
	assert(result == 0 && "[PLATFORM]: Failed to set current directory.");
	::strcpy(current_executable_directory, module_path_absolute);
}

bool
platform_file_copy(const char *from, const char *to)
{
	int32_t src_file = ::open(from, O_RDONLY);
	if (src_file < 0)
		return false;

	int32_t dst_file = ::open(to, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (dst_file < 0)
	{
		::close(src_file);
		return false;
	}

	char buffer[8192];
	int64_t total_written = 0;
	while (true)
	{
		int64_t bytes_read = ::read(src_file, buffer, sizeof(buffer));
		if (bytes_read == 0)
			break;

		if (bytes_read == -1)
		{
			::close(src_file);
			::close(dst_file);
			return false;
		}

		int64_t bytes_written = ::write(dst_file, buffer, bytes_read);
		if (bytes_written != bytes_read || bytes_written == 0)
		{
			::close(src_file);
			::close(dst_file);
			return false;
		}

		total_written += bytes_written;
	}

	::close(src_file);
	::close(dst_file);

	if (total_written == 0)
		return false;

	return true;
}

bool
platform_file_delete(const char *filepath)
{
	return ::unlink(filepath) == 0;
}

void
platform_sleep_set_period(uint32_t)
{

}

void
platform_sleep(uint32_t milliseconds)
{
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000 * 1000;
	nanosleep(&ts, 0);
}