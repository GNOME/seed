/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-types.c
 * Copyright (C) Robert Carr 2008 <carrr@rpi.edu>
 *
 * libseed is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libseed is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "seed-private.h"

JSClassRef gobject_class;
JSClassRef gobject_method_class;
JSClassRef gobject_constructor_class;
SeedEngine * eng;

static void seed_protect_object(SeedValue val)
{
		JSValueProtect(eng->context, val);
}

static void seed_unprotect_object(SeedValue val)
{
		JSValueUnprotect(eng->context, val);
}

static gboolean seed_value_is_gobject(SeedValue value)
{
		gboolean ret;
	
		if (!JSValueIsObject(eng->context, value) || 
			JSValueIsNull(eng->context, value))
				return FALSE;

	
		return JSValueIsObjectOfClass(eng->context, value, gobject_class);
}

static GObject * seed_value_to_gobject(SeedValue value)
{
		GObject * gobject;
	
		if (!JSValueIsObject(eng->context, value) || 
			JSValueIsNull(eng->context, value))
				return NULL;
	
		if(JSValueIsObjectOfClass(eng->context, value, gobject_class))
				gobject = (GObject*)JSObjectGetPrivate((JSObjectRef)value);
		else
				gobject = NULL;

		return gobject;
}

static SeedValue seed_wrap_object(GObject * object)
{
		SeedValue user_data;
		SeedValue js_ref;
		JSClassRef class;
		GType type;
		JSValueRef prototype;
	

		type = G_OBJECT_TYPE(object);

		user_data = (SeedValue)g_object_get_data(object,
												 "js-ref");
	
		if (user_data)
				return user_data;

		class = seed_gobject_get_class_for_gtype(type);


		while (!class && (type=g_type_parent(type)))
		{
				class = seed_gobject_get_class_for_gtype(type);
		}


		prototype = seed_gobject_get_prototype_for_gtype(type);
		js_ref = JSObjectMake(eng->context, class, object);
		JSObjectSetPrototype(eng->context, (JSObjectRef)js_ref, prototype);

		// Is this going to work? g_free seems wrong, don't we need some
		// variety of finalize. This is a pointer declared on the stack anyway.
		// What is happening. You had &js_ref, after this ramblign comment
		// I am changing it to js_ref, and protecting/unprotecting it.
		object = g_object_ref(object);
		//g_object_set_data_full(object, "js-ref", (gpointer)js_ref, 
		//		       (GDestroyNotify) seed_unprotect_object);
		/* I think we avoid the need for this
		   g_object_add_toggle_ref(object,
		   object_toggle_notify_cb,
		   0); */
	
		return js_ref;
	
	
	
	
}

/* Should update to try and use glib type conversion */
gchar * seed_value_to_locale_string (SeedValue val)
{
		JSStringRef jsstr;
		gint length;
		gchar * ret;
	
		g_return_if_fail(val);

		if (!JSValueIsString(eng->context, val))
				return 0;

	
		jsstr = JSValueToStringCopy(eng->context, val, NULL);
	
		length = JSStringGetMaximumUTF8CStringSize(jsstr);
		ret = malloc(length * sizeof(gchar));

		JSStringGetUTF8CString(jsstr, ret, length);

		JSStringRelease(jsstr);
	
		return ret;
}

