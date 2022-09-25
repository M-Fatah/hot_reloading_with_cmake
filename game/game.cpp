#include "export.h"

#include "../platform/application.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int CONSOLE_COLOR_COUNT = 8;
static const char *CONSOLE_COLORS[CONSOLE_COLOR_COUNT] = {
	"1;30",
	"1;31",
	"1;32",
	"1;33",
	"1;34",
	"0;37",
	"1;37",
	"1;41"
};

struct Game
{
	int hot_reload_count;
	bool did_hot_reload;
};

bool
_init(Application *app)
{
	Game *self = (Game *)::malloc(sizeof(Game));
	::memset((void *)self, 0, sizeof(Game));
	if (self == nullptr)
		return false;
	self->did_hot_reload = true;
	app->data = self;
	return true;
}

void
_deinit(Application *app)
{
	Game *self = (Game *)app->data;
	::free(self);
}

bool
_update(Application *app)
{
	Game *self = (Game *)app->data;
	if (self->did_hot_reload)
	{
		::printf("\033[%smHot reloading %d.\n\033[0m", CONSOLE_COLORS[rand() % CONSOLE_COLOR_COUNT], self->hot_reload_count);
		self->did_hot_reload = false;
	}
	return true;
}

#ifdef __cplusplus
extern "C" {
#endif

GAME_API void *
platform_api(void *old_api, PLATFORM_API_STATE state)
{
	switch (state)
	{
		case PLATFORM_API_STATE_INIT:
		{
			Application *app = (Application *)::malloc(sizeof(Application));
			if (app == nullptr)
				return nullptr;

			app->init   = _init;
			app->deinit = _deinit;
			app->update = _update;
			return app;
		}
		case PLATFORM_API_STATE_DEINIT:
		{
			if (old_api != nullptr)
			{
				Application *app = (Application *)old_api;
				::free(app);
			}
			return nullptr;
		}
		case PLATFORM_API_STATE_LOAD:
		{
			if (old_api == nullptr)
				return nullptr;

			Application *app = (Application *)old_api;
			app->init   = _init;
			app->deinit = _deinit;
			app->update = _update;

			printf("-----------------HOT RELOADING-----------------\n");
			Game *self = (Game *)app->data;
			self->hot_reload_count++;
			self->did_hot_reload = true;

			return app;
		}
	}
	return nullptr;
}

#ifdef __cplusplus
}
#endif