#include <stdio.h>
#include <gtk-3.0/gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>   //creating folders
#include <dirent.h> //counting files
#include <pango/pangocairo.h>   //fonts
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <pthread.h>
#define SDL_MAIN_HANDLED


// these objects must be global

GtkBuilder *builder;
GtkWidget *main_window;
GtkWidget *stack;
GtkWidget* gallery_menu_fixed;
GtkWidget *gallery_menu_background;

GtkWidget *main_icon_buttons[7]; // number of icons + 1
GtkWidget *main_time_label;

GtkWidget *homepage_fixed;
GtkWidget *homepage_wallpaper;
GdkPixbuf *homepage_wallpaper_pixIn;
GdkPixbuf *homepage_wallpaper_pix;

//gallery variables-----------------------------------------------------------------------------------
GtkWidget *camera_images_fixed;
GtkWidget *camera_images_grid;
GtkWidget *camera_images_scrolled_window;
GtkWidget *camera_images_icon;
GtkWidget *no_of_camera_images_label;

GdkPixbuf *pixIn_gallery_menu_background;
GdkPixbuf *pix_gallery_menu_background;

GdkPixbuf *pixIn_camera_images_grid;
GdkPixbuf *pix_camera_images_grid;

GdkPixbuf *pixIn_camera_images_icon;
GdkPixbuf *pix_camera_images_icon;

char **images_list;

//music player variables-------------------------------------------------------------------------------
GtkWidget *music_menu_scrolled_window;
GtkWidget *music_menu_fixed; // assigning gallery menu fixed
GtkWidget *music_menu_grid;
GtkWidget *music_menu_add_new_folder_button;
GtkWidget *music_files_fixed;
GtkWidget *music_files_scrolled_window;

int music_menu_folders_number;
char **music_files_list;
char music_directory[128];
GtkWidget *playpausebutton;
int isPaused = 0;
Mix_Music* music;
char clicked_music_filepath[512];
GtkWidget *slider;
int music_is_playing;

// function prototypes
void show_homepage();

//Gallery functions--------------------------------------------------------------------------------------
void generate_gallery_menu_fixed();
void generate_camera_images_fixed();
void a_camera_image_clicked(GtkWidget *button, gpointer i);


//Music Player functions-------------------------------------------------------------------------------
void music_menu_add_new_folder_button_clicked();
void music_menu_folder_button_clicked();
void generate_music_files_fixed();

gboolean update_main_time_label(gpointer user_data)
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%I:%M:%S %p", timeinfo);


    gtk_label_set_text(GTK_LABEL(main_time_label), buffer);
        // this allows the function to be called repeatedly
    return G_SOURCE_CONTINUE;
}

//static void activate(GtkApplication *app)
//{
//    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
//    window = gtk_application_window_new(app);
//
//    gtk_window_set_title(GTK_WINDOW(window), "WIP app shelf");
//    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
//
//    //GtkWidget *overlay = gtk_overlay_new();
//    //gtk_window_set_child(GTK_WINDOW(window), overlay);
//        //hierarchy- window > overlay > grid > scrolledWindow > textView
//
//    //gtk_widget_set_halign (overlay, GTK_ALIGN_CENTER);
//    //gtk_widget_set_valign (overlay, GTK_ALIGN_START);
//
//    gtk_window_present(GTK_WINDOW(window));
//}

    //compiling command
