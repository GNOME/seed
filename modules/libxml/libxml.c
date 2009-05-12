#include <seed.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

SeedObject namespace_ref;
SeedEngine *eng;
SeedClass xml_doc_class;

seed_static_function doc_funcs[] = {
  {0, 0, 0}
};

seed_static_value doc_values[] = {
  {0, 0, 0, 0}
};

#define XML_DOC_PRIV(obj) ((xmlDocPtr)seed_object_get_private(obj))

static SeedValue 
seed_xml_parse_file (SeedContext ctx,
		     SeedObject function,
		     SeedObject this_object,
		     gsize argument_count,
		     const SeedValue arguments[],
		     SeedException * exception)
{
  SeedObject ret;
  xmlDocPtr doc;
  gchar *path;
  if (argument_count != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "parseFile expected 1 argument, got %d",
			   argument_count);
      return seed_make_null (ctx);
    }
  path = seed_value_to_string (ctx, arguments[0], exception);
  doc = xmlParseFile (path);
  if (!doc)
    {
      seed_make_exception (ctx, exception, "XMLError",
			   "Document not parsed successfully");
      g_free (path);
      return seed_make_null (ctx);
    }
  ret = seed_make_object (ctx, xml_doc_class, doc);

  g_free (path);
  return ret;
}

static void
seed_xml_doc_finalize (SeedObject object)
{
  xmlDocPtr ptr = XML_DOC_PRIV (object);
  xmlFreeDoc (ptr);
}

static void
seed_libxml_define_stuff ()
{
  seed_class_definition xml_doc_class_def = seed_empty_class;

  xml_doc_class_def.class_name="XMLDocument";
  xml_doc_class_def.static_functions = doc_funcs;
  xml_doc_class_def.static_values = doc_values;
  xml_doc_class = seed_create_class (&xml_doc_class_def);
  
  seed_create_function (eng->context, "parseFile", 
			(SeedFunctionCallback) seed_xml_parse_file,
			namespace_ref);
}

SeedObject
seed_module_init(SeedEngine *local_eng)
{
  eng = local_eng;
  namespace_ref = seed_make_object (eng->context, NULL, NULL);
  
  seed_libxml_define_stuff();

  return namespace_ref;
}
