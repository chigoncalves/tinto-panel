2010-06-26
==========
* unhide `tint2 <https://code.google.com/archive/p/tint2/source>`_ panel when dragging something
* battery *FreeBSD* uses the new *ACPI API* (thanks to
  yamagi.burmeister)

2010-06-16
==========
* Set ``_NET_WM_ICON_GEOMETRY`` for every task

2010-06-12
==========
* strut_policy=minimum is possible even if autohide is disabled

2010-06-11
==========
* Added icons and a *\ *.desktop* file for **tint2conf**
* BSD fixes

2010-06-09
==========
* Add `CMake <http://cmake.org>`_ build system

2010-06-08
==========
* *Xrandr* and *Nvidia* problems fixed

2010-05-17
==========
* Fix installation of examples

2010-05-15
==========
* added panel theme switcher ``tint2conf`` with preview of each theme
  based on current config (size of monitor, task opened, ...)
* include tintwizard : theme editor
* autodetect composite manager and switch to real or fake transparency
  this feature needed a patch to xcompmgr see
  `FAQ <http://code.google.com/p/tint2/wiki/FAQ#How_to_configure_real_transparency_(xcompmgr)>`_
* added ``strut_policy = none``
* drag n drop task re-ordering


2010-01-28
==========
* Battery hide option
* bugfixes in task redrawing
* traywindow and autohide bugfix


2010-01-13
==========
* calculate right struts for multiple monitors
* New timer implementation (works for *\ *BSD* too)
* Changed behaviour for forwarding clicks to the *window manager*
* `tint2 <https://code.google.com/archive/p/tint2/source>`_ sets a class name, such that it can be identified by window managers
* better omnipresent task handling
* clock timezone support
* real transparency
* adjustable systray icons
* systray: several icons in one line
* autohide
* more task states (active/urgent/iconified)
* and some minor bugfixes which nobody recognized yet ;)


2009-12-16
==========
* Scrolling through tasks by schattenprinz (added action next_task and prev_task)
* Andreas : *add* setting tooltip on clock with ``clock_tooltip = format``
* Andreas : *add* changed everything to kernel timer for easier integration with new timer
* Andreas : *add* multiple tasks can be urgent now
* Thierry : panel snapshot
* patch for FULL battery by peter.ebden
* Thierry : work on tint2conf. not ready for release yet.
* Thierry : ``killall -SIGUSR1 tint2`` will reload config file cleanly
* Thierry : improved multi-monitor detection
* Thierry : added left2right and right2left value to systray_sort option
* Andreas : *add* option for ascending/descending/unsorted systray
* Thierry : EWMH non-resizable panel
* less disturbing omnipresent task by dmitry medvinsky
* Thierry : lower cpu use with icon. replace HUE by ALPHA on icon (see task_icon_asb)
* Thierry : add default icon on task
* Andreas : *add* tooltips
* Andreas : added possibility to put `tint2 <https://code.google.com/archive/p/tint2/source>`_ into the windowmanagers dock
* Andreas : Drag n drop over Task bring Focus
* Andreas : support skip_taskbar
* Thierry : detect pid of process owning the systray
* Andreas : added possibility to put `tint2 <https://code.google.com/archive/p/tint2/source>`_ into the windowmanagers dock

2009-09-08
==========
* Drag n drop over Task bring Focus by Andreas.Fink85

2009-09-07
==========
* fixed issue 117 : support skip_taskbar by Andreas.Fink85
* cleanup : switch space to tab

2009-09-01
==========
* fixed segfault

2009-08-30
==========
* detect pid of process owning the systray

2009-08-30
==========
* added taskbar_active_background_id to change current desktop background.
  warning : this feature have a price on cpu (`tint2 <https://code.google.com/archive/p/tint2/source>`_ redraw all task) when you switch desktop.
  comment out ``taskbar_active_background_id`` if you don't want it.

2009-08-30
==========
* fixed issue 130 by maato
  ordered systray icon with config ``systray_sort = asc/desc``

