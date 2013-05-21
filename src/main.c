
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xEE, 0xC2, 0x13, 0xF7, 0x50, 0x71, 0x49, 0xC8, 0xB5, 0xF4, 0x54, 0xC8, 0x4C, 0x3D, 0xC1, 0xA6 }
PBL_APP_INFO(MY_UUID, "Tea Timer", "Clockwork Coding", 0x0, 0x2, RESOURCE_ID_IMAGE_MENU_ICON_WHITE, APP_INFO_STANDARD_APP);



Window window;

TextLayer timeLayer; //Timer
TextLayer temperatureLayer;
TextLayer teaModeLayer;


AppTimerHandle timer_handle;

#define TEA_MODE_BLACK		0
#define TEA_MODE_WHITE		1
#define TEA_MODE_GREEN		2
#define TEA_MODE_MATE		3
#define TEA_MODE_ROOIBOS	4
#define TEA_MODE_HERBAL		5
#define TEA_MODE_OOLONG		6

Window window;

TextLayer textLayer;


bool timerRunning = false;
bool hidden = false;

short teaMode = TEA_MODE_BLACK;

int seconds = 120;
static char times[600][6];

void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
  (void)ctx;
  (void)handle;

  hidden = false;
  layer_set_hidden(&timeLayer.layer, hidden);
}


// Called once per second
void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) 
{

  (void)ctx;
  (void)t;
	
  if (timerRunning)
  {
    seconds--;
    text_layer_set_text(&timeLayer, times[seconds]);
  }

  if (seconds <= 0)
  {
    vibes_double_pulse();
    seconds = 0;
    text_layer_set_text(&timeLayer, times[seconds]);
    hidden = true;
    layer_set_hidden(&timeLayer.layer, hidden);
    timer_handle = app_timer_send_event(ctx, 500 , 1);
  } 

}

void updateTeaMode()
{
  switch(teaMode) 
  {
    case TEA_MODE_BLACK:
      text_layer_set_text(&teaModeLayer, "Black");
      text_layer_set_text(&temperatureLayer, "205°");
      seconds = 120;
      break;
    case TEA_MODE_GREEN:
      text_layer_set_text(&teaModeLayer, "Green");
      text_layer_set_text(&temperatureLayer, "175°");
      seconds = 30;
      break;
    case TEA_MODE_WHITE:
      text_layer_set_text(&teaModeLayer, "White");
      text_layer_set_text(&temperatureLayer, "175°");
      seconds = 120;
      break;
    case TEA_MODE_MATE:
      text_layer_set_text(&teaModeLayer, "Maté");
      text_layer_set_text(&temperatureLayer, "208°");
      seconds = 300;
      break;
    case TEA_MODE_ROOIBOS:
      text_layer_set_text(&teaModeLayer, "Rooibos");
      text_layer_set_text(&temperatureLayer, "208°");
      seconds = 300;
      break;
    case TEA_MODE_HERBAL:
      text_layer_set_text(&teaModeLayer, "Herbal");
      text_layer_set_text(&temperatureLayer, "208°");
      seconds = 300;
      break;
    case TEA_MODE_OOLONG:
      text_layer_set_text(&teaModeLayer, "Oolong");
      text_layer_set_text(&temperatureLayer, "195°");
      seconds = 180;
      break;
    default:
      teaMode = 0;
      updateTeaMode();
      break;
  }
  hidden = false;
  layer_set_hidden(&timeLayer.layer, hidden);
  text_layer_set_text(&timeLayer, times[seconds]);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  timerRunning = !timerRunning;
  if(!timerRunning)
  {
    updateTeaMode();
  }
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  
  if(!timerRunning)
  {
    teaMode ++;
    updateTeaMode();
  }
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  seconds += 15;
  if (seconds >= 600)
  {
    seconds = 599;
  }
  hidden = false;
  layer_set_hidden(&timeLayer.layer, hidden);
  text_layer_set_text(&timeLayer, times[seconds]);
}


void config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;


  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

void populateTimeArray()
{
  int r = 0;
  int h = 0;
  for(int i = 0; i < 600; i++)
  {
    r = i%60;
    h = (int) i/60;
    if ( i > 60)
      itoa(h,(char*)&times[i][0]);
    else
      times[i][0] = '0';


    times[i][1] = ':';


    if (r < 10)
    {
      times[i][2] = '0';
      itoa(r,(char*)&times[i][3]);
    }
    else
    {
      itoa(r,(char*)&times[i][2] );
    }
  }
}

// Handle the start-up of the app
void handle_init(AppContextRef app_ctx) {
  populateTimeArray();

  // Create our app's base window
  window_init(&window, "Tea Timer");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
	
  
  resource_init_current_app(&APP_RESOURCES);

  // Init the text layer used to show the Temperature
  text_layer_init(&temperatureLayer, GRect(4, 104, 144-4, 40));
  text_layer_set_text_color(&temperatureLayer, GColorWhite);
  text_layer_set_background_color(&temperatureLayer, GColorClear);
  text_layer_set_font(&temperatureLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));

  // Init the text layer used to show the Tea Mode
  text_layer_init(&teaModeLayer, GRect(4, 4, 144-4, 40));
  text_layer_set_text_color(&teaModeLayer, GColorWhite);
  text_layer_set_background_color(&teaModeLayer, GColorClear);
  text_layer_set_font(&teaModeLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));

  // Init the text layer used to show the time left
  text_layer_init(&timeLayer, GRect( 4, 44, 144-4, 60));
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_second_tick(app_ctx, NULL);

  updateTeaMode();

  layer_add_child(&window.layer, &timeLayer.layer);
  layer_add_child(&window.layer, &temperatureLayer.layer);
  layer_add_child(&window.layer, &teaModeLayer.layer);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) config_provider);
}

// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;


  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}



void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init,
      .timer_handler = &handle_timer,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}