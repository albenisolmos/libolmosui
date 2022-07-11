#include <gio/gdesktopappinfo.h>
#include <olmos/ol_string.h>
#include <olmos/ol_desktop.h>

#include "ol_app_list.h"

void ol_ui_app_list_new(char **keys, GtkWidget *container,
                        FuncBuilder builder, FuncAppender appender)
{
	GDesktopAppInfo *app_info;
	guint len_keys = g_strv_length(keys);
	guint len_list;
	char **app_id_list = ol_desktop_get_apps_list(&len_list);
	char *key;
	
	for (guint i = 0; i < len_list; i++)
	{
		app_info = g_desktop_app_info_new(app_id_list[i]);

		if (app_info == NULL) {
			g_warning("No desktop file: %s", app_id_list[i]);
			continue;
		}

		char *val_keys[len_keys];

		for (guint j = 0; j < len_keys; j++)
		{
			key = g_desktop_app_info_get_string(app_info, keys[j]);

			if (!key) {
				g_warning("Not key (%s) in %s", keys[j], app_id_list[i]);
				continue;
			}

			val_keys[j] = key;
		}

		g_object_unref(app_info);
		appender(container, builder(val_keys));
	}
}