GType seed_gi_type_to_gtype(GITypeInfo *type_info, GITypeTag tag)
{
		switch(tag)
		{
		case GI_TYPE_TAG_VOID:
				return G_TYPE_NONE;
		case GI_TYPE_TAG_BOOLEAN:
				return G_TYPE_BOOLEAN;
		case GI_TYPE_TAG_INT8:
				return G_TYPE_CHAR;
		case GI_TYPE_TAG_UINT8:
				return G_TYPE_UCHAR;
		case GI_TYPE_TAG_INT16:
				return G_TYPE_INT;
		case GI_TYPE_TAG_UINT16:
				return G_TYPE_UINT;
		case GI_TYPE_TAG_INT32:
				return G_TYPE_INT;
		case GI_TYPE_TAG_UINT32:
				return G_TYPE_UINT;
		case GI_TYPE_TAG_INT64:
				return G_TYPE_INT64;
		case GI_TYPE_TAG_UINT64:
				return G_TYPE_UINT64;
		case GI_TYPE_TAG_INT:
				return G_TYPE_INT;
		case GI_TYPE_TAG_UINT:
				return G_TYPE_UINT;
		case GI_TYPE_TAG_LONG:
				return G_TYPE_LONG;
		case GI_TYPE_TAG_ULONG:
				return G_TYPE_ULONG;
		case GI_TYPE_TAG_SSIZE:
				return G_TYPE_INT;
		case GI_TYPE_TAG_SIZE:
				return G_TYPE_INT;
		case GI_TYPE_TAG_FLOAT:
				return G_TYPE_FLOAT;
		case GI_TYPE_TAG_DOUBLE:
				return G_TYPE_DOUBLE;
		case GI_TYPE_TAG_UTF8:
		case GI_TYPE_TAG_FILENAME:
				return G_TYPE_STRING;
		case GI_TYPE_TAG_ARRAY:
		case GI_TYPE_TAG_GLIST:
		case GI_TYPE_TAG_GSLIST:
		case GI_TYPE_TAG_GHASH:
		case GI_TYPE_TAG_ERROR:
				return G_TYPE_INVALID;
		case GI_TYPE_TAG_INTERFACE:
		{
				GIBaseInfo *interface;
				GIInfoType interface_type;

				interface = g_type_info_get_interface(type_info);
				interface_type = g_base_info_get_type(interface);
				if (interface_type == GI_INFO_TYPE_OBJECT)
						return G_TYPE_OBJECT;
				else if (interface_type == GI_INFO_TYPE_ENUM)
						return G_TYPE_LONG;
		}
		}
		return 0;
}


gboolean seed_gi_make_argument(SeedValue value,
							   GITypeInfo *type_info,
							   GArgument * arg)
{
		GValue gval = {0};
		GITypeTag gi_tag = g_type_info_get_tag(type_info);
		GType gtag = seed_gi_type_to_gtype(type_info,
										   gi_tag);
	
		if (!value || JSValueIsNull(eng->context, value))
		{
				arg->v_pointer = 0;
				return 1;
		}

		seed_gvalue_from_seed_value(value,
									gtag,
									&gval);
	
		switch(gi_tag)
		{
		case GI_TYPE_TAG_VOID:
				break;
		case GI_TYPE_TAG_BOOLEAN:
				arg->v_boolean = g_value_get_boolean(&gval);
				break;
		case GI_TYPE_TAG_INT8:
				arg->v_int8 = g_value_get_char(&gval);
				break;
		case GI_TYPE_TAG_UINT8:
				arg->v_uint8 = g_value_get_uchar(&gval);
				break;
		case GI_TYPE_TAG_INT16:
				arg->v_int16 = g_value_get_int(&gval);
				break;
		case GI_TYPE_TAG_UINT16:
				arg->v_uint16 = g_value_get_uint(&gval);
				break;
		case GI_TYPE_TAG_INT32:
				arg->v_int32 = g_value_get_int(&gval);
				break;
		case GI_TYPE_TAG_UINT32:
				arg->v_uint32 = g_value_get_uint(&gval);
				break;
		case GI_TYPE_TAG_LONG:
		case GI_TYPE_TAG_INT64:
				arg->v_int64 = (gint64)g_value_get_long(&gval);
				break;
		case GI_TYPE_TAG_ULONG:
		case GI_TYPE_TAG_UINT64:
				arg->v_uint64 = (guint64)g_value_get_ulong(&gval);
				break;
		case GI_TYPE_TAG_INT:
				arg->v_int = g_value_get_int(&gval);
				break;
		case GI_TYPE_TAG_UINT:
				arg->v_uint = g_value_get_uint(&gval);
				break;
		case GI_TYPE_TAG_SSIZE:
				arg->v_int = g_value_get_int(&gval);
				break;
		case GI_TYPE_TAG_SIZE:
				arg->v_int = g_value_get_int(&gval);
				break;
		case GI_TYPE_TAG_FLOAT:
				arg->v_float = g_value_get_float(&gval);
				break;
		case GI_TYPE_TAG_DOUBLE:
				arg->v_double = g_value_get_double(&gval);
				break;
		case GI_TYPE_TAG_UTF8:
				arg->v_string = g_strdup(g_value_get_string(&gval));
				break;
		case GI_TYPE_TAG_INTERFACE:
		{
				GIBaseInfo *interface;
				GIInfoType interface_type;
				GType required_gtype;
				GObject * gobject;

				interface = g_type_info_get_interface(type_info);
				interface_type = g_base_info_get_type(interface);
		
				arg->v_pointer = NULL;
		
				if (interface_type == GI_INFO_TYPE_OBJECT)
				{
						if (!G_VALUE_HOLDS_OBJECT(&gval))
						{
								return FALSE;
						}
						gobject = g_value_get_object(&gval);
						required_gtype = 
								g_registered_type_info_get_g_type(
										(GIRegisteredTypeInfo *) interface);
						if (!g_type_is_a(G_OBJECT_TYPE (gobject), 
										 required_gtype))
						{
								return FALSE;
						}
			
						arg->v_pointer = g_object_ref(gobject);
						break;
				}
				else if (interface_type == GI_INFO_TYPE_ENUM)
				{
						arg->v_long = JSValueToNumber(eng->context, 
													  value, NULL);
						break;
				}
		}
	  
		default:
				return FALSE;

	      
		
		
		}
		return TRUE;
		
}

