#include <glib.h>
#include <seed-module.h>

SeedObject
seed_module_init(SeedEngine * eng)
{
  g_print("Hello Seed Module World \n");
  return seed_make_object (eng->context, NULL, NULL);
}
