#include "seed-private.h"

JSClassRef importer_class;
JSObjectRef importer;


JSClassDefinition importer_class_def = {
  0,				/* Version, always 0 */
  0,
  "importer",		/* Class Name */
  NULL,				/* Parent Class */
  NULL,				/* Static Values */
  NULL,				/* Static Functions */
  NULL,                         /* Initialize */
  NULL,				/* Finalize */
  NULL,				/* Has Property */
  NULL,				/* Get Property */
  NULL,				/* Set Property */
  NULL,				/* Delete Property */
  NULL,				/* Get Property Names */
  NULL,				/* Call As Function */
  NULL,	/* Call As Constructor */
  NULL,				/* Has Instance */
  NULL				/* Convert To Type */
};

void seed_initialize_importer(JSContextRef ctx,
			      JSObjectRef global)
{
  importer_class = JSClassCreate (&importer_class_def);
  importer = JSObjectMake (ctx, importer_class, NULL);
  
  seed_object_set_property (ctx, global, "imports", importer);
}
