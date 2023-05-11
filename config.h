#include <X11/XF86keysym.h>
#include "movestack.c"
#include "horizgrid.c"

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx = 6;
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;     /* 0 means no bar */
// static const int topbar             = 1;     /* 0 means bottom bar */
static const int topbar             = 0;     /* 0 means bottom bar */
static const char *fonts[]          = { "JetBrains Mono:size=11", "JoyPixels:pixelsize=11:antialias=true:autohint=true" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
/* static const char col_gray4[]       = "#eeeeee"; */
static const char col_gray4[]       = "#924441";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray3, col_gray4,  col_gray4  },
	/* [SchemeSel]  = { col_gray4, col_cyan,  col_cyan  }, */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* class      instance    title       tags mask     isfloating   monitor    scratch key */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1,        0  },
	{ "firefox",  NULL,       NULL,       1 << 8,       0,           -1,        0  },
	{ "spterm",       NULL,   "scratchpad",   0,            1,           -1,       's' },
	{ NULL,       NULL,   "ranger",   0,            1,           -1,       'r' }
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "###",      horizgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG)												\
	{1, {{MODKEY, KEY}},								view,           {.ui = 1 << TAG} },	\
	{1, {{MODKEY|ControlMask, KEY}},					toggleview,     {.ui = 1 << TAG} }, \
	{1, {{MODKEY|ShiftMask, KEY}},						tag,            {.ui = 1 << TAG} }, \
	{1, {{MODKEY|ControlMask|ShiftMask, KEY}},			toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "kitty", NULL };
static const char *rofi[] = {"rofi", "-show", "drun", NULL};
static const char *newsboat[] = {"kitty", "-e", "newsboat", NULL};

// static const char *scratchpadcmd[] = {"s+", "st", "-t", "scratchpad", NULL}; 
const char *scratchpadcmd[] = {"s", "kitty", "--title", "scratchpad", "--class", "spterm", "-o", "window.dimensions.columns=120", "-o", "window.dimensions.lines=34", "-e", "cmus", NULL };
const char *scratchpadcmd1[] = {"r", "kitty", "--title", "ranger", "-o", "window.dimensions.columns=120", "-o", "window.dimensions.lines=34", "-e", "ranger", NULL };

