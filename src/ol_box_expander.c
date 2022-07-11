#include "ol_box_expander.h"

static int get_widget_index(GtkWidget *widget);
static void on_drag_leave(GtkWidget* self, GdkDragContext* context, guint time,
		gpointer user_data);

static void on_drag_begin(GtkWidget *widget, GdkDragContext *context,
		gpointer data);

static gboolean on_drag_motion(GtkWidget *widget, GdkDragContext *context,
		gint x, gint y, guint t, gpointer data );

static void on_drag_data_received(GtkWidget *widget, GdkDragContext *context,
		gint x, gint y, GtkSelectionData *selection_data,
		guint info, guint32 time, gpointer data);

static gboolean on_drag_failed(GtkWidget* self, GdkDragContext* context,
		GtkDragResult result, gpointer user_data);

static void on_drag_end(GtkWidget* self, GdkDragContext* context,
		gpointer user_data);

olDNDExpander *ol_dnd_expander_new(GtkContainer *box)
{
	olDNDExpander *self = g_new(olDNDExpander, 1);
	self->bullets = BULLET_LEFT | BULLET_RIGHT;
	return self;
}

void ol_dnd_expander_setup_child(olDNDExpander *self, GtkWidget *child, 
		GtkTargetEntry *entries)
{
	g_return_if_fail(self != NULL || child != NULL);

	gtk_drag_source_set(child, GDK_BUTTON1_MASK, entries, 1, GDK_ACTION_MOVE);
	gtk_drag_dest_set(child, GTK_DEST_DEFAULT_ALL, entries, 1, GDK_ACTION_MOVE);

	//g_signal_connect(child, "drag-data-get", G_CALLBACK(on_drag_data_get), NULL);
	g_signal_connect(child, "drag-begin", G_CALLBACK(on_drag_begin), self);
	g_signal_connect(child, "drag-data-received",
			G_CALLBACK(on_drag_data_received), NULL);
	g_signal_connect(child, "drag-motion", G_CALLBACK(on_drag_motion),
			(gpointer) self);
	g_signal_connect(child, "drag-leave", G_CALLBACK(on_drag_leave),
			(gpointer) self);
	g_signal_connect(child, "drag-failed", G_CALLBACK(on_drag_failed),
			(gpointer) self);
	g_signal_connect(child, "drag-end", G_CALLBACK(on_drag_end),
			(gpointer) self);
}

static void displace_widget(GtkWidget *widget, int index)
{
	GtkWidget *parent = gtk_widget_get_parent(widget);
	gtk_box_reorder_child(GTK_BOX(parent), widget, index);
	gtk_widget_show_all(parent);
}

static void on_drag_data_received(
		GtkWidget *widget,
		GdkDragContext *context,
		gint x,
		gint y,
		GtkSelectionData *selection_data,
		guint target_type,
		guint32 time,
		gpointer data
) {
	//g_message("on_drag_data_received %d", target_type);
	//g_message("%s", (char*) gtk_selection_data_get_data(selection_data));
}

gboolean on_drag_failed (
		GtkWidget* self,
		GdkDragContext* context,
		GtkDragResult result,
		gpointer user_data
) {
	g_message("Failed");
	return TRUE;
}

void on_drag_end (
		GtkWidget* self,
		GdkDragContext* context,
		gpointer expander 
) {
	gtk_widget_set_opacity(self, 1.0);
	displace_widget(self, ((olDNDExpander *)expander)->last_index);
}

static void on_drag_begin(GtkWidget *self, GdkDragContext *context, gpointer _expander)
{
	olDNDExpander *expander = (olDNDExpander *) _expander;
	GtkAllocation alloc;
	cairo_surface_t *surface;
	cairo_t *cr;
	int x, y;
	double sx, sy;

	gtk_widget_get_allocation(self, &alloc);
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, alloc.width, alloc.height);
	cr = cairo_create (surface);

	gtk_style_context_add_class(gtk_widget_get_style_context(self), "drag-icon");
	gtk_widget_draw(self, cr);
	gtk_style_context_remove_class(gtk_widget_get_style_context(self), "drag-icon");

	gtk_widget_translate_coordinates(self, self, 0, 0, &x, &y);
	cairo_surface_get_device_scale(surface, &sx, &sy);
	cairo_surface_set_device_offset(surface, -x * sx, -y * sy);
	gtk_drag_set_icon_surface(context, surface);

	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	expander->source = self;
	gtk_widget_set_opacity(self, 0.0);
	gtk_widget_show(self);
}

static int get_widget_index(GtkWidget *widget)
{
	GtkWidget *parent  = gtk_widget_get_parent(widget);
	g_return_val_if_fail(parent != NULL, -1);
	GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
	return g_list_index(children, (gpointer) widget);
}


static void on_drag_leave(GtkWidget* self, GdkDragContext* context, guint time,
		gpointer expander)
{
	((olDNDExpander *) expander)->bullets = BULLET_LEFT | BULLET_RIGHT;
}

static gboolean on_drag_motion(GtkWidget *self, GdkDragContext *context,
		gint _x, gint y, guint t, gpointer _expander)
{
	if (gtk_widget_get_opacity(self) == 0.0)
		return FALSE;

	olDNDExpander *expander = (olDNDExpander *) _expander;
	GtkAllocation alloc;
	int x, mid, index,
		gap = 2;

	gtk_widget_get_allocation(self, &alloc);

	mid = alloc.x + (alloc.width / 2);
	x = alloc.x + _x;
	index = get_widget_index(self);

	if (x < (mid-gap) && expander->bullets & BULLET_LEFT)
	{ // left
		expander->bullets &= ~BULLET_LEFT;
		expander->bullets |= BULLET_RIGHT;
		index--;

		if (index == -1)
			index = 0;
		if (index != expander->last_index)
			displace_widget(expander->source, index);
		expander->last_index = index;
	}
	else if ((mid+gap) > x && expander->bullets & BULLET_RIGHT)
	{ // right
		expander->bullets &= ~BULLET_RIGHT;
		expander->bullets |= BULLET_LEFT;

		if (index != expander->last_index)
			displace_widget(expander->source, index);

		expander->last_index = index;
	}

	return  FALSE;
}