//gcc app.c -o app $(pkg-config --cflags --libs gtk+-3.0) -Wl,--export-all-symbols
int SDL_main(int argc, char *argv[])
{
    //GtkApplication *app;  // this is gtk4 specific

    //app = gtk_application_new("my.first.app", G_APPLICATION_DEFAULT_FLAGS);


    gtk_init(NULL, NULL);   //GtkApplication works in gtk4
    //GError *error = NULL;
    //builder = gtk_builder_new();
    builder = gtk_builder_new_from_file("glade.glade"); // initialising builder object
    if(!builder)
    {
        g_printerr("Failed to load Glade file. \n");
        return 1;
    }
        //initialising stack
    stack = gtk_stack_new();
    //g_error_free(error);

    //gtk_builder_connect_signals(builder, NULL);

    // just in case we need this
    for(int i = 1; i < 7; i++)
    {
        // getting each button and applying rounded borders
        char buttonname[64];
        sprintf(buttonname, "icon_button%d", i);
        main_icon_buttons[i] = GTK_WIDGET(gtk_builder_get_object(builder, buttonname));
        GtkCssProvider *css_provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(css_provider,
            //".icon_button { border-radius: 25px; background: none; box-shadow: none; border: none;}",    // adds a class which is mentioned in the glad
            ".icon_button { border-radius: 25px; background: white;}",    // adds a class which is mentioned in the glad
            -1, NULL);
        GtkStyleContext *context = gtk_widget_get_style_context((main_icon_buttons[i]));
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


            //adding image to each button

            //TODO
        //GtkWidget *icon_button_image = gtk_image_new();
        //char icon_buttonname[64] = "icon_";
        //strcat(icon_buttonname, buttonname);
        //icon_button_image = GTK_WIDGET(gtk_builder_get_object(builder, icon_buttonname));
        //homepage_wallpaper_pixIn = gdk_pixbuf_new_from_file("assets/homepage_wallpaper.jpg", NULL);    // unscaled pixbuf
        //homepage_wallpaper_pix = gdk_pixbuf_scale_simple(homepage_wallpaper_pixIn, 1000, 800, GDK_INTERP_NEAREST); // scaled pixbuf
        //gtk_image_set_from_pixbuf(GTK_IMAGE(homepage_wallpaper), homepage_wallpaper_pix);


    }

    //gtk_widget_set_name(main_icon_buttons[1], "rounded");



    main_time_label = GTK_WIDGET(gtk_builder_get_object(builder, "main_time_label"));
    gtk_widget_set_halign(main_time_label, GTK_ALIGN_CENTER);
    guint timeout_id = g_timeout_add_seconds(1, update_main_time_label, NULL);
    //PangoFontDescription *font_desc = pango_font_description_from_string("Sans 32");
    //gtk_widget_override_font(main_time_label, font_desc);

    //GtkWidget *icon_grid = GTK_WIDGET(gtk_builder_get_object(builder, "icon_grid"));
    //gtk_widget_set_halign(GTK_WIDGET(icon_grid), GTK_ALIGN_CENTER);

    homepage_wallpaper = gtk_image_new();
    homepage_wallpaper = GTK_WIDGET(gtk_builder_get_object(builder, "homepage_wallpaper"));
    homepage_wallpaper_pixIn = gdk_pixbuf_new_from_file("assets/homepage_wallpaper.jpg", NULL);    // unscaled pixbuf
    homepage_wallpaper_pix = gdk_pixbuf_scale_simple(homepage_wallpaper_pixIn, 1000, 800, GDK_INTERP_NEAREST); // scaled pixbuf
    gtk_image_set_from_pixbuf(GTK_IMAGE(homepage_wallpaper), homepage_wallpaper_pix);


        //gtk_widget_add_css_class(homepage_fixed, "homepage_wallpaper");
    //GtkCssProvider *css_provider = gtk_css_provider_new();
    //const gchar *css_data = "homepage_fixed {background-color: red; background-size: cover; background-repeat: no-repeat;}";
    //const gchar *css_data = "homepage_fixed {background-image: url(\"assets/homepage_wallpaper.jpg\"); background-size: cover; background-repeat: no-repeat;}";
      //gtk_css_provider_load_from_string(css_provider, css_data);
    //gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);
    //gtk_css_provider_load_from_path(css_provider, "asdf.css", NULL);

    //GtkStyleContext *context = gtk_widget_get_style_context(homepage_fixed);
    //gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);





    // for (int i = 0; i < fileCount; i++) {
    //     // Create image widget from file
    //     GtkWidget *image = gtk_image_new();
    //     pixIn_camera_images_grid = gdk_pixbuf_new_from_file(images_list[i], NULL);    // unscaled pixbuf
    //     pix_camera_images_grid = gdk_pixbuf_scale_simple(pixIn_camera_images_grid, 176, 176, GDK_INTERP_NEAREST); // scaled pixbuf
    //     gtk_image_set_from_pixbuf(GTK_IMAGE(image), pix_camera_images_grid);
    // //     // Attach image to the grid
    //     gtk_grid_attach(GTK_GRID(camera_images_grid), image, i%5, i/5, 1, 1);
    // }


    images_list = (char **)malloc(1024*sizeof(char *));
    // Gallery GtkFixed's definition
    gallery_menu_fixed = GTK_WIDGET(gtk_builder_get_object(builder, "gallery_menu_fixed")); // assigning gallery menu fixed
    camera_images_fixed = GTK_WIDGET(gtk_builder_get_object(builder, "camera_images_fixed"));
    camera_images_grid = GTK_WIDGET(gtk_builder_get_object(builder, "camera_images_grid"));

    //setting camera images scrolled menu
    camera_images_scrolled_window = GTK_WIDGET(gtk_builder_get_object(builder, "camera_images_scrolled_window"));

    /* Setting gallery menu fixed stacks-------------------------------------------------------------------------------------*/
    gtk_container_add(GTK_CONTAINER(camera_images_scrolled_window), GTK_WIDGET(camera_images_fixed));//adding camera images fixed to scroll window

    // adding both pages to the GtkStack
    gtk_stack_add_named(GTK_STACK(stack), gallery_menu_fixed, "gallery_menu_fixed");
    gtk_stack_add_named(GTK_STACK(stack), camera_images_scrolled_window, "camera_images_scrolled_window");

    // Add widgets to GtkStack

    generate_camera_images_fixed();




    /* Music player code begins here------------------------------------------------------*/

    // getting ScrolledWindow and fixed from builder
    music_menu_scrolled_window = GTK_WIDGET(gtk_builder_get_object(builder, "music_menu_scrolled_window"));
    music_menu_fixed = GTK_WIDGET(gtk_builder_get_object(builder, "music_menu_fixed")); // assigning gallery menu fixed
    gtk_container_add(GTK_CONTAINER(music_menu_scrolled_window), GTK_WIDGET(music_menu_fixed));//adding camera images fixed to scroll window
    music_menu_grid = GTK_WIDGET(gtk_builder_get_object(builder, "music_menu_grid")); // assigning gallery menu fixed


    gtk_stack_add_named(GTK_STACK(stack), music_menu_scrolled_window, "music_menu_scrolled_window");


    music_files_scrolled_window = GTK_WIDGET(gtk_builder_get_object(builder, "music_files_scrolled_window"));
    music_files_fixed = GTK_WIDGET(gtk_builder_get_object(builder, "music_files_fixed"));
    gtk_container_add(GTK_CONTAINER(music_files_scrolled_window), GTK_WIDGET(music_files_fixed));//adding camera images fixed to scroll window

    gtk_stack_add_named(GTK_STACK(stack), music_files_scrolled_window, "music_files_scrolled_window");


    music_menu_folders_number = 1;
    generate_music_files_fixed();




    //music_menu_add_new_folder_button = gtk_button_new();
    //GtkWidget *music_menu_plus_image = gtk_image_new_from_icon_name("list-add", GTK_ICON_SIZE_BUTTON);
    //gtk_grid_attach(GTK_GRID(music_menu_grid), music_menu_add_new_folder_button, 1, 0, 1, 1);

    //g_signal_connect(music_menu_add_new_folder_button, "clicked", G_CALLBACK(music_menu_add_new_folder_button_clicked), NULL);


    /* main_window creation and important stuff-------------------------------------------*/
    main_window = GTK_WIDGET(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1000, 800);

        //connects destroy signal (close button) to gtk_main_quit (predefined)
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

        //creating a GtkStack that will contain the GtkFixed's of all pages
    gtk_container_add(GTK_CONTAINER(main_window), stack);


        //get all GtkFixed's from builder

    homepage_fixed = GTK_WIDGET(gtk_builder_get_object(builder, "homepage_fixed"));
        //adding all GtkFixed's to this GtkStack

    gtk_stack_add_named(GTK_STACK(stack), homepage_fixed, "homepage_fixed");

    show_homepage();

    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show_all(main_window);

    gtk_main();



    // Stop the music playback
    Mix_HaltMusic();
    // Free the music resource
    Mix_FreeMusic(music);
    // Close the audio system
    Mix_CloseAudio();
    // Quit the SDL subsystems
    SDL_Quit();
    return EXIT_SUCCESS;


        // once application starts, it makes a new window
    //g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        // this runs the application till we close it
    //int status = g_application_run(G_APPLICATION(app), 0, NULL);
        // this releases the reference app
    //g_object_unref(app);

    //g_signal_connect(window, "destroy", G_CALLBACK(g_main_quit()), NULL);
    //while(1)
    //{
    //    g_main_context_iteration (NULL, TRUE);  // this calls the main function again
    //}

    //return status;
}




