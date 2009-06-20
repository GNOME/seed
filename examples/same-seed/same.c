// gcc `pkg-config --cflags --libs clutter-0.9 glib-2.0` same.c -o same

#include <glib.h>
#include <clutter/clutter.h>
#include <stdlib.h>

#define WIDTH 15
#define HEIGHT 10

ClutterColor colors[5] = {{ 0xff, 0x00, 0x00, 0xff },
                          { 0x00, 0xff, 0x00, 0xff },
                          { 0x00, 0x00, 0xff, 0xff },
                          { 0xff, 0xff, 0x00, 0xff },
                          { 0x00, 0xff, 0xff, 0xff }};

typedef struct
{
	ClutterActor * actor;
	gboolean visited, closed;
	int color;
	int x, y;
} light;

light * grid[WIDTH][HEIGHT];
gboolean animating_lights = FALSE;
GQueue * click_queue;

gboolean light_on_matches(int x, int y, int color)
{
	return (grid[x][y] && (grid[x][y]->color == color));
}

GList * _connected_lights(int x, int y)
{
	GList * cl = NULL;
	
	light * l = grid[x][y];
	
	if(!l || l->visited || l->closed)
		return cl;
	
	l->visited = TRUE;
	
	cl = g_list_append(cl, grid[x][y]);
	
	if(y < HEIGHT && light_on_matches(x, y + 1, l->color))
		cl = g_list_concat(cl, _connected_lights(x, y + 1));
	
	if(y > 0 && light_on_matches(x, y - 1, l->color))
		cl = g_list_concat(cl, _connected_lights(x, y - 1));
	
	if(x < WIDTH && light_on_matches(x + 1, y, l->color))
		cl = g_list_concat(cl, _connected_lights(x + 1, y));
	
	if(x > 0 && light_on_matches(x - 1, y, l->color))
		cl = g_list_concat(cl, _connected_lights(x - 1, y));
	
	return cl;
}

GList * connected_lights(light * l)
{
	int vx, vy;
	
	for(vx = 0; vx < WIDTH; vx++)
		for(vy = 0; vy < HEIGHT; vy++)
			if(grid[vx][vy])
				grid[vx][vy]->visited = FALSE;

	return _connected_lights(l->x, l->y);
}

void update_light_position(light * l, int new_x, int new_y)
{
	l->x = new_x;
	l->y = new_y;
	
	double pos_x = (50 * l->x);
	double pos_y = ((HEIGHT - 1) * 50) - (l->y * 50);
	
	clutter_actor_animate(l->actor, CLUTTER_EASE_OUT_BOUNCE, 500,
	                      "x", pos_x,
	                      "y", pos_y,
	                      NULL);
}

gboolean animation_finished(ClutterTimeline * timeline,
                            gpointer user_data)
{
	animating_lights = FALSE;
	light * l = NULL;
	
	if(!g_queue_is_empty(click_queue))
	{
		l = (light *)g_queue_pop_head(click_queue);
		light_clicked(NULL, NULL, (gpointer)l);
	}
	
	return FALSE;
}

