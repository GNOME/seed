#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/utsname.h>

#include <seed.h>

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
      EXPECTED_EXCEPTION("os.getpgid", "1 argument");
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

SeedValue
seed_os_getpid (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getpid", "no arguments");
    }
  
  return seed_value_from_long (ctx, (glong) getpid(), exception);
}

SeedValue
seed_os_getppid (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.getppid", "no arguments");
    }
  
  return seed_value_from_long (ctx, (glong) getppid(), exception);
}

SeedValue
seed_os_getenv (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  SeedValue ret;
  gchar *name, *value;
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.getenv", "1 arguments");
    }
  name = seed_value_to_string (ctx, arguments[0], exception);
  value = getenv (name);
  ret = seed_value_from_string (ctx, value, exception);
  g_free (name);
  
  return ret;
}

SeedValue
seed_os_putenv (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  gint ret;
  gchar *name, *value, *arg;
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("os.putenv", "2 arguments");
    }
  name = seed_value_to_string (ctx, arguments[0], exception);
  value = seed_value_to_string (ctx, arguments[1], exception);
  arg = g_strconcat (name, "=", value, NULL);
  
  ret = putenv (arg);
  
  g_free (name);
  g_free (value);
  
  return seed_value_from_int (ctx, ret, exception);
}


SeedValue
seed_os_setegid (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  gid_t arg;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.setegid", "1 argument");
    }
  arg = seed_value_to_long (ctx, arguments[0], exception);
  
  return seed_value_from_int (ctx, setegid(arg), exception);
}

SeedValue
seed_os_setgid (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  gid_t arg;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.setgid", "1 argument");
    }
  arg = seed_value_to_long (ctx, arguments[0], exception);
  
  return seed_value_from_int (ctx, setgid(arg), exception);
}

SeedValue
seed_os_seteuid (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  uid_t arg;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.seteuid", "1 argument");
    }
  arg = seed_value_to_long (ctx, arguments[0], exception);
  
  return seed_value_from_int (ctx, seteuid(arg), exception);
}

SeedValue
seed_os_setuid (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  uid_t arg;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.setuid", "1 argument");
    }
  arg = seed_value_to_long (ctx, arguments[0], exception);
  
  return seed_value_from_int (ctx, setuid(arg), exception);
}

SeedValue
seed_os_strerror (SeedContext ctx,
		  SeedObject function,
		  SeedObject this_object,
		  size_t argument_count,
		  const SeedValue arguments[], 
		  SeedException * exception)
{
  int arg;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.strerror", "1 argument");
    }
  arg = seed_value_to_int (ctx, arguments[0], exception);
  
  return seed_value_from_string (ctx, strerror(arg), exception);
}

SeedValue
seed_os_umask (SeedContext ctx,
	       SeedObject function,
	       SeedObject this_object,
	       size_t argument_count,
	       const SeedValue arguments[], 
	       SeedException * exception)
{
  mode_t arg;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.umask", "1 argument");
    }
  arg = seed_value_to_long (ctx, arguments[0], exception);
  
  return seed_value_from_long (ctx, umask(arg), exception);
}

SeedValue
seed_os_uname (SeedContext ctx,
	       SeedObject function,
	       SeedObject this_object,
	       size_t argument_count,
	       const SeedValue arguments[], 
	       SeedException * exception)
{
  SeedValue elements[5], ret;
  guint c;
  struct utsname name;
  
  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION("os.uname", "no arguments");
    }
  c = uname (&name);
  // TODO: Do something with c
  elements[0] = seed_value_from_string (ctx, name.sysname, exception);
  elements[1] = seed_value_from_string (ctx, name.nodename, exception);
  elements[2] = seed_value_from_string (ctx, name.release, exception);
  elements[3] = seed_value_from_string (ctx, name.version, exception);
  elements[4] = seed_value_from_string (ctx, name.machine, exception);
  ret = seed_make_array (ctx, elements, 5, exception);
  
  
  return ret;
}


SeedValue
seed_os_unsetenv (SeedContext ctx,
		  SeedObject function,
		  SeedObject this_object,
		  size_t argument_count,
		  const SeedValue arguments[], 
		  SeedException * exception)
{
  gint ret;
  gchar *arg;

  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.unsetenv", "1 argument");
    }
  
  arg = seed_value_to_string (ctx, arguments[0], exception);
  ret = unsetenv (arg);
  g_free (arg);

  return seed_value_from_int (ctx, ret, exception);
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
  {"getpgrp", seed_os_getpgrp, 0},
  {"getpid", seed_os_getpid, 0},
  {"getppid", seed_os_getuid, 0},
  {"getenv", seed_os_getenv, 0},
  {"putenv", seed_os_putenv, 0},
  {"setegid", seed_os_setegid, 0},
  {"setgid", seed_os_setegid, 0},
  {"seteuid", seed_os_setegid, 0},
  {"setuid", seed_os_setuid, 0},
  {"strerror", seed_os_strerror, 0},
  {"umask", seed_os_umask, 0},
  {"uname", seed_os_uname, 0},
  {"unsetenv", seed_os_unsetenv, 0}
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