//Gallery functions----------------------------------------------------------------------------------

void generate_gallery_menu_fixed()
{


    gallery_menu_background = gtk_image_new();
    gallery_menu_background = GTK_WIDGET(gtk_builder_get_object(builder, "gallery_menu_background"));
    pixIn_gallery_menu_background = gdk_pixbuf_new_from_file("assets/gallery_menu_background.jpg", NULL);    // unscaled pixbuf
    pix_gallery_menu_background = gdk_pixbuf_scale_simple(pixIn_gallery_menu_background, 1000, 800, GDK_INTERP_NEAREST); // scaled pixbuf
    gtk_image_set_from_pixbuf(GTK_IMAGE(gallery_menu_background), pix_gallery_menu_background);

    camera_images_icon = gtk_image_new();// assigning camera_images_icon
    camera_images_icon = GTK_WIDGET(gtk_builder_get_object(builder, "camera_icon"));
    pixIn_camera_images_icon = gdk_pixbuf_new_from_file("assets/camera_icon.jpg", NULL);    // unscaled pixbuf
    pix_camera_images_icon = gdk_pixbuf_scale_simple(pixIn_camera_images_icon, 200, 250, GDK_INTERP_NEAREST); // scaled pixbuf
    gtk_image_set_from_pixbuf(GTK_IMAGE(camera_images_icon), pix_camera_images_icon);
}

