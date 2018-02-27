sources = files(
  'dh-app.c',
  'dh-assistant.c',
  'dh-main.c',
  'dh-preferences.c',
  'dh-tab.c',
  'dh-tab-label.c',
  'dh-util-app.c',
  'dh-web-view.c',
  'dh-window.c',
  'tepl-info-bar.c'
)

resource_files = files(
  'dh-assistant.ui',
  'dh-preferences.ui',
  'dh-window.ui',
  'help-overlay.ui',
  'menus.ui'
)

sources += gnome.compile_resources(
  'dh-resources',
  'dh.gresource.xml',
  c_name: dh_namespace.to_lower(),
  dependencies: resource_files,
  export: true
)

deps = [
  libdevhelp_dep,
  m_dep
]

executable(
  dh_name,
  sources,
  include_directories: top_inc,
  dependencies: deps,
  c_args: '-DG_LOG_DOMAIN="@0@"'.format(meson.project_name()),
  install: true
)