2009-08-29
==========
* fixed issue 134

2009-08-29
==========
* cleanup and add desktop_right/desktop_left action on task (by jackp)

2009-08-29
==========
* adjust saturation/brightness on task icon

2009-08-29
==========
* merge change described in issue 122 (by jackp)
  made right click easier

2009-08-28
==========
* add some HSB config

2009-08-09
==========
* add mouse action : ``maximize_restore``
* fixed issue 125

2009-07-28
==========
* some tint2conf code

2009-07-26
==========
* fixed icon on 64 bit system by benjaminfranzke

2009-07-19
==========
* fixed systray crash by mart1987

2009-07-14
==========
* play with some tint2conf code

2009-07-04
==========
* fixed *defunct* process after fork

----

2009-06-30  thilor77 <lorthiois@bbsoft.fr>
==========================================
released tint-0.7

2009-06-29
==========
* fixed bug when active window draged to another desktop

2009-06-29
==========
* fixed issue 110 and issue 107

2009-06-25
==========
* improve user's message when compiled with --disable-battery
* replace ``task_width = width`` by ``task_maximum_size = width height``
  width will be used in horizontal panel
  height will be used in vertical panel

2009-06-24
==========
* vertical panel done
  user's parameters :

     #. add ``vertical`` to panel_position (panel_position = center
        left vertical)

     #. use ``task_width`` to define the task height (``task_width =
        30``)

     #. adjust paddingy on task and systray wiil define the size of
     icons all others parameters are automatically converted by
     `tint2 <https://code.google.com/archive/p/tint2/source>`_ (x padding become y padding, width become height ...)
     so user's change between horizontal and vertical are minimised.


2009-06-23
==========
* play with vertical panel
  check sample file tintrc05 and tintrc06
  more change to come...

2009-06-21
==========
* play with some code for vertical panel

2009-06-20
==========
* remove flickering on systray

2009-06-20
==========
* ``urgent_nb_of_blink`` allow to choose the number of blink

2009-06-19
==========
* change systray code. fixed issue 91.

2009-06-18
==========
* patch by Christian Ruppert, configure option for battery

2009-06-14
==========
* revert r78, fixed issue 100

2009-06-13
==========
* fixed background refresh on systray

2009-06-09
==========
* fixed issue 98

2009-06-08
==========
* fixed issue 97 and issue 95

2009-06-08
==========
* allow ``taskbar_mode = multi_desktop`` config with ``panel_monitor = all``

2009-06-07
==========
* merge autotool build system by Christian Ruppert

2009-06-06
==========
* fixed issue 56 : with windowmaker
  bug in windowmaker : send unecessary ``desktop changed`` each time focus changed

2009-06-06
==========
* add ``wm_menu`` config option to disable window manager's menu
  if you don't specify ``wm_menu =``, the default value will show WM menu.

2009-06-06
==========
* fixed issue 90 and issue 81 : window manager's menu in panel padding
  works on openbox, pekwm, fluxbox, icewm.
  doesn't work on xfce, windowmaker ?
  `tint2 <https://code.google.com/archive/p/tint2/source>`_ send mouse event to your window manager on left/right padding
  and on top/bottom panel padding.

2009-06-05
==========
* fixed issue 46 and 59 : urgent task
  limitation : just one urgent task at a time

2009-06-04
==========
* patch by dmitry.medvinsky : right/left clic command

2009-06-02
==========
* Systray Temp_Fix from keshto
  not yet perfect, but a first step

2009-05-31
==========
* fixed issue 71 : time format %T %r

2009-05-31
==========
* fixed issue 86 : padding bug

2009-05-31
==========
* fixed issue 87 and 88 : battery detection

2009-05-26
==========
* fixed memory leak and battery detection

2009-05-20
==========
* merge patch from yarin.kaul (fixed issue 52)

2009-05-15
==========
* merge battery applet from Sebastian Reichel <elektranox@gmail.com>

