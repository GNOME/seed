#include <seed.h>

SeedObject
seed_module_init(SeedEngine * eng)
{
  g_printf("Hello Seed Module World \n");
  return seed_make_object (eng->context, NULL, NULL);
}
