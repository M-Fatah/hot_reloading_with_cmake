#include "platform.h"

#define NOMINMAX
#include <Windows.h>
#include <assert.h>

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

	char path[128] = {};
	_string_concat(filepath, ".dll", path);

	char path_tmp[128] = {};
	_string_concat(path, ".tmp", path_tmp);

	bool result = ::CopyFileA(path, path_tmp, false);
	assert(result && "[PLATFORM]: Failed to copy library.");

	self.handle = ::LoadLibraryA(path_tmp);
	assert(self.handle && "[PLATFORM]: Failed to load library.");

	platform_api_proc proc = (platform_api_proc)::GetProcAddress((HMODULE)self.handle, "platform_api");
	assert(proc && "[PLATFORM]: Failed to get proc platform_api.");

	self.api = proc(nullptr, PLATFORM_API_STATE_INIT);
	assert(self.api && "[PLATFORM]: Failed to get api.");

	WIN32_FILE_ATTRIBUTE_DATA data = {};
	result = ::GetFileAttributesExA(path, GetFileExInfoStandard, &data);
	assert(result && "[PLATFORM]: Failed to get file attributes.");

	self.last_write_time = *(int64_t *)&data.ftLastWriteTime;
	::strcpy_s(self.filepath, filepath);

	return self;
}

void
platform_api_deinit(Platform_Api *self)
{
	if (self->api)
	{
		platform_api_proc proc = (platform_api_proc)::GetProcAddress((HMODULE)self->handle, "platform_api");
		assert(proc && "[PLATFORM]: Failed to get proc platform_api.");
		self->api = proc(self->api, PLATFORM_API_STATE_DEINIT);
	}

	::FreeLibrary((HMODULE)self->handle);
}

void *
platform_api_load(Platform_Api *self)
{
	char path[128] = {};
	_string_concat(self->filepath, ".dll", path);

	WIN32_FILE_ATTRIBUTE_DATA data = {};
	bool result = ::GetFileAttributesExA(path, GetFileExInfoStandard, &data);

	int64_t last_write_time = *(int64_t *)&data.ftLastWriteTime;
	if ((last_write_time == self->last_write_time) || (result == false))
		return self->api;

	result = ::FreeLibrary((HMODULE)self->handle);
	assert(result && "[PLATFORM]: Failed to free library.");

	char path_tmp[128] = {};
	_string_concat(path, ".tmp", path_tmp);

	bool copy_result = ::CopyFileA(path, path_tmp, false);

	self->handle = ::LoadLibraryA(path_tmp);
	assert(self->handle && "[PLATFORM]: Failed to load library.");

	platform_api_proc proc = (platform_api_proc)::GetProcAddress((HMODULE)self->handle, "platform_api");
	assert(proc && "[PLATFORM]: Failed to get proc platform_api.");

	self->api = proc(self->api, PLATFORM_API_STATE_LOAD);
	assert(self->api && "[PLATFORM]: Failed to get api.");

	// If copying failed we don't update last write time so that we can try copying it again in the next frame.
	if (copy_result)
		self->last_write_time = last_write_time;

	return self->api;
}

void
platform_set_cwd_to_process_directory()
{
	char module_path[1024];
	::GetModuleFileNameA(0, module_path, sizeof(module_path));

	char *last_slash = module_path;
	char *iterator = module_path;
	while (*iterator++)
	{
		if (*iterator == '\\')
			last_slash = ++iterator;
	}
	*last_slash = '\0';

	[[maybe_unused]] bool result = ::SetCurrentDirectoryA(module_path);
	assert(result && "[PLATFORM]: Failed to set current directory.");
}

bool
platform_file_copy(const char *from, const char *to)
{
	return ::CopyFileA(from, to, false);
}

bool
platform_file_delete(const char *filepath)
{
	return ::DeleteFileA(filepath);
}

void
platform_sleep_set_period(uint32_t period)
{
	[[maybe_unused]] MMRESULT result = ::timeBeginPeriod(period);
	assert(result == TIMERR_NOERROR && "[PLATFORM]: Failed to set time begin period.");
}

void
platform_sleep(uint32_t milliseconds)
{
	::Sleep(milliseconds);
}