gboolean light_clicked(ClutterActor * actor,
                       ClutterEvent * event,
                       gpointer user_data)
{
	light * l = (light *)user_data;
	GList * cl = connected_lights(l);

	if(g_list_length(cl) < 2)
		return FALSE;
	
	if(animating_lights)
	{
		g_queue_push_tail(click_queue, l);
		return FALSE;
	}
	
	animating_lights = TRUE;

	GList * li;
	ClutterTimeline * out_timeline = clutter_timeline_new(500);
	
	for(li = cl; li != NULL; li = g_list_next(li))
	{
		light * inner_l = li->data;
		
		clutter_actor_animate_with_timeline(inner_l->actor,
		                                    CLUTTER_EASE_OUT_EXPO,
		                                    out_timeline,
		                                    "opacity",
		                                    0, NULL);
		inner_l->closed = TRUE;
	}
	
	clutter_timeline_start(out_timeline);
	
	g_signal_connect(out_timeline, "completed",
	                 G_CALLBACK(animation_finished), NULL);
	
	int vx, vy;
	int new_x = 0;
	
	for(vx = 0; vx < WIDTH; vx++)
	{
		GList * new_col = NULL;
		
		for(vy = 0; vy < HEIGHT; vy++)
		{
			if(grid[vx][vy] && !grid[vx][vy]->closed)
				new_col = g_list_append(new_col, grid[vx][vy]);
		}
		
		GList * new_col_iter;
		gboolean empty_column = TRUE;
		int new_y = 0;
		
		for(new_col_iter = new_col;
		    new_col_iter != NULL;
		    new_col_iter = g_list_next(new_col_iter))
		{
			light * l = (light *)new_col_iter->data;
			
			if(!l->closed)
				empty_column = FALSE;
			
			update_light_position(l, new_x, new_y);
			
			grid[new_x][new_y] = l;
			new_y++;
		}
		
		for(; new_y < HEIGHT; new_y++)
		{
			grid[new_x][new_y] = NULL;
		}
		
		if(!empty_column)
			new_x++;
	}
	
	for(; new_x < WIDTH; new_x++)
	{
		for(vy = 0; vy < HEIGHT; vy++)
		{
			grid[new_x][vy] = NULL;
		}
	}
	
	return FALSE;
}

gboolean light_entered(ClutterActor * actor,
                       ClutterEvent * event,
                       gpointer user_data)
{
	light * l = (light *)user_data;
	GList * cl = connected_lights(l);
	
	if(g_list_length(cl) < 2)
		return FALSE;

	GList * li;
	
	for(li = cl; li != NULL; li = g_list_next(li))
	{
		light * inner_l = li->data;
		clutter_actor_set_opacity(inner_l->actor, 255);
	}
	
	return FALSE;
}

gboolean light_left(ClutterActor * actor,
                    ClutterEvent * event,
                    gpointer user_data)
{
	light * l = (light *)user_data;
	GList * cl = connected_lights(l);
	
	if(g_list_length(cl) < 2)
		return FALSE;

	GList * li;
	
	for(li = cl; li != NULL; li = g_list_next(li))
	{
		light * inner_l = li->data;
		clutter_actor_set_opacity(inner_l->actor, 180);
	}
	
	return FALSE;
}

ClutterActor * create_light(light * l)
{
	ClutterActor * actor;
	
	actor = clutter_rectangle_new();
	clutter_rectangle_set_color(CLUTTER_RECTANGLE(actor), &colors[l->color]);
	clutter_actor_set_size(actor, 50, 50);
	clutter_actor_set_position(actor, 50 * l->x, ((HEIGHT - 1) * 50) - (50 * l->y));
	clutter_actor_set_opacity(actor, 180);
	clutter_actor_set_reactive(actor, TRUE);
	
	g_signal_connect(actor, "button-release-event",
	                 G_CALLBACK(light_clicked), l);
	g_signal_connect(actor, "enter-event",
	                 G_CALLBACK(light_entered), l);
	g_signal_connect(actor, "leave-event",
	                 G_CALLBACK(light_left), l);

	return actor;
}

void create_game(ClutterActor * stage)
{
	int x, y;
	
	for(x = 0; x < WIDTH; x++)
	{
		for(y = 0; y < HEIGHT; y++)
		{
			light * l = calloc(1, sizeof(light));
			grid[x][y] = l;

			l->x = x;
			l->y = y;
			l->color = rand() % 3;
			l->closed = FALSE;
			
			l->actor = create_light(l);
			
			clutter_container_add_actor(CLUTTER_CONTAINER(stage), l->actor);
		}
	}
}

int main()
{
	ClutterActor * stage;
	ClutterColor stage_color = { 0x00, 0x00, 0x00, 0xff };
	
	click_queue = g_queue_new();

	clutter_init(NULL, NULL);
	
	stage = clutter_stage_get_default();
	clutter_stage_set_color(CLUTTER_STAGE(stage), &stage_color);
	clutter_actor_set_size(stage, WIDTH * 50, HEIGHT * 50);
	
	create_game(stage);
	
	clutter_actor_show_all(stage);
	
	clutter_main();
}
