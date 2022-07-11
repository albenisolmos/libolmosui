#include <gtk/gtk.h>
#include "ol_window.h"

/* Only some X servers support alpha channels. Always have a fallback */
gboolean supports_alpha = FALSE;
static gboolean on_draw(GtkWidget*, cairo_t*, gpointer);
static void screen_changed(GtkWidget *widget, GdkScreen *old_screen,
		gpointer user_data);

void ol_widget_add_class(GtkWidget *widget, char *class_name)
{
	gtk_style_context_add_class(
			gtk_widget_get_style_context(widget), class_name);
}

void ol_window_setup_alpha(GtkWindow* window)
{
	ol_widget_add_class(GTK_WIDGET(window), "transparent");
	gtk_widget_add_events(GTK_WIDGET(window), GDK_BUTTON_PRESS_MASK);
	gtk_widget_set_app_paintable(GTK_WIDGET(window), TRUE);

	g_signal_connect(G_OBJECT(window), "draw", G_CALLBACK(on_draw), NULL);
	g_signal_connect(G_OBJECT(window), "screen-changed",
			G_CALLBACK(screen_changed), NULL);

	screen_changed(GTK_WIDGET(window), NULL, NULL);
}

void ol_window_setup_desktop_component(GtkWindow *window)
{
	gtk_window_set_type_hint(window, GDK_WINDOW_TYPE_HINT_DOCK);
	gtk_widget_add_events(GTK_WIDGET(window), GDK_BUTTON_PRESS_MASK);
	gtk_window_set_skip_taskbar_hint(window, TRUE);
	ol_widget_add_class(GTK_WIDGET(window), "desktop-component");
}


static void screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata) {
	/* To check if the display supports alpha channels, get the visual */
	GdkScreen *screen = gtk_widget_get_screen(widget);
	GdkVisual *visual = gdk_screen_get_rgba_visual(screen);

	if (!visual)
	{
		g_debug("Your screen does not support alpha channels!\n");
		visual = gdk_screen_get_system_visual(screen);
		supports_alpha = FALSE;
	}
	else
	{
		g_debug("Your screen supports alpha channels!\n");
		supports_alpha = TRUE;
	}

	gtk_widget_set_visual(widget, visual);
}

static gboolean on_draw(GtkWidget* widget, cairo_t *cr, gpointer user_data)
{
	cairo_save (cr);

	if (supports_alpha)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0); /* transparent */
	else
		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); /* opaque white */

	/* draw the background */
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint (cr);

	cairo_restore (cr);
	return FALSE;
}
