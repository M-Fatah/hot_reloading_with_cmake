#pragma once

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Application
{
	void *data;
	bool (*init)  (Application *self);
	void (*deinit)(Application *self);
	bool (*update)(Application *self);
} Application;

inline static bool
application_init(Application *self)
{
	return self->init(self);
}

inline static void
application_deinit(Application *self)
{
	self->deinit(self);
}

inline static bool
application_update(Application *self)
{
	return self->update(self);
}

#ifdef __cplusplus
}
#endif