void generate_camera_images_fixed()
{



    char *user_profile = getenv("USERPROFILE"); // gets the system variables %USERPROFILE% (only in windows)
    char camera_directory[128];
    sprintf(camera_directory, "%s\\Pictures\\Camera Roll", user_profile);

    mkdir(camera_directory); // 0 if it makes a folder, -1 if already exist

    struct dirent *dir;
    DIR *d = opendir(camera_directory);

    int fileCount = -2; // . and .. also count as folders

    /* loop to iterate through camera roll directory images using dir pointer and storing the image file name using dirent struct
       and storing the name in a array of string pointers named images_list.
       Then creating a gtk button , adding the image to the thebutton , adding a signal to the button then adding the button to the grid*/
    while((dir = readdir(d)) != NULL)
    {
        if(fileCount >= 0)
        {
            char *imagefilepath = (char *)malloc(500*(sizeof(char)));
            char *button_name = (char *)malloc(500*sizeof(char));

            //condition to filter out image files
            if (g_str_has_suffix(dir->d_name, ".jpg") || g_str_has_suffix(dir->d_name, ".png") || g_str_has_suffix(dir->d_name, ".jpeg"))
            {
                sprintf(imagefilepath, "%s\\%s", camera_directory, dir->d_name); //adding image paths to imagefilepath string
                images_list[fileCount] = imagefilepath;//adding image path to array of names

                GtkWidget *button = gtk_button_new();// creating new gtk button

                g_signal_connect(button, "clicked", G_CALLBACK(a_camera_image_clicked), GINT_TO_POINTER(fileCount));// adding signal to button

                GtkWidget *image = gtk_image_new();// creating new gtk image
                pixIn_camera_images_grid = gdk_pixbuf_new_from_file(imagefilepath, NULL);    // adding image to the gtk image having the imagefilepath as path
                pix_camera_images_grid = gdk_pixbuf_scale_simple(pixIn_camera_images_grid, 175, 175, GDK_INTERP_NEAREST); // scaled pixbuf
                gtk_image_set_from_pixbuf(GTK_IMAGE(image), pix_camera_images_grid);

                gtk_button_set_image(GTK_BUTTON(button), image);

                // gtk_widget_add_css_class(button, "icon_button");
                //GtkCssProvider *css_provider = gtk_css_provider_new();
                //gtk_css_provider_load_from_data(css_provider, ".icon_button { background: none; box-shadow: none; border: none;}", -1, NULL);
                //GtkStyleContext *context = gtk_widget_get_style_context(button);
                //gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider),
                //GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

                // Attach image to the grid
                gtk_grid_attach(GTK_GRID(camera_images_grid), button, (fileCount)%5, (fileCount)/5, 1, 1);


                fileCount++;

            }
            else
            {
            sprintf(imagefilepath, "%s\\%s", camera_directory, dir->d_name);
            printf("NON-IMAGE FILE %s \n", imagefilepath);
            }

        }
        else
        {
            fileCount++;
        }
    }

    closedir(d);

    no_of_camera_images_label =GTK_WIDGET(gtk_builder_get_object(builder, "no_of_camera_images_label"));
    char no_of_images[20];
    if(fileCount ==1)
    {
        sprintf(no_of_images, "%d image", fileCount);
    }
    else
    {
        sprintf(no_of_images, "%d images", fileCount);
    }
    gtk_label_set_text(GTK_LABEL(no_of_camera_images_label), no_of_images);
    int n = (fileCount/5)+1;

    //setting no of rows of the grid
    gtk_grid_set_row_homogeneous(GTK_GRID(camera_images_grid), n);

}




void on_camera_images_button_clicked(GtkWidget *icon1)
{
    generate_camera_images_fixed();

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "camera_images_scrolled_window");
}

void on_camera_images_back_button_clicked(GtkWidget *icon1)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "gallery_menu_fixed");
}

void on_camera_images_home_button_clicked(GtkWidget *icon1)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "homepage_fixed");
}

void on_gallery_menu_back_button_clicked()
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "homepage_fixed");
}

void on_gallery_menu_home_button_clicked()
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "homepage_fixed");

}

