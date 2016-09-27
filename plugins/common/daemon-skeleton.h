/**
 * Create a gnome-settings-daemon helper easily
 *
 * #define NEW gsd_media_keys_manager_new
 * #define START gsd_media_keys_manager_start
 * #define MANAGER GsdMediaKeysManager
 * #include "gsd-media-keys-manager.h"
 *
 * #include "daemon-skeleton.h"
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>

#include <glib/gi18n.h>

#ifndef PLUGIN_NAME
#error Include PLUGIN_CFLAGS in the daemon s CFLAGS
#endif /* !PLUGIN_NAME */

static MANAGER *manager = NULL;
static int timeout = -1;
static gboolean verbose = FALSE;

static GOptionEntry entries[] = {
        { "exit-time", 0, 0, G_OPTION_ARG_INT, &timeout, "Exit after n seconds time", NULL },
        { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Verbose", NULL },
        {NULL}
};

int
main (int argc, char **argv)
{
        GError *error = NULL;
        GOptionContext *context;
        GMainLoop *loop;

        bindtextdomain (GETTEXT_PACKAGE, GNOME_SETTINGS_LOCALEDIR);
        bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
        textdomain (GETTEXT_PACKAGE);

        context = g_option_context_new (NULL);
        g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
        if (!g_option_context_parse (context, &argc, &argv, &error)) {
                fprintf (stderr, "%s\n", error->message);
                g_error_free (error);
                exit (1);
        }
        g_option_context_free (context);

	loop = g_main_loop_new (NULL, FALSE);

        if (verbose)
                g_setenv ("G_MESSAGES_DEBUG", "all", TRUE);

	if (timeout > 0) {
		guint id;
		id = g_timeout_add_seconds (timeout, (GSourceFunc) g_main_loop_quit, loop);
		g_source_set_name_by_id (id, "[gnome-settings-daemon] g_main_loop_quit");
	}

        manager = NEW ();

        error = NULL;
        if (!START (manager, &error)) {
                fprintf (stderr, "Failed to start: %s\n", error->message);
                g_error_free (error);
                exit (1);
        }

        g_main_loop_run (loop);

        STOP (manager);
        g_object_unref (manager);

        return 0;
}