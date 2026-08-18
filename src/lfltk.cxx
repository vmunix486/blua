extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Progress.H>

/* FLTK box types */
#define LB_FL_UP_BOX        FL_UP_BOX
#define LB_FL_DOWN_BOX      FL_DOWN_BOX
#define LB_FL_FLAT_BOX      FL_FLAT_BOX
#define LB_FL_THIN_UP_BOX   FL_THIN_UP_BOX
#define LB_FL_THIN_DOWN_BOX FL_THIN_DOWN_BOX
#define LB_FL_ENGRAVED_BOX  FL_ENGRAVED_BOX
#define LB_FL_EMBOSSED_BOX  FL_EMBOSSED_BOX

/* FLTK font styles */
#define LB_FL_BOLD      FL_BOLD
#define LB_FL_ITALIC    FL_ITALIC
#define LB_FL_BOLD_ITALIC (FL_BOLD + FL_ITALIC)

/* FLTK label types */
#define LB_FL_NORMAL_LABEL   FL_NORMAL_LABEL
#define LB_FL_SHADOW_LABEL   FL_SHADOW_LABEL
#define LB_FL_ENGRAVED_LABEL FL_ENGRAVED_LABEL
#define LB_FL_EMBOSSED_LABEL FL_EMBOSSED_LABEL

/* Global Lua state for callback trampolines */
static lua_State *g_blua_L = NULL;

/* Callback: registry maps lightuserdata(widget ptr) -> Lua function */
static void simple_callback(Fl_Widget *w, void *) {
  if (!g_blua_L) return;
  lua_State *L = g_blua_L;
  lua_pushlightuserdata(L, w);
  lua_gettable(L, LUA_REGISTRYINDEX);
  if (lua_isfunction(L, -1)) {
    lua_pushlightuserdata(L, w);
    lua_call(L, 1, 0);
  } else {
    lua_pop(L, 1);
  }
}

/* Widget object stored as Lua userdata */
typedef struct {
  const char *tag;
  Fl_Widget *ptr;
} widget_userdata;

static widget_userdata *pushwidget(lua_State *L, const char *tag, Fl_Widget *w) {
  widget_userdata *ud = (widget_userdata *)lua_newuserdatauv(L, sizeof(widget_userdata), 0);
  luaL_getmetatable(L, tag);
  lua_setmetatable(L, -2);
  ud->tag = tag;
  ud->ptr = w;
  return ud;
}

static widget_userdata *checkwidget(lua_State *L, int idx) {
  static const char *types[] = {
    "Fl_Widget", "Fl_Window", "Fl_Box", "Fl_Button", "Fl_Input",
    "Fl_Output", "Fl_Slider", "Fl_Check_Button", "Fl_Choice",
    "Fl_Hold_Browser", "Fl_Spinner", "Fl_Progress", NULL
  };
  for (int i = 0; types[i]; i++) {
    widget_userdata *ud = (widget_userdata *)luaL_testudata(L, idx, types[i]);
    if (ud) return ud;
  }
  return (widget_userdata *)luaL_checkudata(L, idx, "Fl_Widget");
}

static widget_userdata *checkwidgettag(lua_State *L, int idx, const char *tag) {
  return (widget_userdata *)luaL_checkudata(L, idx, tag);
}

/*
 * Widget base methods
 */
static int l_widget_box(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 2) {
    ud->ptr->box((Fl_Boxtype)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, ud->ptr->box());
  return 1;
}