void a_camera_image_clicked(GtkWidget * button, gpointer data)
{
    int i = GPOINTER_TO_INT(data);
    GtkWidget *image_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(image_window), images_list[i]);

    GdkPixbuf *pixIn_image = gdk_pixbuf_new_from_file(images_list[i], NULL);    // unscaled pixbuf

    int src_width = gdk_pixbuf_get_width(pixIn_image);
    int src_height = gdk_pixbuf_get_height(pixIn_image);

    int x = (800 * src_width)/src_height;

    GtkWidget *image = gtk_image_new_from_file(images_list[i]);
    GdkPixbuf *pix_image = gdk_pixbuf_scale_simple(pixIn_image, x, 800, GDK_INTERP_NEAREST); // scaled pixbuf

    gtk_image_set_from_pixbuf(GTK_IMAGE(image), pix_image);

    gtk_container_add(GTK_CONTAINER(image_window), image);

    // g_signal_connect(image_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        //connects destroy signal (close button) to gtk_main_quit
    gtk_widget_show_all(image_window);
}


// music player functions------------------------------------------------------------------------
gboolean slider_grabbed = FALSE;


// Function to toggle play/pause state
void play_pause_clicked(GtkButton *playpausebutton, gpointer data) {
    if (isPaused) {
        Mix_ResumeMusic();
        isPaused = 0;
        gtk_button_set_label(playpausebutton, "Pause");
    } else {
        Mix_PauseMusic();
        isPaused = 1;
        gtk_button_set_label(playpausebutton, "Play");
    }
}
void onDestroy(GtkWidget *widget, gpointer data) {
    music_is_playing = 0;
    // Stop the music playback
    Mix_HaltMusic();

    //// Free the music resource
    //Mix_FreeMusic(music);

    //// Close the audio system
    //Mix_CloseAudio();

    //// Quit the SDL subsystems
    //SDL_Quit();


    //// Quit the GTK main loop
    //gtk_main_quit();
}

void *audio_parallel_thread(void *arg){

    music_is_playing = 1;

    // Initialize SDL with audio support
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return NULL;
    }

    // Initialize SDL_mixer with clearer error handling
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 256) < 0) {
        printf("SDL_mixer initialization failed: %s\n", Mix_GetError());
        SDL_Quit();
        return NULL;
    }

    // Load the MP3 file with a valid path
    Mix_Music* music = Mix_LoadMUS(clicked_music_filepath);
    if (!music) {
        printf("Failed to load MP3: %s\n", Mix_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return NULL;
    }

    // Play the music, handling potential errors
    if (Mix_PlayMusic(music, -1) < 0) {
        printf("Failed to play music: %s\n", Mix_GetError());
        Mix_FreeMusic(music);
        Mix_CloseAudio();
        SDL_Quit();
        return NULL;
    }

    // Wait for the music to finish or be paused/stopped
    while (Mix_PlayingMusic()) {
        SDL_Delay(100);
    }

    //// Clean up resources
    //Mix_FreeMusic(music);
    //Mix_CloseAudio();
    //printf("this is sus!");
    //SDL_Quit();
    music_is_playing = 0;
    return NULL;



}
// Audio callback function
void audioCallback(void *userdata, Uint8 *stream, int len) {
    SDL_memset(stream, 0, len); // Clear the audio buffer
}

gboolean update_music_progress_slider(void *data){
    //SDL_AudioSpec audioSpec;
    //SDL_AudioDeviceID audioDevice;
    //    // Set up the audio specifications
    //audioSpec.freq = 44100; // Sample rate
    //audioSpec.format = AUDIO_S16SYS; // 16-bit signed audio
    //audioSpec.channels = 2; // Stereo
    //audioSpec.samples = 4096; // Buffer size
    //audioSpec.callback = audioCallback;
    //    // Open the audio device
    //audioDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
    //SDL_PauseAudioDevice(audioDevice, 1); //a tiny pause to avoid race conditions
    //if(music_is_playing && !slider_grabbed){
    if(music_is_playing){
        // Get the current position of the music playback in milliseconds
        double current_position = Mix_GetMusicPosition(music);

        // Get the total duration of the music in milliseconds
        double total_duration = Mix_MusicDuration(music);

        // Calculate the position percentage
        double position_percentage = (double)current_position / (double)total_duration;

        printf("player at %lf%% \n", position_percentage);
        fflush(stdout);
        // Set the slider position based on the percentage
        gtk_range_set_value(GTK_RANGE(slider), position_percentage);
        return G_SOURCE_CONTINUE;   // allows g_timeout to keep it in a loop
    }
    //printf("hey?");
    else{
        G_SOURCE_REMOVE;    // the window is closed, stop the loop
    }

    //SDL_PauseAudioDevice(audioDevice, 0); // Resume audio playback
}