JSValueRef seed_gi_argument_make_js(GArgument * arg, GITypeInfo *type_info)
{
		GValue gval = {0};
		GITypeTag gi_tag = g_type_info_get_tag(type_info);
		GType gtag = seed_gi_type_to_gtype(type_info, gi_tag);
		SeedValue ret;
	
		g_value_init(&gval, gtag);
		switch (gi_tag)
		{
		case GI_TYPE_TAG_VOID:
				return 0;
		case GI_TYPE_TAG_BOOLEAN:
				g_value_set_boolean(&gval,arg->v_boolean );
				break;
		case GI_TYPE_TAG_INT8:
				g_value_set_char(&gval,arg->v_int8 );
				break;
		case GI_TYPE_TAG_UINT8:
				g_value_set_uchar(&gval,arg->v_uint8 );
				break;
		case GI_TYPE_TAG_INT16:
				g_value_set_int(&gval,arg->v_int16 );
				break;
		case GI_TYPE_TAG_UINT16:
				g_value_set_uint(&gval,arg->v_uint16 );
				break;
		case GI_TYPE_TAG_INT32:
				g_value_set_int(&gval,arg->v_int32 );
				break;
		case GI_TYPE_TAG_UINT32:
				g_value_set_uint(&gval,arg->v_uint32 );
				break;
		case GI_TYPE_TAG_LONG:
		case GI_TYPE_TAG_INT64:
				g_value_set_long(&gval,arg->v_int64);
				break;
		case GI_TYPE_TAG_ULONG:
		case GI_TYPE_TAG_UINT64:
				g_value_set_ulong(&gval,arg->v_uint64);
				break;
		case GI_TYPE_TAG_INT:
				g_value_set_int(&gval,arg->v_int );
				break;
		case GI_TYPE_TAG_UINT:
				g_value_set_uint(&gval,arg->v_uint );
				break;
		case GI_TYPE_TAG_SSIZE:
				g_value_set_int(&gval,arg->v_int );
				break;
		case GI_TYPE_TAG_SIZE:
				g_value_set_int(&gval,arg->v_int );
				break;
		case GI_TYPE_TAG_FLOAT:
				g_value_set_float(&gval,arg->v_float );
				break;
		case GI_TYPE_TAG_DOUBLE:
				g_value_set_double(&gval,arg->v_double );
				break;
		case GI_TYPE_TAG_UTF8:
				g_value_set_string(&gval,arg->v_string);
				break;
		case GI_TYPE_TAG_INTERFACE:
		{
				GIBaseInfo *interface;
				GIInfoType interface_type;
				GType required_gtype;
				GObject * gobject;

				interface = g_type_info_get_interface(type_info);
				interface_type = g_base_info_get_type(interface);
		
		
				if (interface_type == GI_INFO_TYPE_OBJECT)
				{
						g_value_set_object(&gval, arg->v_pointer);
						break;
				}
				else if (interface_type == GI_INFO_TYPE_ENUM)
				{
						g_value_set_long(&gval, arg->v_double);
						return 0;
				}
				else
				{
						printf("OH NO! \n");
				}
		}
	  
		default:
				return FALSE;

		}
		ret = seed_value_from_gvalue(&gval);
		g_value_unset(&gval);
		return ret;
}

