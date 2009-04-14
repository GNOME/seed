#include <seed.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

SeedObject os_namespace;

#define EXPECTED_EXCEPTION(name, argnum) \
  gchar *mes = g_strdup_printf (name " expected " argnum " got %d", argument_count); \
  seed_make_exception (ctx, exception, "ArgumentError", mes); \
  g_free (mes); \
  return seed_make_null (ctx);

SeedValue
seed_os_chdir (SeedContext ctx,
	       SeedObject function,
	       SeedObject this_object,
	       size_t argument_count,
	       const SeedValue arguments[], 
	       SeedException * exception)
{
  gchar *arg;
  gint ret;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.chdir", "1 argument");
    }
  arg = seed_value_to_string (ctx, arguments[0], exception);
  ret = chdir (arg);
  g_free (arg);
  
  return seed_value_from_int (ctx, ret, exception);
}

SeedValue
seed_os_fchdir (SeedContext ctx,
	       SeedObject function,
	       SeedObject this_object,
	       size_t argument_count,
	       const SeedValue arguments[], 
	       SeedException * exception)
{
  gint ret, arg;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.fchdir", "1 argument");
    }
  arg = seed_value_to_int (ctx, arguments[0], exception);
  ret = fchdir (arg);
  
  return seed_value_from_int (ctx, ret, exception);
}

SeedValue
seed_os_getcwd (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  SeedValue seed_ret;
  gchar *ret;
  
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getcwd", "no arguments");
    }
  ret = getcwd (NULL, 0);
  seed_ret = seed_value_from_string (ctx, ret, exception);
  g_free (ret);
  
  return seed_ret;
}

SeedValue
seed_os_ctermid (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  SeedValue seed_ret;
  gchar *ret;
  
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getcwd", "no arguments");
    }
  // ctermid returns a static buffer
  ret = ctermid (NULL);
  seed_ret = seed_value_from_string (ctx, ret, exception);
  
  return seed_ret;
}

SeedValue
seed_os_getegid (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  gid_t ret;
  
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getegid", "no arguments");
    }
  ret = getegid ();

  return seed_value_from_long (ctx, (glong) ret, exception);
}

SeedValue
seed_os_geteuid (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  uid_t ret;
  
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.geteuid", "no arguments");
    }
  ret = geteuid ();

  return seed_value_from_long (ctx, (glong) ret, exception);
}

SeedValue
seed_os_getgid (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  SeedValue seed_ret;
  gid_t ret;
  
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getgid", "no arguments");
    }
  ret = getgid ();

  return seed_value_from_long (ctx, (glong) ret, exception);
}

SeedValue
seed_os_getuid (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  uid_t ret;
  
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getuid", "no arguments");
    }
  ret = getuid ();

  return seed_value_from_long (ctx, (glong) ret, exception);
}

SeedValue
seed_os_getgroups (SeedContext ctx,
		   SeedObject function,
		   SeedObject this_object,
		   size_t argument_count,
		   const SeedValue arguments[], 
		   SeedException * exception)
{
  SeedValue ret;
  SeedValue *groups;
  gid_t *group_list;
  guint num_groups, i;
  
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getgroups", "no arguments");
    }
  num_groups = getgroups(0, NULL);
  group_list = g_alloca (num_groups * sizeof(gid_t));
  groups = g_alloca (num_groups * sizeof(SeedValue));
  if (getgroups (num_groups, group_list) < 0)
    {
      // TODO: Decide on how to handle exceptions for things like this...
      // Investigate python
      return seed_make_null (ctx);
    }
  
  for (i = 0; i < num_groups; i++)
    {
      groups[i] = seed_value_from_long (ctx, (glong) group_list[i], exception);
    }
  ret = seed_make_array (ctx, groups, num_groups, exception);

  return ret;
}

SeedValue
seed_os_getlogin (SeedContext ctx,
		  SeedObject function,
		  SeedObject this_object,
		  size_t argument_count,
		  const SeedValue arguments[], 
		  SeedException * exception)
{
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getlogin", "no arguments");
    }

  return seed_value_from_string (ctx, getlogin(), exception);
}

SeedValue
seed_os_getpgid (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  pid_t pid;
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.getpgid", "no arguments");
    }
  pid = (pid_t) seed_value_to_long (ctx, arguments[0], exception);
  
  return seed_value_from_long (ctx, (glong) getpgid(pid), exception);
}

SeedValue
seed_os_getpgrp (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getpgrp", "no arguments");
    }
  
  return seed_value_from_long (ctx, (glong) getpgrp(), exception);
}

seed_static_function os_funcs[] = {
  {"chdir", seed_os_chdir, 0},
  {"fchdir", seed_os_fchdir, 0},
  {"getcwd", seed_os_getcwd, 0},
  {"ctermid", seed_os_ctermid, 0},
  {"getegid", seed_os_getegid, 0},
  {"geteuid", seed_os_geteuid, 0},
  {"getgid", seed_os_getgid, 0},
  {"getuid", seed_os_getuid, 0},
  {"getlogin", seed_os_getlogin, 0},
  {"getpgid", seed_os_getpgid, 0},
  {"getpgrp", seed_os_getpgrp, 0}
};

SeedObject
seed_module_init(SeedEngine * eng)
{
  SeedClass os_namespace_class;
  seed_class_definition os_namespace_class_definition = seed_empty_class;
  
  os_namespace_class_definition.static_functions = os_funcs;
  os_namespace_class = seed_create_class (&os_namespace_class_definition);

  os_namespace = seed_make_object (eng->context, os_namespace_class, NULL);
}