// Function to handle slider value changes
void on_slider_changed(GtkWidget* widget, gpointer data) {
    //if (music_is_playing && slider_grabbed) {
    if (music_is_playing) {

        // Get the value of the slider
        double slider_value = gtk_range_get_value(GTK_RANGE(widget));

        // Get the total duration of the music in milliseconds
        double total_duration = Mix_MusicDuration(music);

        double old_position = Mix_GetMusicPosition(music);
        // Calculate the new position in milliseconds
        double new_position = (double)(slider_value * total_duration);
        //double timediff = new_position - old_position;
        //// taking modulus
        //timediff = (timediff>0) ? timediff : (-timediff);
        //if(timediff < 1){
        //    printf("no biggie \n");
        //    printf("old is %lf and new is %lf\n", old_position, new_position);
        //    printf("the diff is %lf \n", timediff);
        //    return;
        //}
        //else{
        //    printf("moving cursor \n");

        //    // Set the music playback position
        //}
        Mix_SetMusicPosition(new_position);
    }
}
//void slider_pressed(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data){
//    slider_grabbed = TRUE;
//}
//void slider_released(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data){
//    slider_grabbed = FALSE;
//}

void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data){
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeIter iter;
    //char music_directory[128];


    // Get the iter associated with the clicked row
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        // Now you can extract data from the model using the iter
        // For example, you can get the text of the first column
        gchar *filename;
        gtk_tree_model_get(model, &iter, 0, &filename, -1);
        //char clicked_music_filepath[512];
        sprintf(clicked_music_filepath, "%s\\%s", music_directory, filename);

        g_print("clicked on file %s\n", clicked_music_filepath);



    // Create a vertical box to hold the buttons
    GtkWidget *playbar_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *music_player_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(music_player_window), filename);

    g_signal_connect(music_player_window, "destroy", G_CALLBACK(onDestroy), NULL);


    // Create the slider
    slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.0001);
    gtk_widget_set_hexpand(slider, TRUE);
    g_signal_connect(slider, "value-changed", G_CALLBACK(on_slider_changed), NULL);
    //g_signal_connect(music_player_window, "button-press-event", G_CALLBACK(on_slider_changed), NULL);
    //g_signal_connect(music_player_window, "button-release-event", G_CALLBACK(slider_released), NULL);

    gtk_box_pack_start(GTK_BOX(playbar_box), slider, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(music_player_window), playbar_box);

    // Create the play/pause button
    playpausebutton = gtk_button_new_with_label("Pause");
    g_signal_connect(playpausebutton, "clicked", G_CALLBACK(play_pause_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(playbar_box), playpausebutton, TRUE, TRUE, 0);

    //gtk_container_add(GTK_CONTAINER(music_player_window), playpausebutton);
    gtk_widget_show_all(music_player_window);


    // SDL2_mixer Code for playing music------------------

    // Start audio playback in a separate thread
    pthread_t audio_thread_id;
    pthread_create(&audio_thread_id, NULL, audio_parallel_thread, NULL);


    // Start the timeout function to update the slider
    g_timeout_add(500, update_music_progress_slider, NULL);

    };
}
void generate_music_files_fixed(){


    char *user_profile = getenv("USERPROFILE"); // gets the system variables %USERPROFILE% (only in windows)
    //char music_directory[128];
    sprintf(music_directory, "%s\\Music", user_profile);

    mkdir(music_directory); // 0 if it makes a folder, -1 if already exist

    struct dirent *dir;
    DIR *d = opendir(music_directory);

    music_files_list = (char**) malloc(1024 * sizeof(char*));
    int musicfileCount = -2; // . and .. also count as folders
    GtkListStore *list_store = gtk_list_store_new(1, G_TYPE_STRING);

    /* loop to iterate through camera roll directory images using dir pointer and storing the image file name using dirent struct
       and storing the name in a array of string pointers named music_files_list.
       Then creating a gtk button , adding the image to the thebutton , adding a signal to the button then adding the button to the grid*/
    while((dir = readdir(d)) != NULL)
    {
        if(musicfileCount >= 0)
        {
            char *musicfilepath = (char *)malloc(500*(sizeof(char)));
            char *button_name = (char *)malloc(500*sizeof(char));

            //condition to filter out image files
            if (g_str_has_suffix(dir->d_name, ".mp3"))
            {
                sprintf(musicfilepath, "%s\\%s", music_directory, dir->d_name); //adding image paths to musicfilepath string
                music_files_list[musicfileCount] = musicfilepath;//adding image path to array of names

                //GtkWidget *button = gtk_button_new();// creating new gtk button

                //g_signal_connect(button, "clicked", G_CALLBACK(a_camera_image_clicked), GINT_TO_POINTER(musicfileCount));// adding signal to button


                // gtk_widget_add_css_class(button, "icon_button");
                //GtkCssProvider *css_provider = gtk_css_provider_new();
                //gtk_css_provider_load_from_data(css_provider, ".icon_button { background: none; box-shadow: none; border: none;}", -1, NULL);
                //GtkStyleContext *context = gtk_widget_get_style_context(button);
                //gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider),
                //GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

                // Attach image to the grid
                //gtk_grid_attach(GTK_GRID(camera_images_grid), button, (musicfileCount)%5, (musicfileCount)/5, 1, 1);
                GtkWidget *list_view = gtk_tree_view_new();
                gtk_tree_view_set_model(GTK_TREE_VIEW(list_view), GTK_TREE_MODEL(list_store));

                // Add columns to the list view
                GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
                GtkTreeViewColumn *column1 = gtk_tree_view_column_new_with_attributes("Music Files", renderer, "text", 0, NULL);
                GtkTreeViewColumn *column2 = gtk_tree_view_column_new_with_attributes("Author", renderer, "text", 1, NULL);

                gtk_tree_view_append_column(GTK_TREE_VIEW(list_view), column1);
                //gtk_tree_view_append_column(GTK_TREE_VIEW(list_view), column2);

                // Connect signal for row activation
                g_signal_connect(list_view, "row-activated", G_CALLBACK(on_row_activated), NULL);

                // Add music files to the list store
                // You can use a file dialog or directory scan to get the music files
                //printf("music is %s \n", musicfilepath);
                GtkTreeIter iter;
                gtk_list_store_append(list_store, &iter);
                gtk_list_store_set(list_store, &iter, 0, dir->d_name, -1);

                // Add list view to window and show all widgets
                gtk_fixed_put(GTK_FIXED(music_files_fixed), list_view, 75, 100);

                musicfileCount++;

            }
            else
            {
            sprintf(musicfilepath, "%s\\%s", music_directory, dir->d_name);
            printf("NON-MUSIC FILE %s \n", musicfilepath);
            }

        }
        else
        {
            musicfileCount++;
        }
    }

    closedir(d);

    no_of_camera_images_label =GTK_WIDGET(gtk_builder_get_object(builder, "no_of_music_files_label"));
    char no_of_images[20];
    if(musicfileCount ==1)
    {
        sprintf(no_of_images, "%d music file", musicfileCount);
    }
    else
    {
        sprintf(no_of_images, "%d music files", musicfileCount);
    }
    gtk_label_set_text(GTK_LABEL(no_of_camera_images_label), no_of_images);
    int n = (musicfileCount/5)+1;





}

