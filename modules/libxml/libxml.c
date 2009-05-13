#include <seed.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

SeedObject namespace_ref;
SeedEngine *eng;

SeedClass xml_doc_class;
SeedClass xml_node_class;
SeedClass xml_attr_class;

SeedClass xml_xpath_class;
SeedClass xml_xpathobj_class;

#define XML_DOC_PRIV(obj) ((xmlDocPtr)seed_object_get_private(obj))
#define XML_NODE_PRIV(obj) ((xmlNodePtr)seed_object_get_private(obj))
#define XML_XPATH_PRIV(obj) ((xmlXPathContextPtr)seed_object_get_private (obj))
#define XML_XPATHOBJ_PRIV(obj) ((xmlXPathObjectPtr)seed_object_get_private (obj))

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

static SeedObject
seed_make_xml_attr (SeedContext ctx,
		    xmlAttrPtr attr)
{
 SeedObject ret;
  if (attr == NULL)
    return seed_make_null (ctx);
  if (attr->_private)
    return (SeedObject) attr->_private;
  ret = seed_make_object (ctx, xml_attr_class, attr);
  attr->_private = ret;
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
seed_xml_parse_string (SeedContext ctx,
		       SeedObject function,
		       SeedObject this_object,
		       gsize argument_count,
		       const SeedValue arguments[],
		       SeedException * exception)
{
  SeedObject ret;
  xmlDocPtr doc;
  gchar *string;
  if (argument_count != 1)
    {
      seed_make_exception (ctx, exception, "ArgumentError",
			   "parseString expected 1 argument, got %d",
			   argument_count);
      return seed_make_null (ctx);
    }
  string = seed_value_to_string (ctx, arguments[0], exception);
  doc = xmlParseMemory (string, strlen (string));
  if (!doc)
    {
      seed_make_exception (ctx, exception, "XMLError",
			   "Document not parsed successfully");
      g_free (string);
      return seed_make_null (ctx);
    }
  ret = seed_make_xml_doc (ctx, doc);

  g_free (string);
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

static SeedValue
seed_xml_node_get_properties (SeedContext ctx,
			      SeedObject object,
			      SeedString property_name,
			      SeedException *exception)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
  
  return seed_make_xml_attr (ctx, node->properties);
}

static void
seed_xml_node_init (SeedContext ctx,
		    SeedObject object)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
  if (node && node->doc->_private)
    seed_value_protect (ctx, node->doc->_private);
}

static void
seed_xml_node_finalize (SeedObject object)
{
  xmlNodePtr node = XML_NODE_PRIV (object);
  if (!node)
    return;
  node->_private = NULL;
  // This might be invalid.
  if (node->doc->_private)
    seed_value_unprotect (eng->context, node->doc->_private);
}

static SeedValue
seed_xml_xpath_eval (SeedContext ctx,
		     SeedObject function,
		     SeedObject this_object,
		     gsize argument_count,
		     const SeedValue arguments[],
		     SeedException * exception)
{
  xmlXPathObjectPtr xpath_obj;
  xmlXPathContextPtr xpath_ctx;
  gchar *xpath;
  
  xpath_ctx = XML_XPATH_PRIV (this_object);
  
  if (argument_count != 1)
    {
      seed_make_exception (ctx, exception, 
			   "ArgumentError",
			   "xpathEval expected 1 argument, got %d",
			   argument_count);
      return seed_make_null (ctx);
    }
  xpath = seed_value_to_string (ctx, arguments[0], exception);
  xpath_obj = xmlXPathEval (xpath, xpath_ctx);
  g_free (xpath);
  
  return seed_make_object (ctx, xml_xpathobj_class, xpath_obj);
}

static SeedValue
seed_xml_construct_xpath_context (SeedContext ctx,
				  SeedObject function,
				  SeedObject this_object,
				  gsize argument_count,
				  const SeedValue arguments[],
				  SeedException * exception)
{
  xmlXPathContextPtr xpath;
  xmlDocPtr doc;
  
  doc = XML_DOC_PRIV (this_object);
  xpath = xmlXPathNewContext (doc);
  
  seed_value_protect (ctx, this_object);
  
  return seed_make_object (ctx, xml_xpath_class, xpath);
}

static void
seed_xml_xpath_finalize (SeedObject object)
{
  xmlXPathContextPtr xpath = XML_XPATH_PRIV (object);
  // Maybe unsafe. Seems to work.
  seed_value_unprotect (eng->context, xpath->doc->_private);
  xmlXPathFreeContext (xpath);
}

