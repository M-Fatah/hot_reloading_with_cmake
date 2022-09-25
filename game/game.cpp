#include "export.h"

#include "../platform/application.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Game
{
	int hot_reload_count = 0;
};

bool
_init(Application *app)
{
	Game *self = (Game *)::malloc(sizeof(Game));
	::memset((void *)self, 0, sizeof(Game));
	if (self == nullptr)
		return false;

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
	printf("Hot reloading %d.\n", self->hot_reload_count);
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

			return app;
		}
	}
	return nullptr;
}

#ifdef __cplusplus
}
#endif