void on_music_menu_button1_clicked(GtkWidget *music_menu_button1){
    generate_music_files_fixed();


    gtk_stack_set_visible_child_name(GTK_STACK(stack), "music_files_scrolled_window");
}
void music_menu_add_new_folder_button_clicked(){
    //GtkWidget *dialog = gtk_file_chooser_dialog_new(
    //    "Select Folder",                                // Dialog title
    //    GTK_WINDOW(main_window),   // Parent window
    //    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,         // Action: Select folder
    //    "_Cancel", GTK_RESPONSE_CANCEL,                 // Cancel button
    //    "_Open", GTK_RESPONSE_ACCEPT,                  // Open button
    //    NULL);                                         // End of arguments

    //// Show the dialog and wait for a response
    //gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    //// Check the response
    //gchar *folder_path;
    //if (response == GTK_RESPONSE_ACCEPT) {
    //    // Get the selected folder path
    //    folder_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

    //    g_print("Selected folder: %s\n", folder_path);

    //    // Free the folder path string
    //    g_free(folder_path);
    //}
    //else
    //{
    //    // Destroy the dialog if canceled
    //    gtk_widget_destroy(dialog);
    //    return;
    //}

    //// Destroy the dialog
    //gtk_widget_destroy(dialog);

    ////gtk_container_remove(GTK_CONTAINER(music_menu_grid), music_menu_add_new_folder_button);

    ////GtkWidget *music_menu_folder_button = gtk_button_new();
    ////GtkWidget *folder_image = gtk_image_new_from_icon_name("folder", GTK_ICON_SIZE_BUTTON);
    ////gtk_button_set_image(GTK_BUTTON(music_menu_folder_button), folder_image);
    ////g_signal_connect(music_menu_folder_button, "clicked", G_CALLBACK(music_menu_folder_button_clicked), folder_path);
    ////gtk_grid_attach(GTK_GRID(music_menu_grid), music_menu_folder_button, music_menu_folders_number%3, music_menu_folders_number/3, 1, 1);

    ////music_menu_folders_number++;

    ////GtkWidget *susbutton = gtk_button_new_with_label("sussy");
    ////GtkWidget *sussybutton = gtk_button_new_with_label("sussssyyyeirjeriajlsdkfjaweioruaosidu");
    ////printf("sus");

    ////GtkWidget *susFixed = gtk_fixed_new();
    ////GtkWidget *susgrid = gtk_grid_new();
    ////gtk_fixed_put(GTK_FIXED(susFixed), susbutton, 400, 500);
    ////gtk_grid_attach(GTK_GRID(susgrid), sussybutton, music_menu_folders_number%3, music_menu_folders_number/3, 1, 1);
    ////
    ////gtk_stack_add_named(GTK_STACK(stack), susFixed, "susFixed");
    ////gtk_stack_set_visible_child_name(GTK_STACK(stack), "susFixed");





    //// Create a new folder button with the folder icon
    //GtkWidget *music_menu_folder_button = gtk_button_new();
    //GtkWidget *folder_image = gtk_image_new_from_icon_name("folder", GTK_ICON_SIZE_BUTTON);
    //gtk_button_set_image(GTK_BUTTON(music_menu_folder_button), folder_image);

    //// Connect the folder button to the callback function with the folder path as data
    //g_signal_connect(music_menu_folder_button, "clicked", G_CALLBACK(music_menu_folder_button_clicked), folder_path);

    //// Add the folder button to the music menu grid
    //gtk_grid_attach(GTK_GRID(music_menu_grid), music_menu_folder_button, music_menu_folders_number % 3, music_menu_folders_number / 3, 1, 1);

    //music_menu_folders_number++;

    //// Create new buttons for the sus fixed container
    //GtkWidget *susbutton = gtk_button_new_with_label("sussy");
    //GtkWidget *sussybutton = gtk_button_new_with_label("sussssyyyeirjeriajlsdkfjaweioruaosidu");

    //// Create a new fixed container and a grid to hold the buttons
    //susFixed = gtk_fixed_new();
    //susgrid = gtk_grid_new();

    //// Attach buttons to the grid
    //gtk_grid_attach(GTK_GRID(susgrid), susbutton, 0, 0, 1, 1);
    //gtk_grid_attach(GTK_GRID(susgrid), sussybutton, 1, 0, 1, 1);

    //// Add the grid to the fixed container
    //gtk_container_add(GTK_CONTAINER(susFixed), susgrid);
    //ADDING ANYTHING TO A FIXED DOES NOT WORK WHEN IT IS CALLED OUTSIDE MAIN() i.e CALLED BY A BUTTON!!!!!!

    //// Add the fixed container to the GtkStack
    //gtk_stack_add_named(GTK_STACK(stack), susFixed, "susFixed");

    // Set the visible child of the stack to the newly added fixed container




    //gtk_grid_attach(GTK_GRID(music_menu_grid), music_menu_add_new_folder_button, music_menu_folders_number%3, music_menu_folders_number/3, 1, 1);
        //music_menu_add_new_folder_button_clicked
    //gtk_widget_show(music_menu_fixed);
    //gtk_stack_set_visible_child_name(GTK_STACK(stack), "music_menu_scrolled_window");
}

