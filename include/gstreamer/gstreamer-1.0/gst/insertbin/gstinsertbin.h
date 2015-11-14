/*
 * GStreamer
 *
 *  Copyright 2013 Collabora Ltd
 *   @author: Olivier Crete <olivier.crete@collabora.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */



#ifndef __GST_INSERT_BIN_H__
#define __GST_INSERT_BIN_H__

#ifndef GST_USE_UNSTABLE_API
#warning "The GStreamer insertbin library is unstable API and may change in future."
#warning "You can define GST_USE_UNSTABLE_API to avoid this warning."
#endif

#include <gst/gst.h>

G_BEGIN_DECLS
#define GST_TYPE_INSERT_BIN            (gst_insert_bin_get_type())
#define GST_INSERT_BIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_INSERT_BIN,GstInsertBin))
#define GST_IS_INSERT_BIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_INSERT_BIN))
#define GST_INSERT_BIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass) ,GST_TYPE_INSERT_BIN,GstInsertBinClass))
#define GST_IS_INSERT_BIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass) ,GST_TYPE_INSERT_BIN))
#define GST_INSERT_BIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj) ,GST_TYPE_INSERT_BIN,GstInsertBinClass))
typedef struct _GstInsertBin GstInsertBin;
typedef struct _GstInsertBinClass GstInsertBinClass;
typedef struct _GstInsertBinPrivate GstInsertBinPrivate;

/**
 * GstInsertBinCallback:
 * @insertbin: A #GstInsertBin
 * @element: The #GstElement on which the operation was performed
 * @success: %TRUE if the operation was successful
 * @user_data: The user data passed
 *
 * This is the prototype of callbacks to be called when the operation completes.
 * It could be called at any time, including as a re-entrant call while the
 * operation is requested.
 */

typedef void (*GstInsertBinCallback) (GstInsertBin *insertbin,
    GstElement *element,
    gboolean success,
    gpointer user_data);

/**
 * GstInsertBin:
 *
 * The object structure.
 */
struct _GstInsertBin
{
  GstBin parent;

  /*< private >*/
  GstInsertBinPrivate *priv;
};

/**
 * GstInsertBinClass:
 *
 * The object class structure.
 */
struct _GstInsertBinClass
{
  GstBinClass parent_class;
};

GType gst_insert_bin_get_type (void);

GstElement *gst_insert_bin_new (const gchar * name);

void gst_insert_bin_prepend (GstInsertBin * self, GstElement * element,
    GstInsertBinCallback callback, gpointer user_data);
void gst_insert_bin_append (GstInsertBin * self, GstElement * element,
    GstInsertBinCallback callback, gpointer user_data);
void gst_insert_bin_insert_before (GstInsertBin * self,
    GstElement * element, GstElement * sibling,
    GstInsertBinCallback callback, gpointer user_data);
void gst_insert_bin_insert_after (GstInsertBin * self,
    GstElement * element, GstElement * sibling,
    GstInsertBinCallback callback, gpointer user_data);
void gst_insert_bin_remove (GstInsertBin * self, GstElement * element,
    GstInsertBinCallback callback, gpointer user_data);


G_END_DECLS
#endif /* __GST_INSERT_BIN_H__ */
