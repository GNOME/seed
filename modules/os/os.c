#define _GNU_SOURCE

#include "../../config.h"

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/utsname.h>

#include <sys/types.h>

#include <fcntl.h>

#ifdef HAVE_PTY_H
#include <pty.h>
#endif

#include <seed.h>

SeedObject os_namespace;

#define EXPECTED_EXCEPTION(name, argnum) \
  seed_make_exception (ctx, exception, "ArgumentError", name " expected " argnum " got %Zd", argument_count); \
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

SeedValue
seed_os_open (SeedContext ctx,
	      SeedObject function,
	      SeedObject this_object,
	      size_t argument_count,
	      const SeedValue arguments[], 
	      SeedException * exception)
{
  gchar *path;
  gint flags, ret;

  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("os.open", "2 arguments");
    }
  
  path = seed_value_to_string (ctx, arguments[0], exception);
  flags = seed_value_to_int (ctx, arguments[1], exception);
  
  ret = open (path, flags);
  g_free (path);
  
  return seed_value_from_int (ctx, ret, exception);
}

SeedValue
seed_os_close (SeedContext ctx,
	       SeedObject function,
	       SeedObject this_object,
	       size_t argument_count,
	       const SeedValue arguments[], 
	       SeedException * exception)
{
  gint arg;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.close", "2 arguments");
    }
  
  arg = seed_value_to_int (ctx, arguments[0], exception);
  
  return seed_value_from_int (ctx, close (arg), exception);
}

SeedValue
seed_os_dup (SeedContext ctx,
	     SeedObject function,
	     SeedObject this_object,
	     size_t argument_count,
	     const SeedValue arguments[], 
	     SeedException * exception)
{
  gint arg;
  
  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION("os.dup", "1 argument");
    }
  
  arg = seed_value_to_int (ctx, arguments[0], exception);
  
  return seed_value_from_int (ctx, dup (arg), exception);
}
SeedValue
seed_os_dup2 (SeedContext ctx,
	      SeedObject function,
	      SeedObject this_object,
	      size_t argument_count,
	      const SeedValue arguments[], 
	      SeedException * exception)
{
  gint arg, arg2;
  
  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION("os.dup2", "2 arguments");
    }
  
  arg = seed_value_to_int (ctx, arguments[0], exception);
  arg2 = seed_value_to_int (ctx, arguments[0], exception);
  
  return seed_value_from_int (ctx, dup2 (arg, arg2), exception);
}

SeedValue
seed_os_fchmod (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  gint fd;
  mode_t mode;

  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("os.fchmod", "2 arguments");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  mode = seed_value_to_long (ctx, arguments[1], exception);
  
  return seed_value_from_int (ctx, fchmod (fd, mode), exception);
}

