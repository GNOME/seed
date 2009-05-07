#include <stdio.h>
#include "../../libseed/seed.h"
#include <ffi.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/mman.h>

SeedObject namespace_ref;
SeedEngine *eng;
gboolean readline_has_initialized = FALSE;

static void
seed_handle_rl_closure(ffi_cif * cif, void *result, void **args, void *userdata)
{
  SeedContext ctx = seed_context_create(eng->group, NULL);
  SeedValue exception = 0;
  SeedObject function = (SeedObject) userdata;

  seed_object_call(ctx, function, 0, 0, 0, &exception);
  if (exception)
    {
      gchar *mes = seed_exception_to_string(ctx,
					    exception);
      g_warning("Exception in readline bind key closure. %s \n", mes, 0);
    }
  seed_context_unref((SeedContext) ctx);
}

// "Leaky" in that it exists for lifetime of program,
// kind of unavoidable though.
static ffi_closure *seed_make_rl_closure(SeedObject function)
{
  ffi_cif *cif;
  ffi_closure *closure;
  ffi_arg result;
  ffi_status status;

  cif = g_new0(ffi_cif, 1);
  closure = mmap(0, sizeof(ffi_closure), PROT_READ | PROT_WRITE |
		 PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
  ffi_prep_cif(cif, FFI_DEFAULT_ABI, 0, &ffi_type_sint, 0);
  ffi_prep_closure(closure, cif, seed_handle_rl_closure, function);

  return closure;
}

static SeedValue
seed_readline_bind(SeedContext ctx,
		   SeedObject function,
		   SeedObject this_object,
		   size_t argumentCount,
		   const SeedValue arguments[], SeedValue * exception)
{
  gchar *key;
  ffi_closure *c;

  if (argumentCount != 2)
    {
      seed_make_exception(ctx, exception, "ArgumentError", 
			  "Seed.readline_bind expected 2 arguments, "
			  "got %Zd", argumentCount);
      return seed_make_null(ctx);
    }

  key = seed_value_to_string(ctx, arguments[0], exception);
  c = seed_make_rl_closure((SeedObject) arguments[1]);

  rl_bind_key(*key, (Function *) c);

  g_free(key);

  return seed_make_null(ctx);
}

static SeedValue
seed_rl_done(SeedContext ctx,
	     SeedObject function,
	     SeedObject this_object,
	     size_t argumentCount,
	     const SeedValue arguments[], 
	     SeedValue * exception)
{
  rl_done = 1;
  return seed_make_null (ctx);
}

static SeedValue
seed_rl_buffer(SeedContext ctx,
	     SeedObject function,
	     SeedObject this_object,
	     size_t argumentCount,
	     const SeedValue arguments[], 
	     SeedValue * exception)
{
  return seed_value_from_string (ctx, rl_line_buffer, exception);
}

static SeedValue
seed_rl_insert(SeedContext ctx,
	       SeedObject function,
	       SeedObject this_object,
	       size_t argumentCount,
	       const SeedValue arguments[], 
	       SeedValue * exception)
{
  gchar *ins;
  gint ret;
  if (argumentCount != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError", 
			   "readline.insert expected 1 argument, got %d", 
			   argumentCount);
      return seed_make_null (ctx);
    }
  ins = seed_value_to_string (ctx, arguments[0], exception);
  ret = rl_insert_text (ins);
  g_free (ins);
  
  return seed_value_from_int (ctx, ret, exception);
}

static SeedValue
seed_readline(SeedContext ctx,
	      SeedObject function,
	      SeedObject this_object,
	      size_t argumentCount,
	      const SeedValue arguments[], SeedValue * exception)
{
  SeedValue valstr = 0;
  gchar *str = NULL;
  gchar *buf;
  const gchar *histfname = g_get_home_dir();
  gchar *path = g_build_filename(histfname, ".seed_history", NULL);

  if (!readline_has_initialized)
    {
      read_history(path);
      readline_has_initialized = TRUE;
    }

  if (argumentCount != 1)
    {
      seed_make_exception(ctx, exception, "ArgumentError", 
			  "Seed.readline expected 1 argument, "
			  "got %Zd", argumentCount);
      return seed_make_null(ctx);
    }

  buf = seed_value_to_string(ctx, arguments[0], exception);

  str = readline(buf);
  if (str && *str)
    {
      add_history(str);
      valstr = seed_value_from_string(ctx, str, exception);
      g_free(str);
    }

  write_history(path);
  history_truncate_file(path, 1000);

  g_free(buf);
  g_free(path);

  if (valstr == 0)
    valstr = seed_make_null(ctx);

  return valstr;
}

SeedObject
seed_module_init(SeedEngine * local_eng)
{
  seed_class_definition readline_class_def = seed_empty_class;

  eng = local_eng;

  namespace_ref = seed_make_object(eng->context, 0, 0);

  seed_create_function(eng->context,
		       "readline",
		       (SeedFunctionCallback) seed_readline,
		       (SeedObject) namespace_ref);

  seed_create_function(eng->context,
		       "bind",
		       (SeedFunctionCallback) seed_readline_bind,
		       (SeedObject) namespace_ref);

  seed_create_function(eng->context,
		       "done",
		       (SeedFunctionCallback) seed_rl_done,
		       (SeedObject) namespace_ref);

  seed_create_function(eng->context,
		       "buffer",
		       (SeedFunctionCallback) seed_rl_buffer,
		       (SeedObject) namespace_ref);

  seed_create_function(eng->context,
		       "insert",
		       (SeedFunctionCallback) seed_rl_insert,
		       (SeedObject) namespace_ref);

  return namespace_ref;
}

