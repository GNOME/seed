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

gboolean light_clicked(ClutterActor * actor,
                       ClutterEvent * event,
                       gpointer user_data)
{
	light * l = (light *)user_data;
	GList * cl = connected_lights(l);

	if(g_list_length(cl) < 2)
		return FALSE;

	GList * li;
	ClutterTimeline * out_timeline = clutter_timeline_new(500);
	
	for(li = cl; li != NULL; li = g_list_next(li))
	{
		light * inner_l = li->data;
		//clutter_actor_set_opacity(inner_l->actor, 128);
		clutter_actor_animate_with_timeline(inner_l->actor,
		                                    CLUTTER_EASE_OUT_EXPO,
		                                    out_timeline,
		                                    "opacity",
		                                    0, NULL);
		inner_l->closed = TRUE;
	}
	
	int vx, vy;
	
	for(vx = 0; vx < WIDTH; vx++)
	{
		GList * new_col = NULL;
		
		for(vy = 0; vy < HEIGHT; vy++)
		{
			if(grid[vx][vy] && !grid[vx][vy]->closed)
				new_col = g_list_append(new_col, grid[vx][vy]);
		}
		
		GList * new_col_iter;
		int new_y = 0;
		
		for(new_col_iter = new_col;
		    new_col_iter != NULL;
		    new_col_iter = g_list_next(new_col_iter))
		{
			light * l = (light *)new_col_iter->data;
			l->y = new_y;
			clutter_actor_set_y(l->actor, ((HEIGHT - 1) * 50) - (new_y * 50));
			grid[vx][new_y] = l;
			new_y++;
		}
		
		for(; new_y < HEIGHT; new_y++)
		{
			grid[vx][new_y] = NULL;
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

	clutter_init(NULL, NULL);
	
	stage = clutter_stage_get_default();
	clutter_stage_set_color(CLUTTER_STAGE(stage), &stage_color);
	clutter_actor_set_size(stage, WIDTH * 50, HEIGHT * 50);
	
	create_game(stage);
	
	clutter_actor_show_all(stage);
	
	clutter_main();
}
