#include <seed.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

SeedObject namespace_ref;
SeedEngine *eng;
SeedClass xml_doc_class;
SeedClass xml_node_class;

#define XML_DOC_PRIV(obj) ((xmlDocPtr)seed_object_get_private(obj))
#define XML_NODE_PRIV(obj) ((xmlNodePtr)seed_object_get_private(obj))

static SeedObject
seed_make_xml_doc (SeedContext ctx, 
		    xmlDocPtr doc)
{
  SeedObject ret;
  if (doc->_private)
    return (SeedObject) doc->_private;
  ret = seed_make_object (ctx, xml_doc_class, doc);
  doc->_private = ret;
  return ret;
}

static SeedObject
seed_make_xml_node (SeedContext ctx, 
		xmlNodePtr node)
{
  SeedObject ret;
  if (node == NULL)
    return seed_make_null (ctx);
  if (node->_private)
    return (SeedObject) node->_private;
  ret = seed_make_object (ctx, xml_node_class, node);
  node->_private = ret;
  return ret;
}

static gchar *
seed_xml_element_type_to_string (xmlElementType type)
{
  if (type == XML_ELEMENT_NODE)
    return "element";
  else if (type == XML_ATTRIBUTE_NODE)
    return "attribute";
  else if (type == XML_TEXT_NODE)
    return "text";
  else
    return "Implement more types! racarr is lazy.";	   
}

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
  ret = seed_make_xml_doc (ctx, doc);

  g_free (path);
  return ret;
}

static SeedValue
seed_xml_doc_get_root (SeedContext ctx,
		       SeedObject object,
		       SeedString property_name,
		       SeedException *exception)
{
  xmlDocPtr doc = XML_DOC_PRIV (object);
  return seed_make_xml_node (ctx, xmlDocGetRootElement (doc));
}

static void
seed_xml_doc_finalize (SeedObject object)
{
  xmlDocPtr ptr = XML_DOC_PRIV (object);
  xmlFreeDoc (ptr);
}

static SeedValue
seed_xml_node_get_name (SeedContext ctx,
			SeedObject object,
			SeedString property_name,
			SeedException *exception)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
  return seed_value_from_string (ctx, node->name, exception);
}

static SeedValue
seed_xml_node_get_children (SeedContext ctx,
			   SeedObject object,
			   SeedString property_name,
			   SeedException *exception)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
 
  return seed_make_xml_node (ctx, node->children);

}

static SeedValue
seed_xml_node_get_parent (SeedContext ctx,
			  SeedObject object,
			  SeedString property_name,
			  SeedException *exception)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
 
  return seed_make_xml_node (ctx, node->parent);
}

static SeedValue
seed_xml_node_get_next (SeedContext ctx,
			SeedObject object,
			SeedString property_name,
			SeedException *exception)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
 
  return seed_make_xml_node (ctx, node->next);
}

static SeedValue
seed_xml_node_get_prev (SeedContext ctx,
			SeedObject object,
			SeedString property_name,
			SeedException *exception)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
 
  return seed_make_xml_node (ctx, node->prev);
}

static SeedValue
seed_xml_node_get_last (SeedContext ctx,
			SeedObject object,
			SeedString property_name,
			SeedException *exception)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
 
  return seed_make_xml_node (ctx, node->last);
}

static SeedValue
seed_xml_node_get_doc (SeedContext ctx,
		       SeedObject object,
		       SeedString property_name,
		       SeedException *exception)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
  
  return seed_make_xml_doc (ctx, node->doc);
}

static SeedValue
seed_xml_node_get_content (SeedContext ctx,
			SeedObject object,
			SeedString property_name,
			SeedException *exception)
{
  SeedValue ret;
  gchar *content;
  xmlNodePtr node = XML_NODE_PRIV (object);
  
  content = xmlNodeGetContent (node);
  ret = seed_value_from_string (ctx, content, exception);
  g_free (content);
 
  return ret;
}

static SeedValue
seed_xml_node_get_type (SeedContext ctx,
			SeedObject object,
			SeedString property_name,
			SeedException *exception)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
  
  return seed_value_from_string (ctx, 
				 seed_xml_element_type_to_string 
				 (node->type), exception);
}
static void
seed_xml_node_init (SeedContext ctx,
		    SeedObject object)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
  if (node->doc->_private)
    seed_value_protect (ctx, node->doc->_private);
}

static void
seed_xml_node_finalize (SeedObject object)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
  node->_private = NULL;
  // This might be invalid.
  if (node->doc->_private)
    seed_value_unprotect (eng->context, node->doc->_private);
}

seed_static_function doc_funcs[] = {
  {0, 0, 0}
};

seed_static_value doc_values[] = {
  {"root", seed_xml_doc_get_root, 0, 0},
  {"name", seed_xml_node_get_name, 0, 0},
  {"children", seed_xml_node_get_children, 0, 0},
  {"parent", seed_xml_node_get_parent, 0, 0},
  {"next", seed_xml_node_get_next, 0, 0},
  {"prev", seed_xml_node_get_prev, 0, 0},
  {"last", seed_xml_node_get_last, 0, 0},
  {"doc", seed_xml_node_get_doc, 0, 0},
  {"type", seed_xml_node_get_type, 0, 0},
  {0, 0, 0, 0}
};

seed_static_function node_funcs[] = {
  {0, 0, 0}
};

seed_static_value node_values[] = {
  {"name", seed_xml_node_get_name, 0, 0},
  {"children", seed_xml_node_get_children, 0, 0},
  {"parent", seed_xml_node_get_parent, 0, 0},
  {"next", seed_xml_node_get_next, 0, 0},
  {"prev", seed_xml_node_get_prev, 0, 0},
  {"content", seed_xml_node_get_content, 0, 0},
  {"last", seed_xml_node_get_last, 0, 0},
  {"doc", seed_xml_node_get_doc, 0, 0},
  {"type", seed_xml_node_get_type, 0, 0},
  {0, 0, 0, 0}
};

static void
seed_libxml_define_stuff ()
{
  seed_class_definition xml_doc_class_def = seed_empty_class;
  seed_class_definition xml_node_class_def = seed_empty_class;

  xml_doc_class_def.class_name="XMLDocument";
  xml_doc_class_def.static_functions = doc_funcs;
  xml_doc_class_def.static_values = doc_values;
  xml_doc_class_def.finalize = seed_xml_doc_finalize;
  xml_doc_class = seed_create_class (&xml_doc_class_def);

  xml_node_class_def.class_name="XMLNode";
  xml_node_class_def.static_functions = node_funcs;
  xml_node_class_def.static_values = node_values;
  xml_node_class_def.finalize = seed_xml_node_finalize;
  xml_node_class_def.initialize = seed_xml_node_init;
  xml_node_class = seed_create_class (&xml_node_class_def);
  
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
