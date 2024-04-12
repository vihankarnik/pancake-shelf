#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>   //creating folders
#include <dirent.h> //counting files

GtkWidget *window;

static void activate(GtkApplication *app){
    window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "WIP app shelf");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *overlay = gtk_overlay_new();
    gtk_window_set_child(GTK_WINDOW(window), overlay);
        //hierarchy- window > overlay > grid > scrolledWindow > textView

    gtk_widget_set_halign (overlay, GTK_ALIGN_CENTER);
    gtk_widget_set_valign (overlay, GTK_ALIGN_START);

    gtk_window_present(GTK_WINDOW(window));
}

int main(){
    GtkApplication *app;
    //gtk_init();   //GtkApplication object does it for us

    app = gtk_application_new("my.first.app", G_APPLICATION_HANDLES_OPEN);

        // once application starts, it makes a new window
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        // this runs the application till we close it
    int status;
    status = g_application_run(G_APPLICATION(app), 0, NULL);
        // this releases the reference app
    g_object_unref(app);

    //g_signal_connect(window, "destroy", G_CALLBACK(g_main_quit()), NULL);
    //while(1){
    //    g_main_context_iteration (NULL, TRUE);  // this calls the main function again
    //}

    return status;}
