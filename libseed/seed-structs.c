/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * seed-structs.h
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

JSClassRef seed_struct_class = 0;

typedef struct _SeedStructPrivates
{
	gpointer object;
	GIBaseInfo * info;
} SeedStructPrivates;

static void seed_struct_finalize(JSObjectRef younion)
{
		g_free(JSObjectGetPrivate(younion));
}

static JSValueRef seed_struct_get_property(JSContextRef context,
					   JSObjectRef object,
					   JSStringRef property_name,
					   JSValueRef * exception)
{
	SeedStructPrivates * priv;
	GIBaseInfo * info;
	GIFieldInfo * field = 0;
	GITypeInfo * prop_type;
	GIInfoType obj_type;
	GType prop_gtype;
	gint num_fields = 0, i, length, offset = 0;
	gchar * cproperty_name;
	GValue gval = {0};
	JSValueRef ret;
	
	priv = (SeedStructPrivates*)JSObjectGetPrivate(object);
	info = (GIBaseInfo *)priv->info;
	
	obj_type = g_base_info_get_type(info);
	
	length = JSStringGetMaximumUTF8CStringSize(property_name);
	cproperty_name = malloc(length * sizeof(gchar));
	JSStringGetUTF8CString(property_name, cproperty_name, length);
	
	if (obj_type == GI_INFO_TYPE_UNION)
			num_fields = g_union_info_get_n_fields((GIUnionInfo *)info);	
	else if (obj_type == GI_INFO_TYPE_STRUCT)
			num_fields = g_struct_info_get_n_fields((GIStructInfo *)info);
	for (i = 0; i < num_fields; i++)
	{
			if (obj_type == GI_INFO_TYPE_UNION)
					field = g_union_info_get_field((GIUnionInfo *)info, i);
			else if (obj_type == GI_INFO_TYPE_STRUCT)
					field = g_struct_info_get_field((GIStructInfo*)info, i);
			offset += g_field_info_get_size(field);

			if (!strcmp(g_base_info_get_name((GIBaseInfo*)field),
											 cproperty_name))
					break;
			else
					field = 0;
	}
	if (!field)
			return 0;
	
	prop_type = g_field_info_get_type(field);
	prop_gtype = seed_gi_type_to_gtype(prop_type, 
									   g_type_info_get_tag(prop_type));
	
	if (prop_gtype)
			g_value_init(&gval, prop_gtype);
	switch (g_type_info_get_tag(prop_type))
	{
	case GI_TYPE_TAG_VOID:
			return 0;
	case GI_TYPE_TAG_BOOLEAN:
			g_value_set_boolean(&gval,*(gboolean *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_INT8:
			g_value_set_char(&gval,*(char *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_UINT8:
			g_value_set_uchar(&gval,*(guchar *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_INT16:
			g_value_set_int(&gval,*(short *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_UINT16:
			g_value_set_uint(&gval,*(ushort *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_INT32:
			g_value_set_int(&gval,*(gint32 *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_UINT32:
			g_value_set_uint(&gval,*(guint32 *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_LONG:
	case GI_TYPE_TAG_INT64:
			g_value_set_long(&gval,*(glong *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_ULONG:
	case GI_TYPE_TAG_UINT64:
			g_value_set_ulong(&gval,*(gulong *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_INT:
			g_value_set_int(&gval,*(gint *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_UINT:
			g_value_set_uint(&gval,*(guint *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_SSIZE:
			g_value_set_int(&gval,*(gint *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_SIZE:
			g_value_set_int(&gval,*(gint *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_FLOAT:
			g_value_set_float(&gval,*(gfloat *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_DOUBLE:
			g_value_set_double(&gval,*(gdouble *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_UTF8:
			g_value_set_string(&gval,(gchar *)(priv->object + offset));
			break;
	case GI_TYPE_TAG_INTERFACE:
	{
			GIBaseInfo *interface;
			GIInfoType interface_type;
			GType required_gtype;
			GObject * gobject;
			
			interface = g_type_info_get_interface(prop_type);
			interface_type = g_base_info_get_type(interface);
			
			
			if (interface_type == GI_INFO_TYPE_OBJECT)
			{
					g_value_set_object(&gval,
									   *(gpointer *)(priv->object + offset));
					break;
			}
			else if (interface_type == GI_INFO_TYPE_ENUM)
			{
					g_value_set_long(&gval, 
									 *(glong *)(priv->object + offset));
					break;
			}
			else if ((interface_type == GI_INFO_TYPE_STRUCT) || 
					 (interface_type == GI_INFO_TYPE_UNION))
			{
					return seed_make_struct(((gpointer *)
											  (priv->object +offset)),
											g_type_info_get_interface(prop_type));
			}
	}
	
	default:
			return FALSE;
			
	}
	ret = seed_value_from_gvalue(&gval);

	g_value_unset(&gval);
	g_free(cproperty_name);
	return ret;
}


JSClassDefinition gobject_struct_def[] = {
		0, /* Version, always 0 */
		0,
		"gobject_union", /* Class Name */
		NULL, /* Parent Class */
		NULL, /* Static Values */
		NULL, /* Static Functions */
		NULL,
		seed_struct_finalize, /* Finalize */
		NULL, /* Has Property */
		seed_struct_get_property, /* Get Property */
		NULL,  /* Set Property */
		NULL, /* Delete Property */
		NULL, /* Get Property Names */
	        NULL, /* Call As Function */
		NULL, /* Call As Constructor */
		NULL, /* Has Instance */
		NULL  /* Convert To Type */
};

JSObjectRef seed_make_union(gpointer younion, GIBaseInfo * info)
{
	SeedStructPrivates * privates;
	if (!seed_struct_class)
		seed_struct_class = JSClassCreate(gobject_struct_def);
	
	privates = g_new0(SeedStructPrivates, 1);

	privates->object = younion;
	privates->info = info;
	
	return JSObjectMake(eng->context, seed_struct_class, privates);
}

JSObjectRef seed_make_struct(gpointer strukt, GIBaseInfo * info)
{
		seed_make_union(strukt, info);
}