2009-03-14
==========
* add systray option when convert tint-0.6 config file
  but you need to stop other systemtray program

2009-03-07
==========
* fixed segfault when time1_format empty
* fixed systray : when clock change size
* fixed systray segfault when xrandr change size

2009-03-01
==========
* fixed segfault on tray application due to `tint2 <https://code.google.com/archive/p/tint2/source>`_

2009-03-01
==========
* cleanup code
* basic systray with some bugs
* update documentation and tintrc sample for systray

2009-02-27
==========
* fixed issue 49 : patch from Daniel Moerner
* some systray code (doesn't work)
* cleanup code

2009-02-25
==========
* fixed issue 48 : `tint2 <https://code.google.com/archive/p/tint2/source>`_ does't create config file on first start

2009-02-14
==========
* fixed issue 45 : segfault without clock

2009-02-13
==========
* improved object oriented layout
* `tint2 <https://code.google.com/archive/p/tint2/source>`_ looks good for me. if you see bugs, report it.

2009-02-12
==========
* fixed issue 14 : no icons

2009-02-11
==========
* fixed bug with number of desktop changed
* fixed clock resize when date changed

2009-02-08
==========
* backward compatibility with tint-0.6 (convert config file)
  feature freeze until next release. need bug correction.

2009-02-07
==========
* better multi-monitor capability :
  * ``panel_monitor = all`` : `tint2 <https://code.google.com/archive/p/tint2/source>`_ draw one panel per monitor
  * ``panel_size = 90% 30`` : panel_size accept percentage or pixel
* change notation : MULTI_MONITOR changed to ``SINGLE_MONITOR`` in
  SINGLE_MONITOR mode, `tint2 <https://code.google.com/archive/p/tint2/source>`_ draw one taskbar with tasks from
  current monitor
* removed sleep mode (replaced by ``panel_monitor = all``)
* update documentation and sample config file

2009-02-03
==========
* better fixed for Robert Escriva problem : ``set_panel_properties``
  -> Reserved space
* fixed issue 13 with background detection
* fixed bug with disconnected monitor (in multi monitor mode)
* removed capability to show Window magager's menu
  it's a feature I would like, but implementation was buggy

2009-01-29
==========
* use panel_margin parameter even with ``full_width``
* ``multi_monitor`` mode : monitor number are ordered according to coordinate
  so taskbar on the left show task in the left monitor

2009-01-20
==========
* change config file format : add another padding parameter
* update documentation and sample config file

2009-01-19
==========
* patch from Robert Escriva, fixed `tint2 <https://code.google.com/archive/p/tint2/source>`_ on multi-monitor
* fixed bug between ``MULTI_MONITOR`` mode and 'task on all desktop'

2009-01-18
==========
* update documentation for new config format
* fixed memory corruption
* fixed bug with "task on all desktop" (issue 39)

2009-01-17
==========
* fixed drawing of active task (with new layout)

2009-01-05
==========
* fixed decorated window with compiz

2008-12-30
==========
* fixed bug : segfault on ConfigureNotify event

2008-11-12
==========
* fixed bug : segfault with icon
* panel's left and right padding feel like WM background
  (right click open window managers's menu, ...)

2008-11-02
==========
* fixed bugs with new design
* panel.area manage the tree of visible objects

2008-10-28
==========
* fixed issue : "untitled task", "task button did not update", "window don't shade",
  "error if I don't do ``make install``", "Makefile error"

2008-09-24
==========
* change tintrc format
  user can define a list of background and border at the beginning of tintrc
  and each object (panel, taskbar, task, clock, systray) get a background number.
  the background number 0 is reserved by `tint2 <https://code.google.com/archive/p/tint2/source>`_ for full transparency.
  user's background start with number 1, 2, ...

2008-09-22
==========
* better control on icon opacity
  icon opacity use the same value as text opacity

2008-09-21
==========
* new parameter ``task_text = 0`` or ``1``
  so you can show task's icon without text
* remove ``task_icon_size`` parameter because ``icon_size`` is
  calculate with ``task->padding_y``

2008-09-19
==========
* improve clock drawing on 2 lines
* new design : object oriented
    clock inherit ``Area`` (with ``draw_background``)
* rename parameter ``panel_mode`` in ``taskbar_mode``

2008-09-17
==========
* change name from tint to `tint2
  <https://code.google.com/archive/p/tint2/source>`_ because debian
  has already a package named tint
* new design : object oriented
    tasks inherit Area
    taskbars inherit Area

2008-09-16
==========
* new design : object oriented
    remove ``task_margin`` parameter, replaced by ``taskbar_padding``
    fixed segfault with new layout

2008-09-04
==========
* new design : object oriented
  all objects inherit from Area : panel, taskbar, task, clock

2008-09-02 licence changed to GPL V2
====================================
*Hi Pål,
Since latest release (tint-0.6), I try to merge a systray in tint.
The code I would like to merge is under GPL V2 licence.
So are you agree to change the licence of tint to GPL V2 ?*

Pål Staurland answer :
*Yeah no problem::)*

2008-08-17 patch from i...@noctus.net
=====================================
* fixed bug : "Task button titles do not update"

2008-08-12
==========
* code cleanup : Taskbar use GSLIST from glib

2008-08-07
==========
* add 1 space between task title and icon

2008-07-25
==========
* fixed bug when (*task_rounded > panel_height/2*)


----

2008-07-20  thil7 <lorthiois@bbsoft.fr>
=======================================
released tint-0.6

2008-07-19
==========
* backward compatibility : save new config file

2008-07-18
==========
* bug fixed with multi-monitor : memory leak, move task on the same monitor

2008-07-12
==========
* multi_monitor : added ``window_get_monitor(win)``

2008-07-05
==========
* multi_monitor : changed taskbar allocation when add/remove a monitor
* update documentation

2008-07-04
==========
* changed in config file, panel_show_all_desktop replaced by
  ``panel_mode = single_desktop | multi_desktop | multi_monitor``
* with ``panel_mode = multi_monitor``, tint show one taskbar by monitor

2008-07-02
==========
* add *Xinerama* dependency
* read monitors configuration
* tint will run in ``sleep_mode``
  if you specify ``panel_monitor`` on an unplug monitor
* tint 'wake up' when the monitor is detected or resolution changed

2008-06-30 patch from goo...@dougbarton.us
==========================================
* tint wait for window manager on startup

2008-06-29
==========
* specify monitor with parameter ``panel_monitor``
* panel_position is relative to your monitor

::

  panel_monitor = 1
  panel_position = bottom center

2008-06-24
==========
* fixed bug : show **tint** on all desktop with fluxbox

2008-06-23
==========
* ``task_width = 0`` to get full taskbar width
* added documentation in /doc

2008-06-16
==========
* renamed parameter ``panel_margin`` to ``panel_padding``
* added parameter ``panel_margin = vertical_margin |
  horizontal_margin``

2008-06-15
==========
* fixed bug : icon position changed when clock adjust width
* removed boolean parameter ``panel_background`` and
  ``task_background`` replaced with opacity (alpha) = 0
* removed ``task_active_border_width`` parameter
  identical to ``task_border_width``
* fixed bug : draw task below panel border

2008-06-11
==========
* removed the need of desktop margin.
  tint specify margin to window magager (``_NET_WM_STRUT``)

2008-06-10
==========
* fixed bug : time/date vertical and horizontal center

2008-06-10  patch from benjaminfranzke
======================================
* fixed bug : draw icon on 64bit system.

2008-06-08
==========
* fixed bug : in draw border and fill background

2008-06-04
==========
* allow config file on the command line ``% tint -c ./tintrc2``
* allow 2 value in config file : ``key = value1 value2``
* panel_margin can get 2 values : ``vertical_margin horizontal_margin``
  ``panel_margin = 8 4``

2008-05-30
==========
* put panel below other window
* set locale LANG

2008-05-28
==========
* clock_width adjust automatically
* configure : ``time2_format`` see ``man strftime``

2008-05-27
==========
* configure : ``time1_format`` see ``man strftime``
* cleanup code : ``refresh_pos ()``

2008-05-26
==========
* catch time event in main loop
* draw clock fixed format ``%H:%M``

2008-05-24
==========
* removed boolean parameter task_icon
  ``task_icon_size = 0`` replace ``task_icon = 0``
* all colors parameters can get 2 values : color and opacity (alpha)
  ``font_color = #ffffff 70``

2008-05-23
==========
* cpu optimisation : limit call to ``visual_refresh ()``

2008-05-22
==========
* configure clock : ``clock_font``, ``clock_font_color``


----

2008-05-20  thil7 <lorthiois@bbsoft.fr>
=======================================
released tint-0.5

2008-05-19
==========
* improve usability in mode ``show_all_desktop``
  -> don't switch desktop when close a task of another desktop
  -> allow to drag and drop task between desktop
* change panel_position : you can now specify top or bottom. values :
  bottom left, bottom right, bottom center, top left, top right, top
  center keep compatibility with old tintrc files
* change Makefile :
  add ``$PREFIX`` and ``$DESTDIR``
  install default config file in ``/etc/xdg/tint/tintrc``
* on startup, tint copy ``$XDG_CONFIG_DIR/tint/tintrc`` in
  ``~/.config/tint/tintrc`` so each user can customize tintrc
* fixed : name of task in fluxbox
* improve focus detection (TransientForHint)
* cleanup code : send_event


----

2008-05-14  thil7 <lorthiois@bbsoft.fr>
=======================================
released tint-0.4

2008-05-12
==========
* boolean option ``show_all_desktop``
  0 : tint panel show one taskbar (from current desktop)
  1 : tint panel show all taskbars (from all desktops)

  * clic on a task switch to the desktop
  * clic on a taskbar (if empty) switch to the desktop

* add layout for taskbar : panel -> taskbar -> task
* cleanup code : state ``active`` in Task replaced by task_active in Panel
* hide some window : ``_NET_WM_WINDOW_TYPE_DIALOG``,
  ``_NET_WM_STATE_MODAL``
* change project to *MIT licence* because Pål Staurland put *MIT
  licence* in all file source


----

2008-05-10  thil7 <lorthiois@bbsoft.fr>
=======================================
released tint-0.3

2008-05-10
==========
* manage events : 'send to desktop' and 'send to all desktop'
* bug solved : ``close`` action now send ``_NET_CLOSE_WINDOW``
  so openoffice ask 'save document...' if needed
* cpu optimisation :
  -> don't redraw panel background on all events. just when desktop change.
  -> don't remove/add all tasks when switch between desktop. remove ``task_new_desktop()``.
* small bug on ``_NET_WM_STATE_SKIP_TASKBAR`` corrected
* memory leak on ``g_build_filename``


----

2008-05-09  thil7 <lorthiois@bbsoft.fr>
=======================================
released tint-0.2

2008-05-08
==========
* panel : add rounded and border (color and alpha)
* remove option ``panel_tasks_centered``
* user can configure mouse event on task
  (none, toggle, iconify, shade, close, toggle_iconify)

2008-05-07
==========
* cpu optimisation : resize_tasks only when add/remove task,
  compute pos_y at startup

2008-05-04
==========
* task width adjust when too many task

2008-05-01
==========
* task : add rounded and border (color and alpha)
* panel position (left, right, center) thank to ak47izatool
* cleanup code : vertical position (icon and text),
  better refresh after delete task, memory leak \*font
* panel : changed panel_margin and task_margin

2008-04-22
==========
* fork `ttm <http://code.google.com/p/ttm/>`_ projet from (by Pål
  Staurland staura@gmail.com) while the projet is no longer in
  developpement, have not changed the name of *tint*.
