/* See LICENSE file for copyright and license details. */

#include <xkbcommon/xkbcommon-keysyms.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[] = {"RobotoMono Nerd Font:style=Regular:size=8",
                              "Font Awesome 6 Free:style=Regular:size=8"};
static const char dmenufont[] = "Font Awesome 6 Free:style=Regular:size=8";
static const char color_fg[] = "#7270ee";
static const char color_fg_selected[] = "#fdcd11";
static const char color_bg[] = "#303030";
static const char color_bg_selected[] = "#303030";
static const char color_border[] = "#6474aa";
static const char color_border_selected[] = "#edcd11";
static const char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] = {color_fg, color_bg, color_border},
    [SchemeSel] = {color_fg_selected, color_bg_selected, color_border_selected},
};

/* tagging */
static const char *tags[] = {"", "", "", "", "", "", "🖌️", "", ""};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
        {"Gimp"            , NULL        , NULL   , 0      , 1 , -1} ,
        {"Google-chrome"   , NULL        , NULL   , 1 << 1 , 0 , -1} ,
        {"firefox"         , NULL        , NULL   , 1 << 1 , 0 , -1} ,
        {"mpv"             , NULL        , NULL   , 1 << 4 , 1 , 1}  ,
        {"Spotify"         , NULL        , NULL   , 1 << 4 , 1 , 1}  ,
        {"wiki"            , NULL        , NULL   , 0      , 1 , -1} ,
        {"todos"           , NULL        , NULL   , 0      , 1 , -1} ,
        {"Dunst"           , NULL        , NULL   , 0      , 1 , -1} ,
        {"Zathura"         , NULL        , NULL   , 1 << 2 , 0 , -1} ,
        {"Gnucash"         , NULL        , NULL   , 1 << 2 , 0 , -1} ,
        {"DBeaver"         , NULL        , NULL   , 1 << 5 , 0 , 1}  ,
        {"Postman"         , NULL        , NULL   , 1 << 5 , 0 , 1}  ,
        {"Slack"           , NULL        , NULL   , 1 << 3 , 0 , 1}  ,
        {"zoom"            , NULL        , NULL   , 1 << 3 , 1 , -1} ,
        {"discord"         , NULL        , NULL   , 1 << 3 , 0 , -1} ,
        {"TelegramDesktop" , NULL        , NULL   , 1 << 3 , 0 , -1} ,
        {NULL              , NULL        , "cmus" , 1 << 4 , 0 , 1}  ,
        {"Nightly"         , "Navigator" , NULL   , 1 << 1 , 0 , -1} ,
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2]                    = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]              = {"rofi-menu.sh", NULL};
static const char *termcmd[]               = {"st", "-e", "tmux-session.sh", NULL};
static const char *wiki[]                  = {"st", "-e", "wiki.sh", NULL};
static const char *notes[]                 = {"st", "-e", "notes.sh", NULL};
static const char *mixer[]                 = {"st", "-e", "pulsemixer", NULL};
static const char *cmus[]                  = {"st", "-e", "cmus", NULL};
static const char *pop_last_notification[] = {"dunstctl", "history-pop", NULL};
static const char *close_notification[]    = {"dunstctl", "close-all", NULL};
static const char *radio[]                 = {"rofi-radio.sh", NULL};
static const char *clipmenu[]              = {"clipmenu", NULL};
static const char *screenshot[]            = {"screenshot.sh", NULL};
static const char *browser[]               = {"firefox", NULL};
static const char *chat[]                  = {"slack", NULL};
static const char *cmus_pause[]            = {"cmus-remote", "-u", NULL};
static const char *cmus_next[]             = {"cmus-remote", "-n", NULL};
static const char *cmus_prev[]             = {"cmus-remote", "-r", NULL};
static const char *volume_up[]             = {"pactl", "set-sink-volume", "0", "+5%", NULL};
static const char *volume_down[]           = {"pactl", "set-sink-volume", "0", "-5%", NULL};
static const char *toggle_mute[]           = {"pactl", "set-sink-mute", "0", "toggle", NULL};
static const char *move_right[]            = { "xdotool", "mousemove_relative", "15", "0", NULL};
static const char *move_left[]             = { "xdotool", "mousemove_relative", "--", "-15", "0", NULL};
static const char *move_up[]               = { "xdotool", "mousemove_relative", "--", "0", "-15", NULL};
static const char *move_down[]             = { "xdotool", "mousemove_relative", "0", "15", NULL};
static const char *click_left[]            = { "xdotool", "click", "--clearmodifiers", "1", NULL};
static const char *click_right[]           = { "xdotool", "click", "--clearmodifiers", "3", NULL};
static const char *backlit_kbd_on[]        = { "backlit.sh", "kbd_on", NULL };
static const char *backlit_kbd_off[]       = { "backlit.sh", "kbd_off", NULL };
static const char *backlit_screen_dim[]    = { "backlit.sh", "screen_dim", NULL };
static const char *backlit_screen_full[]   = { "backlit.sh", "screen_full", NULL };
static const char *backlit_screen_up[]     = { "backlit.sh", "screen_up", NULL };
static const char *backlit_screen_down[]   = { "backlit.sh", "screen_down", NULL };
static const char *main_monitor[]          = { "wacom.sh", "0", NULL};
static const char *aux_monitor[]           = { "wacom.sh", "1", NULL};

