#include "../../libseed/seed.h"
#include <ffi.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/mman.h>

SeedObject namespace_ref;
SeedEngine * eng;

static void
seed_handle_rl_closure(ffi_cif * cif,
					   void * result,
					   void ** args,
					   void * userdata)
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
	seed_context_unref((SeedContext)ctx);
}

// "Leaky" in that it exists for lifetime of program,
// kind of unavoidable though.
static ffi_closure * seed_make_rl_closure(SeedObject function)
{
	ffi_cif * cif;
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
				   const SeedValue arguments[],
				   SeedValue * exception)
{
	gchar * key;
	ffi_closure * c;

	if (argumentCount != 2)
	{
		gchar *mes =
			g_strdup_printf("Seed.readline_bind expected 2 arguments, "
							"got %d", argumentCount);
		seed_make_exception(ctx, exception, "ArgumentError", mes);
		g_free(mes);
		return seed_make_null(ctx);
	}

	key = seed_value_to_string(ctx, arguments[0], exception);
	c = seed_make_rl_closure((SeedObject)arguments[1]);
	
	rl_bind_key(*key, (Function*)c);
	
	g_free(key);
	
	return seed_make_null(ctx);
}

static SeedValue
seed_readline(SeedContext ctx,
			  SeedObject function,
			  SeedObject this_object,
			  size_t argumentCount,
			  const SeedValue arguments[],
			  SeedValue * exception)
{
	SeedValue valstr = 0;
	gchar *str = 0;
	gchar *buf;

	if (argumentCount != 1)
	{
		gchar *mes =
			g_strdup_printf("Seed.readline expected 1 argument, "
							"got %d", argumentCount);
		seed_make_exception(ctx, exception, "ArgumentError", mes);
		g_free(mes);
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

	g_free(buf);

	if (valstr == 0)
		valstr = seed_make_null(ctx);

	return valstr;
}

void seed_module_init(SeedEngine * local_eng)
{
	seed_class_definition readline_class_def = seed_empty_class;
	
	eng = local_eng;

	namespace_ref = seed_make_object(eng->context, 0, 0);

	seed_create_function(eng->context, 
						 "readline",
						 seed_readline,
						 namespace_ref);

	seed_create_function(eng->context, 
						 "bind",
						 seed_readline_bind,
						 namespace_ref);
	
	seed_object_set_property(eng->context, 
							 eng->global, "readline", namespace_ref);
}