gboolean seed_gi_supports_type(GITypeInfo * type_info)
{
		GITypeTag type_tag;
	
		type_tag = g_type_info_get_tag(type_info);
	
		switch (type_tag)
		{
		case GI_TYPE_TAG_VOID:
		case GI_TYPE_TAG_BOOLEAN:
		case GI_TYPE_TAG_INT8:
		case GI_TYPE_TAG_UINT8:
		case GI_TYPE_TAG_INT16:
		case GI_TYPE_TAG_UINT16:
		case GI_TYPE_TAG_INT32:
		case GI_TYPE_TAG_UINT32:
		case GI_TYPE_TAG_INT64:
		case GI_TYPE_TAG_UINT64:
		case GI_TYPE_TAG_INT:
		case GI_TYPE_TAG_UINT:
		case GI_TYPE_TAG_LONG:
		case GI_TYPE_TAG_ULONG:
		case GI_TYPE_TAG_SSIZE:
		case GI_TYPE_TAG_SIZE:
		case GI_TYPE_TAG_FLOAT:
		case GI_TYPE_TAG_DOUBLE:
		case GI_TYPE_TAG_UTF8:
				return TRUE;
	       
		case GI_TYPE_TAG_FILENAME:
		case GI_TYPE_TAG_ARRAY:
		case GI_TYPE_TAG_GLIST:
		case GI_TYPE_TAG_GSLIST:
		case GI_TYPE_TAG_GHASH:
		case GI_TYPE_TAG_ERROR:
				return FALSE;
		
		case GI_TYPE_TAG_INTERFACE:
		{
				return TRUE;
		}
	
	
		}
}