static Keychord keychords[] = {
	/* Keys        function        argument */
	// Menus
	{1, {{MODKEY|ShiftMask, XK_Return}},			spawn,          {.v = rofi } },
	{2, {{MODKEY, XK_p}, {0, XK_q}},			   spawn,		SHCMD("powermenu")},
	{2, {{MODKEY, XK_p}, {0, XK_p}},			   spawn,		SHCMD("passmenu")},
	/* {2, {{MODKEY, XK_b}, {0, XK_1}},							spawn,          {.v = chrome0 } }, */
	/* {2, {{MODKEY, XK_b}, {0, XK_2}},							spawn,          SHCMD("google-chrome-stable --profile-directory='Profile 1'") }, */
	/* {2, {{MODKEY, XK_b}, {0, XK_3}},							spawn,          SHCMD("google-chrome-stable --profile-directory='Profile 2'") }, */
	//	Apps
	{1, {{MODKEY, XK_Return}},						spawn,          {.v = termcmd} },
	{1, {{MODKEY, XK_n}}, 							spawn,          {.v = newsboat } },
	{1, {{MODKEY, XK_a}},							spawn,          SHCMD("QTWEBENGINE_CHROMIUM_FLAGS='--disable-logging' anki")},
	{2, {{MODKEY, XK_e}, {0, XK_f}},			spawn,          SHCMD("thunar") },
  {1, {{0, 0x0000ff61}},                               spawn,      SHCMD("flameshot gui") },
	/// Workspaces
	// Stack
	{1, {{MODKEY, XK_j}},							focusstack,     {.i = +1 } },
	{1, {{MODKEY, XK_k}},							focusstack,     {.i = -1 } },
	{1, {{MODKEY|ShiftMask, XK_h}},				    rotatestack,    {.i = +1 } },
	{1, {{MODKEY|ShiftMask, XK_l}},      			rotatestack,    {.i = -1 } },
	{1, {{MODKEY|ShiftMask, XK_j}},				    movestack,      {.i = +1 } },
	{1, {{MODKEY|ShiftMask, XK_k}},      			movestack,      {.i = -1 } },
	{1, {{MODKEY, XK_i}},							incnmaster,     {.i = +1 } },
	{1, {{MODKEY, XK_d}},							incnmaster,     {.i = -1 } },
	// Resize
	{1, {{MODKEY, XK_h}},							setmfact,       {.f = -0.05} },
	{1, {{MODKEY, XK_l}},							setmfact,       {.f = +0.05} },
	/* {1, {{MODKEY, XK_0}},							view,           {.ui = ~0 } }, */
	/* {1, {{MODKEY|ShiftMask, XK_0}},					tag,            {.ui = ~0 } }, */
	// Layouts
	{1, {{MODKEY|ShiftMask, XK_space}},				togglefloating, {0} },
	{1, {{MODKEY, XK_t}},							setlayout,      {.v = &layouts[0]} },
	{1, {{MODKEY, XK_m}},							setlayout,      {.v = &layouts[1]} },
	{1, {{MODKEY, XK_f}},							setlayout,      {.v = &layouts[2]} },
	{1, {{MODKEY, XK_g}},      						setlayout,      {.v = &layouts[3]} },
	{1, {{MODKEY, XK_u}},      						setlayout,      {.v = &layouts[4]} },
	{1, {{MODKEY, XK_o}},						    setlayout,      {.v = &layouts[5]} },
	{1, {{MODKEY|ControlMask, XK_comma}}, 			cyclelayout,    {.i = -1 } },
	{1, {{MODKEY|ControlMask, XK_period}},			cyclelayout,    {.i = +1 } },
	{1, {{MODKEY, XK_Tab}},							view,           {0} },
	{1, {{MODKEY, XK_space}},						setlayout,      {0} },
	// Monitors
	{1, {{MODKEY, XK_comma}},						focusmon,       {.i = -1 } },
	{1, {{MODKEY, XK_period}},						focusmon,       {.i = +1 } },
	// Sound
	{2, {{MODKEY, XK_p}, {0, XK_a}},			   spawn,		SHCMD("pavucontrol")},
	{2, {{MODKEY, XK_p}, {0, XK_9}},							spawn,          SHCMD("killall pipewire") },
  // Cmus bindings
  {2, {{MODKEY, XK_p}, {0, XK_1}},         spawn,   SHCMD("cmus-remote --prev")},
  {2, {{MODKEY, XK_p}, {0, XK_2}},         spawn,   SHCMD("cmus-remote --pause")},
  {2, {{MODKEY, XK_p}, {0, XK_3}},         spawn,   SHCMD("cmus-remote --next")},
  {2, {{MODKEY, XK_p}, {0, XK_4}},         spawn,   SHCMD("cmus-remote --repeat")},
  {2, {{MODKEY, XK_p}, {0, XK_5}},         spawn,   SHCMD("cmus-remote --shuffle")},
	// Dwm Stuff
	{1, {{MODKEY|ShiftMask, XK_b}},							togglebar,      {0} },
	{1, {{MODKEY, XK_w}},							killclient,     {0} },
//	{1, {{MODKEY|ShiftMask, XK_comma}},				tagmon,         {.i = -1 } },
//	{1, {{MODKEY|ShiftMask, XK_period}},			tagmon,         {.i = +1 } },
	// Volume	
	/* {1,	{{0, XF86XK_AudioLowerVolume}},				spawn,		SHCMD("amixer sset Master 5%-") }, */
	/* {1, {{0, XF86XK_AudioRaiseVolume}},				spawn,		SHCMD("amixer sset Master 5%+") }, */
	/* {1,	{{0, XF86XK_AudioMute}},					spawn,		SHCMD("amixer -D pulse set Master toggle") }, */
	{1,	{{0, XK_F2}},				spawn,		SHCMD("pamixer -d 5") },
	{1, {{0, XK_F3}},				spawn,		SHCMD("pamixer -i 5") },
	{1,	{{0, XK_F4}},					spawn,		SHCMD("pamixer -t") },
 	TAGKEYS(                        XK_1,                      0)
 	TAGKEYS(                        XK_2,                      1)
 	TAGKEYS(                        XK_3,                      2)
 	TAGKEYS(                        XK_4,                      3)
 	TAGKEYS(                        XK_5,                      4)
 	TAGKEYS(                        XK_6,                      5)
 	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
 	TAGKEYS(                        XK_9,                      8)
	{1, {{MODKEY|ShiftMask, XK_q}},      			quit,           {0} },
	{1, {{MODKEY|ShiftMask, XK_r}},					quit,           {1} },
	{2, {{MODKEY, XK_s}, {0, XK_s}},      						togglesticky,   {0} },
	{2, {{MODKEY, XK_s}, {0, XK_1}},  togglescratch,  {.v = scratchpadcmd } },
	{2, {{MODKEY, XK_s}, {0, XK_2}},  togglescratch,  {.v = scratchpadcmd1 } },
 };

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

