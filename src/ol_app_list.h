#ifndef __OL_APP_LIST_H__
#define __OL_APP_LIST_H__

#include <gtk/gtk.h>
#include <glib.h>

typedef GtkWidget* (*FuncBuilder) (char**);
typedef void (*FuncAppender) (GtkWidget*, GtkWidget*);

void ol_ui_app_list_new(char **keys, GtkWidget *container, FuncBuilder, FuncAppender);

#endif