SeedValue seed_value_from_gvalue(GValue * gval)
{
		if(!G_IS_VALUE(gval))
		{
				return false;
		}
	
		SeedValue val;
	
		switch(G_VALUE_TYPE(gval))
		{
		case G_TYPE_BOOLEAN:
		{
				return JSValueMakeBoolean(eng->context, 
										  g_value_get_boolean(gval));
		}
		case G_TYPE_CHAR:
		{
				return JSValueMakeNumber(eng->context, 
										 g_value_get_char(gval));
		}
		case G_TYPE_UCHAR:
		{
				return JSValueMakeNumber(eng->context, 
										 g_value_get_uchar(gval));
		}
		case G_TYPE_INT:
		{
				return JSValueMakeNumber(eng->context,
										 g_value_get_int(gval));
		}
		case G_TYPE_UINT:
		{
				return JSValueMakeNumber(eng->context, 
										 g_value_get_uint(gval));
		}
		case G_TYPE_LONG:
		{
				return JSValueMakeNumber(eng->context, 
										 g_value_get_long(gval));
		}
		case G_TYPE_ULONG:
		{
				return JSValueMakeNumber(eng->context, 
										 g_value_get_ulong(gval));
		}
		case G_TYPE_INT64:
		{
				return JSValueMakeNumber(eng->context, 
										 g_value_get_int64(gval));
		}
		case G_TYPE_UINT64:
		{
				return JSValueMakeNumber(eng->context, 
										 g_value_get_uint64(gval));
		}
		case G_TYPE_FLOAT:
		{
				return JSValueMakeNumber(eng->context, g_value_get_float(gval));
		}
		case G_TYPE_DOUBLE:
		{
				return JSValueMakeNumber(eng->context,
										 g_value_get_double(gval));
		}
		case G_TYPE_STRING:
		{
				JSValueRef str = JSValueMakeString(eng->context, 
												   JSStringCreateWithUTF8CString(
														   g_value_get_string(gval)));
				return JSValueToObject(eng->context,str,0);
		}		
		}
	
		if(g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_ENUM))
				return JSValueMakeNumber(eng->context, 
										 (double)gval->data[0].v_long);
		else if(g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_ENUM))
				return JSValueMakeNumber(eng->context, 
										 (double)gval->data[0].v_ulong);
		else if(g_type_is_a(G_VALUE_TYPE(gval), G_TYPE_OBJECT))
		{
				GObject * gobject;
		
		
				gobject = (GObject*)g_value_get_object(gval);
				if(gobject == NULL)
						val = JSValueMakeNull(eng->context);
				else
						val = seed_wrap_object(gobject);


				g_object_unref(gobject);
		
				return val;
		}
		else
		{
				GIBaseInfo * info;
				GIInfoType type;
				
				info = g_irepository_find_by_gtype(0, G_VALUE_TYPE(gval));
				type = g_base_info_get_type(info);
				
				if (type == GI_INFO_TYPE_UNION)
				{
						return seed_make_union(g_value_peek_pointer(gval),
											   info);
				}
				else if (type == GI_INFO_TYPE_STRUCT)
				{
						return seed_make_struct(g_value_peek_pointer(gval),
												info);
				}
				
		}
	
		return NULL;
}

gboolean seed_gvalue_from_seed_value(SeedValue val, 
									 GType type,
									 GValue * ret)
	