static int l_widget_label(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 2) {
    ud->ptr->label(luaL_checkstring(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  const char *s = ud->ptr->label();
  lua_pushstring(L, s ? s : "");
  return 1;
}

static int l_widget_labelsize(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 2) {
    ud->ptr->labelsize((Fl_Fontsize)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, ud->ptr->labelsize());
  return 1;
}

static int l_widget_labelfont(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 2) {
    ud->ptr->labelfont((Fl_Font)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, ud->ptr->labelfont());
  return 1;
}

static int l_widget_labeltype(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 2) {
    ud->ptr->labeltype((Fl_Labeltype)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, ud->ptr->labeltype());
  return 1;
}

static int l_widget_show(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  ud->ptr->show();
  return 0;
}

static int l_widget_hide(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  ud->ptr->hide();
  return 0;
}

static int l_widget_resize(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  ud->ptr->resize(
    (int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3),
    (int)luaL_checkinteger(L, 4), (int)luaL_checkinteger(L, 5));
  return 0;
}

static int l_widget_color(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 2) {
    ud->ptr->color((Fl_Color)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, ud->ptr->color());
  return 1;
}

static int l_widget_tooltip(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 2) {
    ud->ptr->tooltip(luaL_checkstring(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  const char *s = ud->ptr->tooltip();
  lua_pushstring(L, s ? s : "");
  return 1;
}

static int l_widget_callback(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  /* Store function in registry keyed by widget pointer */
  lua_pushlightuserdata(L, ud->ptr);
  lua_pushvalue(L, 2);
  lua_settable(L, LUA_REGISTRYINDEX);
  ud->ptr->callback(simple_callback, NULL);
  lua_settop(L, 1);
  return 1;
}

static int l_widget_position(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 3) {
    ud->ptr->position((int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, ud->ptr->x());
  lua_pushinteger(L, ud->ptr->y());
  return 2;
}

static int l_widget_w(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  lua_pushinteger(L, ud->ptr->w());
  return 1;
}

static int l_widget_h(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  lua_pushinteger(L, ud->ptr->h());
  return 1;
}

static int l_widget_visible(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  lua_pushboolean(L, ud->ptr->visible());
  return 1;
}

static int l_widget_active(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  lua_pushboolean(L, ud->ptr->active());
  return 1;
}

static int l_widget_activate(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  ud->ptr->activate();
  return 0;
}

static int l_widget_deactivate(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  ud->ptr->deactivate();
  return 0;
}

static int l_widget_redraw(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  ud->ptr->redraw();
  return 0;
}

static int l_widget_gc(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  ud->ptr = NULL;
  return 0;
}

/*
 * Window methods
 */
static int l_window_end(lua_State *L) {
  checkwidgettag(L, 1, "Fl_Window");
  Fl_Window *win = (Fl_Window *)checkwidget(L, 1)->ptr;
  win->end();
  return 0;
}

static int l_window_show(lua_State *L) {
  Fl_Window *win = (Fl_Window *)checkwidget(L, 1)->ptr;
  win->show();
  return 0;
}

static int l_window_make_current(lua_State *L) {
  Fl_Window *win = (Fl_Window *)checkwidget(L, 1)->ptr;
  win->make_current();
  return 0;
}

/*
 * Button-specific methods
 */
static int l_button_value(lua_State *L) {
  Fl_Button *btn = (Fl_Button *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    btn->value(lua_toboolean(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushboolean(L, btn->value());
  return 1;
}

/*
 * Input-specific methods
 */
static int l_input_value(lua_State *L) {
  Fl_Input *inp = (Fl_Input *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    inp->value(luaL_checkstring(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  const char *s = inp->value();
  lua_pushstring(L, s ? s : "");
  return 1;
}

static int l_input_insert(lua_State *L) {
  Fl_Input *inp = (Fl_Input *)checkwidget(L, 1)->ptr;
  const char *s = luaL_checkstring(L, 2);
  int pos = (int)luaL_optinteger(L, 3, inp->position());
  inp->insert(s, (int)strlen(s));
  lua_settop(L, 1);
  return 1;
}

static int l_input_position(lua_State *L) {
  Fl_Input *inp = (Fl_Input *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    inp->position((int)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, inp->position());
  return 1;
}

static int l_input_mark(lua_State *L) {
  Fl_Input *inp = (Fl_Input *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    inp->mark((int)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, inp->mark());
  return 1;
}

static int l_input_textsize(lua_State *L) {
  Fl_Input *inp = (Fl_Input *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    inp->textsize((Fl_Fontsize)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, inp->textsize());
  return 1;
}

static int l_input_textfont(lua_State *L) {
  Fl_Input *inp = (Fl_Input *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    inp->textfont((Fl_Font)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, inp->textfont());
  return 1;
}

static int l_input_textcolor(lua_State *L) {
  Fl_Input *inp = (Fl_Input *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    inp->textcolor((Fl_Color)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, inp->textcolor());
  return 1;
}

/*
 * Slider-specific methods
 */
static int l_slider_value(lua_State *L) {
  Fl_Slider *sld = (Fl_Slider *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    sld->value(luaL_checknumber(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, sld->value());
  return 1;
}

static int l_slider_bounds(lua_State *L) {
  Fl_Slider *sld = (Fl_Slider *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 3) {
    sld->bounds(luaL_checknumber(L, 2), luaL_checknumber(L, 3));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, sld->minimum());
  lua_pushnumber(L, sld->maximum());
  return 2;
}

static int l_slider_step(lua_State *L) {
  Fl_Slider *sld = (Fl_Slider *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    sld->step(luaL_checknumber(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, sld->step());
  return 1;
}

/*
 * Check_Button-specific methods
 */
static int l_check_button_value(lua_State *L) {
  Fl_Check_Button *btn = (Fl_Check_Button *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    btn->value(lua_toboolean(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushboolean(L, btn->value());
  return 1;
}

/*
 * Choice methods
 */
static int l_choice_add(lua_State *L) {
  Fl_Choice *ch = (Fl_Choice *)checkwidget(L, 1)->ptr;
  const char *label = luaL_checkstring(L, 2);
  lua_pushinteger(L, ch->add(label));
  return 1;
}

static int l_choice_value(lua_State *L) {
  Fl_Choice *ch = (Fl_Choice *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    ch->value((int)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, ch->value());
  return 1;
}

static int l_choice_text(lua_State *L) {
  Fl_Choice *ch = (Fl_Choice *)checkwidget(L, 1)->ptr;
  const char *s = ch->text();
  lua_pushstring(L, s ? s : "");
  return 1;
}

/*
 * Hold_Browser methods
 */
static int l_browser_add(lua_State *L) {
  Fl_Hold_Browser *br = (Fl_Hold_Browser *)checkwidget(L, 1)->ptr;
  br->add(luaL_checkstring(L, 2));
  return 0;
}

static int l_browser_clear(lua_State *L) {
  Fl_Hold_Browser *br = (Fl_Hold_Browser *)checkwidget(L, 1)->ptr;
  br->clear();
  return 0;
}

static int l_browser_value(lua_State *L) {
  Fl_Hold_Browser *br = (Fl_Hold_Browser *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    br->value((int)luaL_checkinteger(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, br->value());
  return 1;
}

static int l_browser_text(lua_State *L) {
  Fl_Hold_Browser *br = (Fl_Hold_Browser *)checkwidget(L, 1)->ptr;
  int n = (int)luaL_checkinteger(L, 2);
  const char *s = br->text(n);
  lua_pushstring(L, s ? s : "");
  return 1;
}

static int l_browser_size(lua_State *L) {
  Fl_Hold_Browser *br = (Fl_Hold_Browser *)checkwidget(L, 1)->ptr;
  lua_pushinteger(L, br->size());
  return 1;
}

/*
 * Spinner methods
 */
static int l_spinner_value(lua_State *L) {
  Fl_Spinner *sp = (Fl_Spinner *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    sp->value(luaL_checknumber(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, sp->value());
  return 1;
}

static int l_spinner_step(lua_State *L) {
  Fl_Spinner *sp = (Fl_Spinner *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    sp->step(luaL_checknumber(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, sp->step());
  return 1;
}

static int l_spinner_min(lua_State *L) {
  Fl_Spinner *sp = (Fl_Spinner *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    sp->minimum(luaL_checknumber(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, sp->minimum());
  return 1;
}

static int l_spinner_max(lua_State *L) {
  Fl_Spinner *sp = (Fl_Spinner *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    sp->maximum(luaL_checknumber(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, sp->maximum());
  return 1;
}

/*
 * Progress methods
 */
static int l_progress_value(lua_State *L) {
  Fl_Progress *pr = (Fl_Progress *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    pr->value((float)luaL_checknumber(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, (double)pr->value());
  return 1;
}

static int l_progress_minimum(lua_State *L) {
  Fl_Progress *pr = (Fl_Progress *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    pr->minimum((float)luaL_checknumber(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, (double)pr->minimum());
  return 1;
}

static int l_progress_maximum(lua_State *L) {
  Fl_Progress *pr = (Fl_Progress *)checkwidget(L, 1)->ptr;
  if (lua_gettop(L) >= 2) {
    pr->maximum((float)luaL_checknumber(L, 2));
    lua_settop(L, 1);
    return 1;
  }
  lua_pushnumber(L, (double)pr->maximum());
  return 1;
}

/*
 * Constructor functions
 */
static int l_new_window(lua_State *L) {
  int w = (int)luaL_checkinteger(L, 1);
  int h = (int)luaL_checkinteger(L, 2);
  const char *title = lua_isstring(L, 3) ? lua_tostring(L, 3) : "";
  Fl_Window *win = new Fl_Window(w, h, title);
  pushwidget(L, "Fl_Window", (Fl_Widget *)win);
  return 1;
}

static int l_new_box(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Box *box = new Fl_Box(x, y, w, h, label);
  pushwidget(L, "Fl_Box", (Fl_Widget *)box);
  return 1;
}

static int l_new_button(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Button *btn = new Fl_Button(x, y, w, h, label);
  pushwidget(L, "Fl_Button", (Fl_Widget *)btn);
  return 1;
}

static int l_new_input(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Input *inp = new Fl_Input(x, y, w, h, label);
  pushwidget(L, "Fl_Input", (Fl_Widget *)inp);
  return 1;
}

static int l_new_output(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Output *out = new Fl_Output(x, y, w, h, label);
  pushwidget(L, "Fl_Output", (Fl_Widget *)out);
  return 1;
}

static int l_new_slider(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Slider *sld = new Fl_Slider(x, y, w, h, label);
  pushwidget(L, "Fl_Slider", (Fl_Widget *)sld);
  return 1;
}

static int l_new_check_button(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Check_Button *btn = new Fl_Check_Button(x, y, w, h, label);
  pushwidget(L, "Fl_Check_Button", (Fl_Widget *)btn);
  return 1;
}

static int l_new_choice(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Choice *ch = new Fl_Choice(x, y, w, h, label);
  pushwidget(L, "Fl_Choice", (Fl_Widget *)ch);
  return 1;
}

static int l_new_hold_browser(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Hold_Browser *br = new Fl_Hold_Browser(x, y, w, h, label);
  pushwidget(L, "Fl_Hold_Browser", (Fl_Widget *)br);
  return 1;
}

static int l_new_spinner(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Spinner *sp = new Fl_Spinner(x, y, w, h, label);
  pushwidget(L, "Fl_Spinner", (Fl_Widget *)sp);
  return 1;
}

static int l_new_progress(lua_State *L) {
  int x = (int)luaL_checkinteger(L, 1);
  int y = (int)luaL_checkinteger(L, 2);
  int w = (int)luaL_checkinteger(L, 3);
  int h = (int)luaL_checkinteger(L, 4);
  const char *label = lua_isstring(L, 5) ? lua_tostring(L, 5) : "";
  Fl_Progress *pr = new Fl_Progress(x, y, w, h, label);
  pushwidget(L, "Fl_Progress", (Fl_Widget *)pr);
  return 1;
}

/*
 * Fl:: utility functions
 */
static int l_fl_run(lua_State *L) {
  lua_pushinteger(L, Fl::run());
  return 1;
}

static int l_fl_wait(lua_State *L) {
  double t = Fl::wait();
  lua_pushnumber(L, t);
  return 1;
}

static int l_fl_scheme(lua_State *L) {
  if (lua_gettop(L) >= 1) {
    const char *s = luaL_checkstring(L, 1);
    lua_pushboolean(L, Fl::scheme(s));
    return 1;
  }
  const char *s = Fl::scheme();
  lua_pushstring(L, s ? s : "");
  return 1;
}

static int l_fl_version(lua_State *L) {
  lua_pushnumber(L, Fl::version());
  return 1;
}

static int l_fl_api_version(lua_State *L) {
  lua_pushinteger(L, Fl::api_version());
  return 1;
}

static int l_fl_redraw(lua_State *L) {
  Fl::redraw();
  return 0;
}

static int l_fl_check(lua_State *L) {
  lua_pushinteger(L, Fl::check());
  return 1;
}

static int l_fl_option(lua_State *L) {
  if (lua_gettop(L) >= 2) {
    int opt = (int)luaL_checkinteger(L, 1);
    int val = lua_toboolean(L, 2);
    Fl::option((Fl::Fl_Option)opt, val);
    lua_settop(L, 1);
    return 1;
  }
  int opt = (int)luaL_checkinteger(L, 1);
  lua_pushboolean(L, Fl::option((Fl::Fl_Option)opt));
  return 1;
}

static int l_fl_display(lua_State *L) {
  const char *s = luaL_checkstring(L, 1);
  Fl::display(s);
  return 0;
}

static int l_fl_focus(lua_State *L) {
  if (lua_gettop(L) >= 1) {
    widget_userdata *ud = checkwidget(L, 1);
    Fl::focus(ud->ptr);
    lua_settop(L, 1);
    return 1;
  }
  Fl_Widget *w = Fl::focus();
  if (w) {
    /* Try to find the userdata - for now just push lightuserdata */
    lua_pushlightuserdata(L, w);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int l_fl_grab(lua_State *L) {
  if (lua_gettop(L) >= 1) {
    Fl_Window *win = (Fl_Window *)checkwidget(L, 1)->ptr;
    Fl::grab(win);
    lua_settop(L, 1);
    return 1;
  }
  Fl_Window *w = Fl::grab();
  if (w) lua_pushlightuserdata(L, w);
  else lua_pushnil(L);
  return 1;
}

static int l_fl_release_grab(lua_State *L) {
  Fl::release();
  return 0;
}

static int l_fl_screen_count(lua_State *L) {
  lua_pushinteger(L, Fl::screen_count());
  return 1;
}

static int l_fl_screen_xywh(lua_State *L) {
  int num = (int)luaL_optinteger(L, 1, 0);
  int X, Y, W, H;
  Fl::screen_xywh(X, Y, W, H, num);
  lua_pushinteger(L, X);
  lua_pushinteger(L, Y);
  lua_pushinteger(L, W);
  lua_pushinteger(L, H);
  return 4;
}

/*
 * Module definition - constructors
 */
static const struct luaL_Reg fltk_funcs[] = {
  {"Fl_Window",      l_new_window},
  {"Fl_Box",         l_new_box},
  {"Fl_Button",      l_new_button},
  {"Fl_Input",       l_new_input},
  {"Fl_Output",      l_new_output},
  {"Fl_Slider",      l_new_slider},
  {"Fl_Check_Button", l_new_check_button},
  {"Fl_Choice",      l_new_choice},
  {"Fl_Hold_Browser", l_new_hold_browser},
  {"Fl_Spinner",     l_new_spinner},
  {"Fl_Progress",    l_new_progress},
  {NULL, NULL}
};

/*
 * Widget base methods (inherited by all widget types)
 */
static const struct luaL_Reg fl_widget_methods[] = {
  {"box",         l_widget_box},
  {"label",       l_widget_label},
  {"labelsize",   l_widget_labelsize},
  {"labelfont",   l_widget_labelfont},
  {"labeltype",   l_widget_labeltype},
  {"color",       l_widget_color},
  {"tooltip",     l_widget_tooltip},
  {"callback",    l_widget_callback},
  {"position",    l_widget_position},
  {"w",           l_widget_w},
  {"h",           l_widget_h},
  {"show",        l_widget_show},
  {"hide",        l_widget_hide},
  {"resize",      l_widget_resize},
  {"visible",     l_widget_visible},
  {"active",      l_widget_active},
  {"activate",    l_widget_activate},
  {"deactivate",  l_widget_deactivate},
  {"redraw",      l_widget_redraw},
  {"__gc",        l_widget_gc},
  {NULL, NULL}
};

static const struct luaL_Reg fl_window_methods[] = {
  {"finish",         l_window_end},
  {"show",           l_window_show},
  {"make_current",   l_window_make_current},
  {NULL, NULL}
};

static const struct luaL_Reg fl_button_methods[] = {
  {"value", l_button_value},
  {NULL, NULL}
};

static const struct luaL_Reg fl_input_methods[] = {
  {"value",     l_input_value},
  {"insert",    l_input_insert},
  {"position",  l_input_position},
  {"mark",      l_input_mark},
  {"textsize",  l_input_textsize},
  {"textfont",  l_input_textfont},
  {"textcolor", l_input_textcolor},
  {NULL, NULL}
};

static const struct luaL_Reg fl_slider_methods[] = {
  {"value",  l_slider_value},
  {"bounds", l_slider_bounds},
  {"step",   l_slider_step},
  {NULL, NULL}
};

static const struct luaL_Reg fl_check_button_methods[] = {
  {"value", l_check_button_value},
  {NULL, NULL}
};

static const struct luaL_Reg fl_choice_methods[] = {
  {"add",   l_choice_add},
  {"value", l_choice_value},
  {"text",  l_choice_text},
  {NULL, NULL}
};

static const struct luaL_Reg fl_hold_browser_methods[] = {
  {"add",   l_browser_add},
  {"clear", l_browser_clear},
  {"value", l_browser_value},
  {"text",  l_browser_text},
  {"size",  l_browser_size},
  {NULL, NULL}
};

static const struct luaL_Reg fl_spinner_methods[] = {
  {"value", l_spinner_value},
  {"step",  l_spinner_step},
  {"min",   l_spinner_min},
  {"max",   l_spinner_max},
  {NULL, NULL}
};

static const struct luaL_Reg fl_progress_methods[] = {
  {"value",   l_progress_value},
  {"minimum", l_progress_minimum},
  {"maximum", l_progress_maximum},
  {NULL, NULL}
};

static void create_metatable(lua_State *L, const char *name, const luaL_Reg *methods) {
  luaL_newmetatable(L, name);
  lua_newtable(L);
  luaL_setfuncs(L, methods, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

/* Helper: create a metatable that inherits from Fl_Widget + adds extra methods */
static void create_widget_metatable(lua_State *L, const char *name, const luaL_Reg *extra) {
  luaL_newmetatable(L, name);
  lua_newtable(L);
  /* First copy all base widget methods */
  luaL_setfuncs(L, fl_widget_methods, 0);
  /* Then add widget-specific methods (these override) */
  if (extra) {
    for (const luaL_Reg *m = extra; m->name; m++) {
      lua_pushcfunction(L, m->func);
      lua_setfield(L, -2, m->name);
    }
  }
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

extern "C" int luaopen_fltk(lua_State *L) {
  g_blua_L = L;

  /* Create base widget metatable */
  create_metatable(L, "Fl_Widget", fl_widget_methods);

  /* Fl_Window: inherits from Fl_Widget */
  luaL_newmetatable(L, "Fl_Window");
  lua_newtable(L);
  luaL_setfuncs(L, fl_widget_methods, 0);
  for (const luaL_Reg *m = fl_window_methods; m->name; m++) {
    lua_pushcfunction(L, m->func);
    lua_setfield(L, -2, m->name);
  }
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  /* Fl_Box: inherits from Fl_Widget */
  create_widget_metatable(L, "Fl_Box", NULL);

  /* Fl_Button: inherits from Fl_Widget */
  create_widget_metatable(L, "Fl_Button", fl_button_methods);

  /* Fl_Input: inherits from Fl_Widget */
  create_widget_metatable(L, "Fl_Input", fl_input_methods);

  /* Fl_Output: inherits from Fl_Widget (output uses input methods for value/textsize etc) */
  create_widget_metatable(L, "Fl_Output", fl_input_methods);

  /* Fl_Slider: inherits from Fl_Widget */
  create_widget_metatable(L, "Fl_Slider", fl_slider_methods);

  /* Fl_Check_Button: inherits from Fl_Widget */
  create_widget_metatable(L, "Fl_Check_Button", fl_check_button_methods);

  /* Fl_Choice: inherits from Fl_Widget */
  create_widget_metatable(L, "Fl_Choice", fl_choice_methods);

  /* Fl_Hold_Browser: inherits from Fl_Widget */
  create_widget_metatable(L, "Fl_Hold_Browser", fl_hold_browser_methods);

  /* Fl_Spinner: inherits from Fl_Widget */
  create_widget_metatable(L, "Fl_Spinner", fl_spinner_methods);

  /* Fl_Progress: inherits from Fl_Widget */
  create_widget_metatable(L, "Fl_Progress", fl_progress_methods);

  /* Create the fltk module table */
  lua_newtable(L);
  luaL_setfuncs(L, fltk_funcs, 0);

  /* Fl:: utility functions */
  lua_pushcfunction(L, l_fl_run);
  lua_setfield(L, -2, "run");
  lua_pushcfunction(L, l_fl_wait);
  lua_setfield(L, -2, "wait");
  lua_pushcfunction(L, l_fl_scheme);
  lua_setfield(L, -2, "scheme");
  lua_pushcfunction(L, l_fl_version);
  lua_setfield(L, -2, "version");
  lua_pushcfunction(L, l_fl_api_version);
  lua_setfield(L, -2, "api_version");
  lua_pushcfunction(L, l_fl_redraw);
  lua_setfield(L, -2, "redraw");
  lua_pushcfunction(L, l_fl_check);
  lua_setfield(L, -2, "check");
  lua_pushcfunction(L, l_fl_option);
  lua_setfield(L, -2, "option");
  lua_pushcfunction(L, l_fl_display);
  lua_setfield(L, -2, "display");
  lua_pushcfunction(L, l_fl_focus);
  lua_setfield(L, -2, "focus");
  lua_pushcfunction(L, l_fl_grab);
  lua_setfield(L, -2, "grab");
  lua_pushcfunction(L, l_fl_release_grab);
  lua_setfield(L, -2, "release_grab");
  lua_pushcfunction(L, l_fl_screen_count);
  lua_setfield(L, -2, "screen_count");
  lua_pushcfunction(L, l_fl_screen_xywh);
  lua_setfield(L, -2, "screen_xywh");

  /* Box type constants (global) */
  lua_pushinteger(L, LB_FL_UP_BOX);
  lua_setglobal(L, "FL_UP_BOX");
  lua_pushinteger(L, LB_FL_DOWN_BOX);
  lua_setglobal(L, "FL_DOWN_BOX");
  lua_pushinteger(L, LB_FL_FLAT_BOX);
  lua_setglobal(L, "FL_FLAT_BOX");
  lua_pushinteger(L, LB_FL_THIN_UP_BOX);
  lua_setglobal(L, "FL_THIN_UP_BOX");
  lua_pushinteger(L, LB_FL_THIN_DOWN_BOX);
  lua_setglobal(L, "FL_THIN_DOWN_BOX");
  lua_pushinteger(L, LB_FL_ENGRAVED_BOX);
  lua_setglobal(L, "FL_ENGRAVED_BOX");
  lua_pushinteger(L, LB_FL_EMBOSSED_BOX);
  lua_setglobal(L, "FL_EMBOSSED_BOX");

  /* Font style constants (global) */
  lua_pushinteger(L, LB_FL_BOLD);
  lua_setglobal(L, "FL_BOLD");
  lua_pushinteger(L, LB_FL_ITALIC);
  lua_setglobal(L, "FL_ITALIC");
  lua_pushinteger(L, LB_FL_BOLD_ITALIC);
  lua_setglobal(L, "FL_BOLD_ITALIC");

  /* Label type constants (global) */
  lua_pushinteger(L, LB_FL_NORMAL_LABEL);
  lua_setglobal(L, "FL_NORMAL_LABEL");
  lua_pushinteger(L, LB_FL_SHADOW_LABEL);
  lua_setglobal(L, "FL_SHADOW_LABEL");
  lua_pushinteger(L, LB_FL_ENGRAVED_LABEL);
  lua_setglobal(L, "FL_ENGRAVED_LABEL");
  lua_pushinteger(L, LB_FL_EMBOSSED_LABEL);
  lua_setglobal(L, "FL_EMBOSSED_LABEL");

  /* Color constants (global) */
  lua_pushinteger(L, FL_BLACK);
  lua_setglobal(L, "FL_BLACK");
  lua_pushinteger(L, FL_RED);
  lua_setglobal(L, "FL_RED");
  lua_pushinteger(L, FL_GREEN);
  lua_setglobal(L, "FL_GREEN");
  lua_pushinteger(L, FL_YELLOW);
  lua_setglobal(L, "FL_YELLOW");
  lua_pushinteger(L, FL_BLUE);
  lua_setglobal(L, "FL_BLUE");
  lua_pushinteger(L, FL_MAGENTA);
  lua_setglobal(L, "FL_MAGENTA");
  lua_pushinteger(L, FL_CYAN);
  lua_setglobal(L, "FL_CYAN");
  lua_pushinteger(L, FL_WHITE);
  lua_setglobal(L, "FL_WHITE");
  lua_pushinteger(L, FL_FOREGROUND_COLOR);
  lua_setglobal(L, "FL_FOREGROUND_COLOR");
  lua_pushinteger(L, FL_BACKGROUND_COLOR);
  lua_setglobal(L, "FL_BACKGROUND_COLOR");
  lua_pushinteger(L, FL_INACTIVE_COLOR);
  lua_setglobal(L, "FL_INACTIVE_COLOR");
  lua_pushinteger(L, FL_SELECTION_COLOR);
  lua_setglobal(L, "FL_SELECTION_COLOR");

  /* Alignment constants (global) */
  lua_pushinteger(L, FL_ALIGN_CENTER);
  lua_setglobal(L, "FL_ALIGN_CENTER");
  lua_pushinteger(L, FL_ALIGN_TOP);
  lua_setglobal(L, "FL_ALIGN_TOP");
  lua_pushinteger(L, FL_ALIGN_BOTTOM);
  lua_setglobal(L, "FL_ALIGN_BOTTOM");
  lua_pushinteger(L, FL_ALIGN_LEFT);
  lua_setglobal(L, "FL_ALIGN_LEFT");
  lua_pushinteger(L, FL_ALIGN_RIGHT);
  lua_setglobal(L, "FL_ALIGN_RIGHT");
  lua_pushinteger(L, FL_ALIGN_INSIDE);
  lua_setglobal(L, "FL_ALIGN_INSIDE");
  lua_pushinteger(L, FL_ALIGN_TEXT_OVER_IMAGE);
  lua_setglobal(L, "FL_ALIGN_TEXT_OVER_IMAGE");
  lua_pushinteger(L, FL_ALIGN_IMAGE_BACKDROP);
  lua_setglobal(L, "FL_ALIGN_IMAGE_BACKDROP");
  lua_pushinteger(L, FL_ALIGN_CLIP);
  lua_setglobal(L, "FL_ALIGN_CLIP");
  lua_pushinteger(L, FL_ALIGN_WRAP);
  lua_setglobal(L, "FL_ALIGN_WRAP");

  /* When constants (for callback triggering) */
  lua_pushinteger(L, FL_WHEN_NEVER);
  lua_setglobal(L, "FL_WHEN_NEVER");
  lua_pushinteger(L, FL_WHEN_CHANGED);
  lua_setglobal(L, "FL_WHEN_CHANGED");
  lua_pushinteger(L, FL_WHEN_NOT_CHANGED);
  lua_setglobal(L, "FL_WHEN_NOT_CHANGED");
  lua_pushinteger(L, FL_WHEN_RELEASE);
  lua_setglobal(L, "FL_WHEN_RELEASE");
  lua_pushinteger(L, FL_WHEN_RELEASE_ALWAYS);
  lua_setglobal(L, "FL_WHEN_RELEASE_ALWAYS");
  lua_pushinteger(L, FL_WHEN_ENTER_KEY);
  lua_setglobal(L, "FL_WHEN_ENTER_KEY");
  lua_pushinteger(L, FL_WHEN_ENTER_KEY_ALWAYS);
  lua_setglobal(L, "FL_WHEN_ENTER_KEY_ALWAYS");

  /* Set fltk as a global */
  lua_setglobal(L, "fltk");

  return 0;
}
