#include <gtk/gtk.h>
#include <seed.h>

typedef struct
{
	GtkWidget * instructions;
	GtkWidget * canvas;
} turtle_ctx_t;

static void destroy(GtkWidget * widget, gpointer data)
{
	gtk_main_quit();
}

static void run_turtle(GtkWidget * widget, gpointer data)
{
	turtle_ctx_t * turtle_ctx = (turtle_ctx_t *)data;
	GtkTextBuffer * buf = gtk_text_view_get_buffer(
								GTK_TEXT_VIEW(turtle_ctx->instructions));
	
	GtkTextIter start, end;
	
	gtk_text_buffer_get_start_iter(buf, &start);
	gtk_text_buffer_get_end_iter(buf, &end);
	
	gchar * instruction_str = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
	
	printf("%s\n", instruction_str);
}

static void step_turtle(GtkWidget * widget, gpointer data)
{

}

GtkWidget * create_ui()
{
	GtkWidget * main;			// HPaned
	GtkWidget * left;			// VBox
	GtkWidget * instructions;	// TextView
	GtkWidget * button_bar;		// HBox
	GtkWidget * run, * step;	// Buttons
	GtkWidget * canvas;			// DrawingArea
	
	turtle_ctx_t * turtle_ctx = g_malloc0(sizeof(turtle_ctx_t));
	
	main = gtk_hpaned_new();
	gtk_paned_set_position(GTK_PANED(main), 200);
	
	left = gtk_vbox_new(0, 0);
	
	instructions = gtk_text_view_new();
	
	button_bar = gtk_hbox_new(0,0);
	
	run = gtk_button_new_with_label("Run");
	step = gtk_button_new_with_label("Step");
	
	canvas = gtk_drawing_area_new();
	gtk_widget_set_size_request(canvas, 400, 400);
	
	gtk_box_pack_start(GTK_BOX(button_bar), step, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(button_bar), run, 1, 1, 0);
	
	gtk_box_pack_start(GTK_BOX(left), instructions, 1, 1, 0);
	gtk_box_pack_start(GTK_BOX(left), button_bar, 0, 1, 0);
	
	gtk_paned_add1(GTK_PANED(main), left);
	gtk_paned_add2(GTK_PANED(main), canvas);
	
	turtle_ctx->instructions = instructions;
	turtle_ctx->canvas = canvas;
	
	g_signal_connect(G_OBJECT(run), "clicked", G_CALLBACK(run_turtle),
					 (gpointer)turtle_ctx);
	g_signal_connect(G_OBJECT(step), "clicked", G_CALLBACK(step_turtle),
					 (gpointer)turtle_ctx);
	
	return main;
}

int main(int argc, char ** argv)
{
	GtkWidget * window, * ui;
	gtk_init(&argc, &argv);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	ui = create_ui();
	
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);
	
	gtk_container_add(GTK_CONTAINER(window), ui);
	gtk_widget_show_all(window);
	
	gtk_main();
	
	return 0;
}
