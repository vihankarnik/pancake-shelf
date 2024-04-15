#include <stdio.h>
#include <gtk-3.0/gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>   //creating folders
#include <dirent.h> //counting files

// these objects must be global

GtkWidget *window;
GtkBuilder *builder;

static void activate(GtkApplication *app){
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "WIP app shelf");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    //GtkWidget *overlay = gtk_overlay_new();
    //gtk_window_set_child(GTK_WINDOW(window), overlay);
        //hierarchy- window > overlay > grid > scrolledWindow > textView

    //gtk_widget_set_halign (overlay, GTK_ALIGN_CENTER);
    //gtk_widget_set_valign (overlay, GTK_ALIGN_START);

    gtk_window_present(GTK_WINDOW(window));
}

int main(){
    //GtkApplication *app;

    //app = gtk_application_new("my.first.app", G_APPLICATION_DEFAULT_FLAGS);

    //builder = gtk_builder_new_from_file("glade.glade");

    //GError *error = NULL;
    //builder = gtk_builder_new();
    //if (!gtk_builder_add_from_file(builder, "glade.glade", &error)) {
    //    g_printerr("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA Error loading Glade file: %s\n", error->message);
    //    g_error_free(error);
    //    // Handle the error appropriately
    //}


    gtk_init(NULL, NULL);   //GtkApplication works in gtk4
    builder = gtk_builder_new_from_file("glade.glade");
    if(!builder){
        g_printerr("Failed to load Glade file. \n");
        return 1;
    }
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show(window);
    gtk_main();
    return EXIT_SUCCESS;


        // once application starts, it makes a new window
    //g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        // this runs the application till we close it
    //int status = g_application_run(G_APPLICATION(app), 0, NULL);
        // this releases the reference app
    //g_object_unref(app);

    //g_signal_connect(window, "destroy", G_CALLBACK(g_main_quit()), NULL);
    //while(1){
    //    g_main_context_iteration (NULL, TRUE);  // this calls the main function again
    //}

    //return status;
}