static const Key keys[] = {
	/* modifier                     key        function        argument */
        { NULL                               , XKB_KEY_XF86MonBrightnessUp   , spawn          , {.v = backlit_screen_up } }     ,
        { NULL                               , XKB_KEY_XF86MonBrightnessDown , spawn          , {.v = backlit_screen_down } }   ,
        { NULL                               , XKB_KEY_XF86KbdBrightnessUp   , spawn          , {.v = backlit_kbd_on } }        ,
        { NULL                               , XKB_KEY_XF86KbdBrightnessDown , spawn          , {.v = backlit_kbd_off } }       ,
	{ MODKEY                             , XK_p                          , spawn          , {.v = dmenucmd } }              ,
	{ MODKEY                             , XK_r                          , spawn          , {.v = radio } }                 ,
	{ MODKEY|ShiftMask                   , XK_h                          , spawn          , {.v = move_left } }             ,
	{ MODKEY|ShiftMask                   , XK_j                          , spawn          , {.v = move_down } }             ,
	{ MODKEY|ShiftMask                   , XK_k                          , spawn          , {.v = move_up } }               ,
	{ MODKEY|ShiftMask                   , XK_l                          , spawn          , {.v = move_right } }            ,
	{ MODKEY                             , XK_o                          , spawn          , {.v = backlit_screen_dim } }    ,
	{ MODKEY|ShiftMask                   , XK_o                          , spawn          , {.v = backlit_screen_full } }   ,
	{ MODKEY                             , XK_f                          , spawn          , {.v = click_left } }            ,
	{ MODKEY                             , XK_g                          , spawn          , {.v = click_right } }           ,
	{ MODKEY                             , XK_w                          , spawn          , {.v = browser } }               ,
	{ MODKEY                             , XK_n                          , spawn          , {.v = wiki } }                  ,
	{ MODKEY                             , XK_v                          , spawn          , {.v = clipmenu } }              ,
	{ MODKEY                             , XK_s                          , spawn          , {.v = screenshot } }            ,
	{ MODKEY|ShiftMask                   , XK_s                          , spawn          , {.v = chat } }                  ,
	{ MODKEY|ShiftMask                   , XK_p                          , spawn          , {.v = mixer } }                 ,
	{ MODKEY|ShiftMask                   , XK_n                          , spawn          , {.v = notes } }                 ,
	{ MODKEY                             , XK_m                          , spawn          , {.v = cmus } }                  ,
	{ MODKEY                             , XK_c                          , spawn          , {.v = close_notification } }    ,
	{ MODKEY                             , XK_z                          , spawn          , {.v = pop_last_notification } } ,
	{ MODKEY                             , XK_Return                     , spawn          , {.v = termcmd } }               ,
	{ MODKEY                             , XK_Up                         , spawn          , {.v = volume_up } }             ,
	{ MODKEY                             , XK_Down                       , spawn          , {.v = volume_down } }           ,
	{ MODKEY                             , XK_End                        , spawn          , {.v = toggle_mute } }           ,
	{ MODKEY                             , XK_Left                       , spawn          , {.v = cmus_prev } }             ,
	{ MODKEY                             , XK_Right                      , spawn          , {.v = cmus_next } }             ,
	{ MODKEY                             , XK_Home                       , spawn          , {.v = cmus_pause } }            ,
	{ MODKEY                             , XK_F1                         , spawn          , {.v = aux_monitor} }            ,
	{ MODKEY                             , XK_F2                         , spawn          , {.v = main_monitor } }          ,
	{ MODKEY                             , XK_b                          , togglebar      , {0} }                           ,
	{ MODKEY                             , XK_j                          , focusstack     , {.i = +1 } }                    ,
	{ MODKEY                             , XK_k                          , focusstack     , {.i = -1 } }                    ,
	{ MODKEY                             , XK_i                          , incnmaster     , {.i = +1 } }                    ,
	{ MODKEY                             , XK_d                          , incnmaster     , {.i = -1 } }                    ,
	{ MODKEY                             , XK_h                          , setmfact       , {.f = -0.05} }                  ,
	{ MODKEY                             , XK_l                          , setmfact       , {.f = +0.05} }                  ,
	{ MODKEY|ShiftMask                   , XK_Return                     , zoom           , {0} }                           ,
	{ MODKEY                             , XK_Tab                        , view           , {0} }                           ,
	{ MODKEY|ShiftMask                   , XK_c                          , killclient     , {0} }                           ,
	{ MODKEY                             , XK_t                          , setlayout      , {.v = &layouts[0]} }            ,
	{ MODKEY|ShiftMask                   , XK_f                          , setlayout      , {.v = &layouts[1]} }            ,
	{ MODKEY|ShiftMask                   , XK_m                          , setlayout      , {.v = &layouts[2]} }            ,
	{ MODKEY|ShiftMask                   , XK_space                      , setlayout      , {0} }                           ,
	{ MODKEY                             , XK_space                      , togglefloating , {0} }                           ,
	{ MODKEY                             , XK_0                          , view           , {.ui = ~0 } }                   ,
	{ MODKEY|ShiftMask                   , XK_0                          , tag            , {.ui = ~0 } }                   ,
	{ MODKEY                             , XK_comma                      , focusmon       , {.i = -1 } }                    ,
	{ MODKEY                             , XK_period                     , focusmon       , {.i = +1 } }                    ,
	{ MODKEY|ShiftMask                   , XK_comma                      , tagmon         , {.i = -1 } }                    ,
	{ MODKEY|ShiftMask                   , XK_period                     , tagmon         , {.i = +1 } }                    ,
	TAGKEYS(                        XK_1 , 0)
	TAGKEYS(                        XK_2 , 1)
	TAGKEYS(                        XK_3 , 2)
	TAGKEYS(                        XK_4 , 3)
	TAGKEYS(                        XK_5 , 4)
	TAGKEYS(                        XK_6 , 5)
	TAGKEYS(                        XK_7 , 6)
	TAGKEYS(                        XK_8 , 7)
	TAGKEYS(                        XK_9 , 8)
	{ MODKEY|ShiftMask                   , XK_q                          , quit           , {0} }                           ,
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
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

