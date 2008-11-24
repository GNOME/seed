#include <glib.h>
#include <glib-object.h>
#include <seed.h>

void seed_module_init(SeedEngine * eng)
{
	g_printf("Hello Seed Module World \n");
}
