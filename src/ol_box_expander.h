#ifndef __OL_DND_EXPANDER_H__
#define __OL_DND_EXPANDER_H__

#include <gtk/gtk.h>

typedef enum {
	BULLET_LEFT = 1 << 0,
	BULLET_RIGHT = 1 << 1,
} BulletFlags;

typedef struct {
	BulletFlags bullets;
	int index;
	int last_index;
	GtkWidget *source;
} olDNDExpander;

olDNDExpander *ol_dnd_expander_new(GtkContainer *box);
void ol_dnd_expander_setup_child(olDNDExpander *self, GtkWidget *child, GtkTargetEntry *entries);

#endif
