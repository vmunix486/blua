extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

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

/* Widget object stored as Lua userdata */
typedef struct {
  const char *tag;  /* "Fl_Widget", "Fl_Window", "Fl_Box", etc. */
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
  /* Check for any known widget type */
  static const char *types[] = {"Fl_Widget", "Fl_Window", "Fl_Box", NULL};
  for (int i = 0; types[i]; i++) {
    widget_userdata *ud = (widget_userdata *)luaL_testudata(L, idx, types[i]);
    if (ud) return ud;
  }
  return (widget_userdata *)luaL_checkudata(L, idx, "Fl_Widget");
}

static widget_userdata *checkwidgettag(lua_State *L, int idx, const char *tag) {
  widget_userdata *ud = (widget_userdata *)luaL_checkudata(L, idx, tag);
  return ud;
}

/*
 * Widget base methods
 */
static int l_widget_box(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 2) {
    int type = (int)luaL_checkinteger(L, 2);
    ud->ptr->box((Fl_Boxtype)type);
    lua_settop(L, 1);
    return 1;
  }
  lua_pushinteger(L, ud->ptr->box());
  return 1;
}

static int l_widget_label(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  if (lua_gettop(L) >= 2) {
    const char *s = luaL_checkstring(L, 2);
    ud->ptr->label(s);
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
    int type = (int)luaL_checkinteger(L, 2);
    ud->ptr->labeltype((Fl_Labeltype)type);
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
  int x = (int)luaL_checkinteger(L, 2);
  int y = (int)luaL_checkinteger(L, 3);
  int w = (int)luaL_checkinteger(L, 4);
  int h = (int)luaL_checkinteger(L, 5);
  ud->ptr->resize(x, y, w, h);
  return 0;
}

static int l_widget_gc(lua_State *L) {
  widget_userdata *ud = checkwidget(L, 1);
  /* FL widgets are owned by their parent or the window;
     Lua should not delete them. Just NULL the pointer. */
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
 * Constructor functions (called on the module table)
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

/*
 * Fl::run()
 */
static int l_fl_run(lua_State *L) {
  lua_pushinteger(L, Fl::run());
  return 1;
}

/*
 * Module definition
 */
static const struct luaL_Reg fltk_funcs[] = {
  {"Fl_Window", l_new_window},
  {"Fl_Box",    l_new_box},
  {NULL, NULL}
};

static const struct luaL_Reg fl_widget_methods[] = {
  {"box",       l_widget_box},
  {"label",     l_widget_label},
  {"labelsize", l_widget_labelsize},
  {"labelfont", l_widget_labelfont},
  {"labeltype", l_widget_labeltype},
  {"show",      l_widget_show},
  {"hide",      l_widget_hide},
  {"resize",    l_widget_resize},
  {"__gc",      l_widget_gc},
  {NULL, NULL}
};

static const struct luaL_Reg fl_window_methods[] = {
  {"finish",         l_window_end},
  {"show",           l_window_show},
  {"make_current",   l_window_make_current},
  {NULL, NULL}
};

static void create_metatable(lua_State *L, const char *name, const luaL_Reg *methods) {
  luaL_newmetatable(L, name);
  lua_newtable(L);  /* metatable for methods */
  luaL_setfuncs(L, methods, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}

extern "C" int luaopen_fltk(lua_State *L) {
  /* Create metatables for widget types */
  create_metatable(L, "Fl_Widget", fl_widget_methods);

  /* Fl_Window: inherits from Fl_Widget, adds window methods */
  luaL_newmetatable(L, "Fl_Window");
  lua_newtable(L);
  /* Copy Fl_Widget methods into Fl_Window metatable */
  luaL_setfuncs(L, fl_widget_methods, 0);
  /* Add Fl_Window-specific methods (these override widget methods) */
  for (const luaL_Reg *m = fl_window_methods; m->name; m++) {
    lua_pushcfunction(L, m->func);
    lua_setfield(L, -2, m->name);
  }
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  /* Fl_Box: inherits from Fl_Widget */
  luaL_newmetatable(L, "Fl_Box");
  lua_newtable(L);
  luaL_setfuncs(L, fl_widget_methods, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  /* Create the fltk module table */
  lua_newtable(L);
  luaL_setfuncs(L, fltk_funcs, 0);

  /* Fl.run */
  lua_pushcfunction(L, l_fl_run);
  lua_setfield(L, -2, "run");

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

  /* Set fltk as a global */
  lua_setglobal(L, "fltk");

  return 0;
}
