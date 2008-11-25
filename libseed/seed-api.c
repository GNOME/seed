#include "seed-private.h"

JSObjectRef seed_make_object(JSClassRef class, gpointer private)
{
	return JSObjectMake(eng->context, class, private);
}