void music_menu_folder_button_clicked(GtkWidget *music_menu_folder_button, gpointer data){
    printf("THE BUTTON PRESSED IS FOR %s", data);
    ;
}

void on_music_menu_back_button_clicked(){
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "homepage_fixed");

}
void on_music_menu_home_button_clicked(){
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "homepage_fixed");

}

void on_music_files_back_button_clicked(){
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "music_menu_scrolled_window");

}

void on_music_files_home_button_clicked(){
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "homepage_fixed");

}
/* homepage functions --------------------------------------------- */

void show_homepage()
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "homepage_fixed");
}
void on_icon_button1_clicked(GtkWidget *icon_button1)
{
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "music_menu_scrolled_window");

    printf("button1 clicked \n");
}
void on_icon_button2_clicked(GtkWidget *icon_button1)
{
    generate_gallery_menu_fixed();

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "gallery_menu_fixed");
    printf("button2 clicked \n");
}
void on_icon_button3_clicked(GtkWidget *icon_button1)
{
    printf("button3 clicked \n");
    system("start microsoft.windows.camera:");
}
void on_icon_button4_clicked(GtkWidget *icon_button1)
{
    printf("button4 clicked \n");
}
void on_icon_button5_clicked(GtkWidget *icon_button1)
{
    printf("button5 clicked \n");
}
void on_icon_button6_clicked(GtkWidget *icon_button1)
{
    printf("button6 clicked \n");
}