static void
seed_xml_xpathobj_finalize (SeedObject object)
{
  xmlXPathObjectPtr xpath = XML_XPATHOBJ_PRIV (object);
  xmlXPathFreeObject (xpath);
}

static SeedValue
seed_xml_array_from_nodeset (SeedContext ctx,
			     xmlNodeSetPtr nodeset,
			     SeedException *exception)
{
  SeedValue *ary = g_alloca (nodeset->nodeNr * sizeof (SeedValue));
  int i;

  for (i = 0; i < nodeset->nodeNr; i++)
    {
      ary[i] = seed_make_xml_node (ctx, nodeset->nodeTab[i]);
    }
  return seed_make_array (ctx, ary, nodeset->nodeNr, exception);
}

static SeedValue
seed_xml_xpathobj_get_value (SeedContext ctx,
			     SeedObject object,
			     SeedString property_name,
			     SeedException *exception)
{
  xmlXPathObjectPtr xpath = XML_XPATHOBJ_PRIV (object);

  switch (xpath->type)
    {
/*    case XPATH_BOOLEAN:
      return seed_value_from_boolean (ctx, xpath->boolval, exception);
    case XPATH_NUMBER:
      return seed_value_from_double (ctx, xpath->floatval, exception);
    case XPATH_STRING:
    return seed_value_from_string (ctx, xpath->stringval, exception);*/
    case XPATH_NODESET:
      return seed_xml_array_from_nodeset (ctx, xpath->nodesetval, exception);
    default:
      return seed_make_null (ctx);
    }
}


seed_static_function doc_funcs[] = {
  {"xpathNewContext", seed_xml_construct_xpath_context, 0}
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
  {"properties", seed_xml_node_get_properties, 0, 0},
  {0, 0, 0, 0}
};

seed_static_function attr_funcs[] = {
  {0, 0, 0}
};

seed_static_value attr_values[] = {
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

seed_static_function xpath_funcs[] = {
  {"xpathEval", seed_xml_xpath_eval, 0},
  {0, 0, 0}
};

seed_static_value xpathobj_values[] = {
  {"value", seed_xml_xpathobj_get_value, 0, 0},
  {0, 0, 0, 0}
};

static void
seed_libxml_define_stuff ()
{
  SeedObject xpath_constructor;
  SeedObject node_proto;

  seed_class_definition xml_doc_class_def = seed_empty_class;
  seed_class_definition xml_node_class_def = seed_empty_class;
  seed_class_definition xml_attr_class_def = seed_empty_class;
  seed_class_definition xml_xpath_class_def = seed_empty_class;
  seed_class_definition xml_xpathobj_class_def = seed_empty_class;

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

  xml_attr_class_def.class_name="XMLAttribute";
  xml_attr_class_def.static_functions = attr_funcs;
  xml_attr_class_def.static_values = attr_values;
  xml_attr_class_def.finalize = seed_xml_node_finalize;
  xml_attr_class_def.initialize = seed_xml_node_init;
  xml_attr_class = seed_create_class (&xml_attr_class_def);
  
  xml_xpath_class_def.class_name = "XMLXPathContext";
  xml_xpath_class_def.finalize = seed_xml_xpath_finalize;
  xml_xpath_class_def.static_functions = xpath_funcs;
  xml_xpath_class = seed_create_class (&xml_xpath_class_def);
  
  xml_xpathobj_class_def.class_name = "XMLXPathObj";
  xml_xpathobj_class_def.finalize = seed_xml_xpathobj_finalize;
  xml_xpathobj_class_def.static_values = xpathobj_values;
  xml_xpathobj_class = seed_create_class (&xml_xpathobj_class_def);
  
  seed_create_function (eng->context, "parseFile", 
			(SeedFunctionCallback) seed_xml_parse_file,
			namespace_ref);
  seed_create_function (eng->context, "parseString", 
			(SeedFunctionCallback) seed_xml_parse_string,
			namespace_ref);

  node_proto = seed_object_get_prototype (eng->context,
					  seed_make_object (eng->context, 
							    xml_node_class, 
							    NULL));
  seed_make_object (eng->context, xml_node_class, NULL);
  seed_object_set_property (eng->context, namespace_ref, "_nodeProto", node_proto);
  seed_simple_evaluate (eng->context, "imports.extensions.xml", NULL);
}

SeedObject
seed_module_init(SeedEngine *local_eng)
{
  eng = local_eng;
  namespace_ref = seed_make_object (eng->context, NULL, NULL);
  seed_value_protect (eng->context, namespace_ref);
  
  seed_libxml_define_stuff();

  return namespace_ref;
}