SeedValue
seed_os_fchown (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  gint fd;
  gid_t gid;
  uid_t uid;

  if (argument_count != 3)
    {
      EXPECTED_EXCEPTION ("os.fchown", "3 arguments");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  uid = seed_value_to_long (ctx, arguments[1], exception);
  gid = seed_value_to_long (ctx, arguments[2], exception);
  
  return seed_value_from_int (ctx, fchown (fd, uid, gid), exception);
}

SeedValue
seed_os_fdatasync (SeedContext ctx,
		   SeedObject function,
		   SeedObject this_object,
		   size_t argument_count,
		   const SeedValue arguments[], 
		   SeedException * exception)
{
  gint fd;

  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION ("os.fdatasync", "1 argument");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  
  return seed_value_from_int (ctx, fdatasync (fd), exception);
}

SeedValue
seed_os_fpathconf (SeedContext ctx,
		   SeedObject function,
		   SeedObject this_object,
		   size_t argument_count,
		   const SeedValue arguments[], 
		   SeedException * exception)
{
  gint fd, name;

  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("os.fpathconf", "2 arguments");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  name = seed_value_to_int (ctx, arguments[1], exception);
  
  return seed_value_from_long (ctx, fpathconf (fd, name), exception);
}

SeedValue
seed_os_fsync (SeedContext ctx,
	       SeedObject function,
	       SeedObject this_object,
	       size_t argument_count,
	       const SeedValue arguments[], 
	       SeedException * exception)
{
  gint fd, name;

  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION ("os.fsync", "1 argument");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  
  return seed_value_from_long (ctx, fsync (fd), exception);
}

SeedValue
seed_os_ftruncate (SeedContext ctx,
		   SeedObject function,
		   SeedObject this_object,
		   size_t argument_count,
		   const SeedValue arguments[], 
		   SeedException * exception)
{
  gint fd;
  off_t length;

  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("os.ftruncate", "2 arguments");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  length = seed_value_to_int (ctx, arguments[1], exception);
  
  return seed_value_from_long (ctx, ftruncate (fd, length), exception);
}

SeedValue
seed_os_isatty (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  gint fd;

  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION ("os.isatty", "1 argument");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  
  return seed_value_from_boolean (ctx, isatty (fd), exception);
}

SeedValue
seed_os_lseek (SeedContext ctx,
	       SeedObject function,
	       SeedObject this_object,
	       size_t argument_count,
	       const SeedValue arguments[], 
	       SeedException * exception)
{
  gint fd, whence;
  off_t offset;

  if (argument_count != 3)
    {
      EXPECTED_EXCEPTION ("os.lseek", "3 arguments");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  offset = seed_value_to_long (ctx, arguments[1], exception);
  whence = seed_value_to_int (ctx, arguments[2], exception);
  
  return seed_value_from_long (ctx, lseek (fd, offset, whence), exception);
}

SeedValue
seed_os_openpty (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  SeedValue fds[2], ret;
  gint master,slave;

  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION ("os.openpty", "no arguments");
    }
  openpty (&master, &slave, NULL, NULL, NULL);
  
  fds[0] = seed_value_from_int (ctx, master, exception);
  fds[1] = seed_value_from_int (ctx, slave, exception);
  
  ret = seed_make_array (ctx, fds, 2, exception);
  
  return ret;
}

SeedValue
seed_os_pipe (SeedContext ctx,
	      SeedObject function,
	      SeedObject this_object,
	      size_t argument_count,
	      const SeedValue arguments[], 
	      SeedException * exception)
{
  SeedValue fds[2], ret;
  gint fildes[2];

  if (argument_count != 0)
    {
      EXPECTED_EXCEPTION ("os.pipe", "no arguments");
    }
  if (pipe (fildes) < 0)
    {
      // TODO
    }
  
  fds[0] = seed_value_from_int (ctx, fildes[0], exception);
  fds[1] = seed_value_from_int (ctx, fildes[1], exception);
  
  ret = seed_make_array (ctx, fds, 2, exception);
  
  return ret;
}

SeedValue
seed_os_read (SeedContext ctx,
	      SeedObject function,
	      SeedObject this_object,
	      size_t argument_count,
	      const SeedValue arguments[], 
	      SeedException * exception)
{
  SeedValue ret;
  gint fd, n, nr;
  gchar *buf;

  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("os.read", "2 arguments");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  n = seed_value_to_int (ctx, arguments[1], exception);
  
  buf = g_alloca (n * sizeof (gchar));
  nr = read (fd, buf, n);
  buf[nr] = '\0';

  if (nr)
    ret = seed_value_from_string (ctx, buf, exception);
  else
    ret = seed_make_null (ctx);
  
  return ret;
}

SeedValue
seed_os_write (SeedContext ctx,
	       SeedObject function,
	       SeedObject this_object,
	       size_t argument_count,
	       const SeedValue arguments[], 
	       SeedException * exception)
{
  SeedValue ret;
  gint fd, nw;
  gchar *buf;

  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("os.write", "2 arguments");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  buf = seed_value_to_string (ctx, arguments[1], exception);
  
  nw = write (fd, buf, strlen (buf));

  return seed_value_from_int (ctx, nw, exception);
}

SeedValue
seed_os_ttyname (SeedContext ctx,
		 SeedObject function,
		 SeedObject this_object,
		 size_t argument_count,
		 const SeedValue arguments[], 
		 SeedException * exception)
{
  SeedValue ret;
  gint fd;

  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION ("os.ttyname", "1 argument");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  
  return seed_value_from_string (ctx, ttyname (fd), exception);
}

SeedValue
seed_os_tcgetpgrp (SeedContext ctx,
		   SeedObject function,
		   SeedObject this_object,
		   size_t argument_count,
		   const SeedValue arguments[], 
		   SeedException * exception)
{
  SeedValue ret;
  gint fd;

  if (argument_count != 1)
    {
      EXPECTED_EXCEPTION ("os.tcgetpgrp", "1 argument");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  
  return seed_value_from_long (ctx, tcgetpgrp (fd), exception);
}

SeedValue
seed_os_tcsetpgrp (SeedContext ctx,
		   SeedObject function,
		   SeedObject this_object,
		   size_t argument_count,
		   const SeedValue arguments[], 
		   SeedException * exception)
{
  SeedValue ret;
  gint fd;
  pid_t pgrp;

  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("os.tcsetpgrp", "2 arguments");
    }
  fd = seed_value_to_int (ctx, arguments[0], exception);
  pgrp = seed_value_to_int (ctx, arguments[1], exception);
  
  return seed_value_from_int (ctx, tcsetpgrp (fd, pgrp), exception);
}

SeedValue
seed_os_access (SeedContext ctx,
		SeedObject function,
		SeedObject this_object,
		size_t argument_count,
		const SeedValue arguments[], 
		SeedException * exception)
{
  int ret;
  gchar *path;
  int amd;

  if (argument_count != 2)
    {
      EXPECTED_EXCEPTION ("os.access", "2 arguments");
    }
  path = seed_value_to_string (ctx, arguments[0], exception);
  amd = seed_value_to_int (ctx, arguments[1], exception);
  
  ret = access (path, amd);
  
  if (ret == 0)
    return seed_value_from_boolean (ctx, TRUE, exception);
  else
    return seed_value_from_boolean (ctx, FALSE, exception);
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
  {"unsetenv", seed_os_unsetenv, 0},
  {"open", seed_os_open, 0},
  {"close", seed_os_close, 0},
  {"dup", seed_os_dup, 0},
  {"dup2", seed_os_dup2, 0},
  {"fchmod", seed_os_fchmod, 0},
  {"fchown", seed_os_fchown, 0},
  {"fdatasync", seed_os_fdatasync, 0},
  {"fpathconf", seed_os_fpathconf, 0},
  {"fsync", seed_os_fsync, 0},
  {"ftruncate", seed_os_ftruncate, 0},
  {"isatty", seed_os_isatty, 0},
  {"lseek", seed_os_lseek, 0},
  {"openpty", seed_os_openpty, 0},
  {"pipe", seed_os_pipe, 0},
  {"read", seed_os_read, 0},
  {"write", seed_os_write, 0},
  {"ttyname", seed_os_ttyname, 0},
  {"tcgetpgrp", seed_os_tcgetpgrp, 0},
  {"tcsetpgrp", seed_os_tcsetpgrp, 0},
  {"access", seed_os_access, 0}
};

#define OS_DEFINE_ENUM(name, value) \
  seed_object_set_property (eng->context, os_namespace, name,		\
			    seed_value_from_long (eng->context, value, NULL))
#define OS_DEFINE_QUICK_ENUM(value)					\
seed_object_set_property (eng->context, os_namespace, #value,		\
			  seed_value_from_long (eng->context, value, NULL)) \

SeedObject
seed_module_init(SeedEngine * eng)
{
  SeedClass os_namespace_class;
  seed_class_definition os_namespace_class_definition = seed_empty_class;
  
  os_namespace_class_definition.static_functions = os_funcs;
  os_namespace_class = seed_create_class (&os_namespace_class_definition);

  os_namespace = seed_make_object (eng->context, os_namespace_class, NULL);
  
  OS_DEFINE_QUICK_ENUM (O_RDONLY);
  OS_DEFINE_QUICK_ENUM (O_WRONLY);
  OS_DEFINE_QUICK_ENUM (O_RDWR);
  OS_DEFINE_QUICK_ENUM (O_APPEND);
  OS_DEFINE_QUICK_ENUM (O_CREAT);
  OS_DEFINE_QUICK_ENUM (O_EXCL);
  OS_DEFINE_QUICK_ENUM (O_TRUNC);

#if defined (O_DSYNC)
  OS_DEFINE_QUICK_ENUM (O_DSYNC);
#endif
#if defined (O_RSYNC)
  OS_DEFINE_QUICK_ENUM (O_RSYNC);
#endif

  OS_DEFINE_QUICK_ENUM (O_SYNC);
  OS_DEFINE_QUICK_ENUM (O_NDELAY);
  OS_DEFINE_QUICK_ENUM (O_NONBLOCK);
  OS_DEFINE_QUICK_ENUM (O_NOCTTY);
  //  OS_DEFINE_QUICK_ENUM (O_SHLOCK);
  //  OS_DEFINE_QUICK_ENUM (O_EXLOCK);

  OS_DEFINE_QUICK_ENUM (O_ASYNC);
#if defined (O_DIRECT)
  OS_DEFINE_QUICK_ENUM (O_DIRECT);
#endif
  OS_DEFINE_QUICK_ENUM (O_DIRECTORY);
  OS_DEFINE_QUICK_ENUM (O_NOFOLLOW);
#if defined (O_NOATIME)
  OS_DEFINE_QUICK_ENUM (O_NOATIME);
#endif

  OS_DEFINE_QUICK_ENUM (SEEK_SET);
  OS_DEFINE_QUICK_ENUM (SEEK_CUR);
  OS_DEFINE_QUICK_ENUM (SEEK_END);
  
  OS_DEFINE_QUICK_ENUM (F_OK);
  OS_DEFINE_QUICK_ENUM (R_OK);
  OS_DEFINE_QUICK_ENUM (W_OK);
  OS_DEFINE_QUICK_ENUM (X_OK);

  return os_namespace;
}