{
		gint32 cv;
	
		switch(type)
		{
		case G_TYPE_BOOLEAN:
		{
				/* This is fail. Need to call
				   seed_gvalue_from_seed_value with no type, and then
				   try gobject cast. */
				if(!JSValueIsBoolean(eng->context, val))
						goto bad_type;
		
				g_value_init(ret, G_TYPE_BOOLEAN);
				g_value_set_boolean(ret, JSValueToBoolean(eng->context, val));
				return TRUE;
		}
		case G_TYPE_INT:
		case G_TYPE_UINT:
		{
				if(!JSValueIsNumber(eng->context, val))
						goto bad_type;
			
				g_value_init(ret, type);
				if (type == G_TYPE_INT)
						g_value_set_int(ret, 
										JSValueToNumber(eng->context, val, NULL));
				else
						g_value_set_uint(ret,
										 JSValueToNumber(eng->context, val, NULL));
				return TRUE;
		}
		case G_TYPE_CHAR:
		{
				if(!JSValueIsNumber(eng->context, val))
						goto bad_type;
		
				cv = JSValueToNumber(eng->context, val, NULL);
				if(cv < G_MININT8 || cv > G_MAXINT8)
						goto bad_type;
			
				g_value_init(ret, G_TYPE_INT);
				g_value_set_char(ret, cv);
				return TRUE;
		}
		case G_TYPE_UCHAR:
		{
				if(!JSValueIsNumber(eng->context, val))
						goto bad_type;
		
				cv = JSValueToNumber(eng->context, val, NULL);
				if(cv < 0 || cv > G_MAXUINT8)
						goto bad_type;
			
				g_value_init(ret, G_TYPE_INT);
				g_value_set_uchar(ret, cv);
				return TRUE;
		}
		case G_TYPE_LONG:
		case G_TYPE_ULONG:
		case G_TYPE_INT64:
		case G_TYPE_UINT64:
		case G_TYPE_FLOAT:
		case G_TYPE_DOUBLE:
		{
				if(!JSValueIsNumber(eng->context, val))
						goto bad_type;
			
				g_value_init(ret, type);
				switch(type)
				{
				case G_TYPE_LONG:
						g_value_set_long(ret, 
										 JSValueToNumber(eng->context,
														 val, NULL));
						break;
				case G_TYPE_ULONG:
						g_value_set_ulong(ret,
										  JSValueToNumber(eng->context, 
														  val, NULL));
						break;
				case G_TYPE_INT64:
						g_value_set_int64(ret, 
										  JSValueToNumber(eng->context, 
														  val, NULL));
						break;
				case G_TYPE_UINT64:
						g_value_set_uint64(ret, 
										   JSValueToNumber(eng->context, 
														   val, NULL));
						break;
				case G_TYPE_FLOAT:
						g_value_set_float(ret, 
										  JSValueToNumber(eng->context, 
														  val, NULL));
						break;
				case G_TYPE_DOUBLE:
						g_value_set_double(ret,
										   JSValueToNumber(eng->context, 
														   val, NULL));
						break;
				}
				return TRUE;
		}
		case G_TYPE_STRING:
		{
				gchar * cval = seed_value_to_string(val);
							
				g_value_init(ret, G_TYPE_STRING);
				g_value_take_string(ret, cval);
		
				return TRUE;
		}
		default:
		{
				switch(JSValueGetType(eng->context, val))
				{
				case kJSTypeBoolean:
				{
						g_value_init(ret, G_TYPE_BOOLEAN);
						g_value_set_boolean(ret, 
											JSValueToBoolean(eng->context, 
															 val));
						return TRUE;
				}
				case kJSTypeNumber:
				{
						g_value_init(ret, G_TYPE_DOUBLE);
						g_value_set_double(ret,
										   JSValueToNumber(eng->context, 
														   val, NULL));
						return TRUE;
				}
				case kJSTypeString:
				{
						gchar * cv = seed_value_to_locale_string(val);

						g_value_init(ret, G_TYPE_STRING);
						g_value_take_string(ret, cv);
						return TRUE;
				}
				}
				break;
		}
		}
	
		if(g_type_is_a(type, G_TYPE_ENUM) && JSValueIsNumber(eng->context, val))
		{
				if(!JSValueIsNumber(eng->context, val))
						goto bad_type;
			
				g_value_init(ret, type);
				/* What? */
				ret->data[0].v_long = JSValueToNumber(eng->context, val, NULL);
				return TRUE;
		}
		else if(g_type_is_a(type, G_TYPE_FLAGS)
				&& JSValueIsNumber(eng->context, val))
		{
				if(!JSValueIsNumber(eng->context, val))
						goto bad_type;
			
				g_value_init(ret, type);
				ret->data[0].v_long = JSValueToNumber(eng->context, val, NULL);
				return TRUE;
		}
		else if(g_type_is_a(type, G_TYPE_OBJECT) 
				&& (JSValueIsNull(eng->context, val) 
					|| seed_value_is_gobject(val)))
		{
				GObject * o = seed_value_to_gobject(val);
		
				if(o == NULL || g_type_is_a(G_OBJECT_TYPE(o), type))
				{
						g_value_init(ret, G_TYPE_OBJECT);
						g_value_set_object(ret, o);
						return TRUE;
				}

				g_object_unref(o);
		}
	
bad_type:
		return 0;
}

SeedValue seed_value_get_property(SeedValue val, 
								  const char * name)
{
	
		JSStringRef jname = JSStringCreateWithUTF8CString(name);
		JSValueRef ret =  JSObjectGetProperty(eng->context, 
											  (JSObjectRef)val, 
											  jname, NULL);
	
		JSStringRelease(jname);
	
		return ret;
}

gboolean seed_value_set_property(JSObjectRef object, 
								 const char * name, 
								 JSValueRef value)
{
		JSStringRef jname = JSStringCreateWithUTF8CString(name);
	
		if (value)
		{
				JSObjectSetProperty(eng->context, (JSObjectRef)object, 
									jname, value, 0, 0);
		}
	
		JSStringRelease(jname);
	
		return TRUE;
}
