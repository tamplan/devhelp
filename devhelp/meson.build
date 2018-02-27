devhelp_inc = include_directories('.')

private_headers = [
  'dh-error.h',
  'dh-parser.h',
  'dh-search-context.h',
  'dh-settings.h',
  'dh-util-lib.h'
]

headers = [
  'devhelp.h',
  'dh-assistant-view.h',
  'dh-book.h',
  'dh-book-manager.h',
  'dh-book-tree.h',
  'dh-completion.h',
  'dh-init.h',
  'dh-keyword-model.h',
  'dh-link.h',
  'dh-sidebar.h'
]

install_headers(
  headers,
  subdir: join_paths(dh_api_name, dh_name)
)

private_sources = [
  'dh-error.c',
  'dh-parser.c',
  'dh-search-context.c',
  'dh-settings.c',
  'dh-util-lib.c'
]

sources = [
  'dh-assistant-view.c',
  'dh-book.c',
  'dh-book-manager.c',
  'dh-book-tree.c',
  'dh-completion.c',
  'dh-init.c',
  'dh-keyword-model.c',
  'dh-link.c',
  'dh-sidebar.c'
]

enum = 'dh-enum-types'

sources += gnome.mkenums(
  enum,
  sources: headers,
  c_template: enum + '.c.template',
  h_template: enum + '.h.template',
  install_header: true,
  install_dir: join_paths(dh_pkgincludedir, dh_name)
)

deps = [
  gio_dep,
  gtk_dep,
  webkit_gtk_dep
]

cflags = [
  '-DDATADIR="@0@"'.format(dh_datadir),
  '-DLOCALEDIR="@0@"'.format(dh_localedir)
]

libdevhelp_core = static_library(
  'devhelp-core',
  sources: sources + private_sources,
  include_directories: top_inc,
  dependencies: deps + [gsettings_desktop_dep],
  c_args: cflags
)

libdevhelp_core_dep = declare_dependency(
  include_directories: devhelp_inc,
  dependencies: deps,
  link_with: libdevhelp_core
)

symbol_map = join_paths(meson.current_source_dir(), 'symbol.map')

ldflags = []
if have_version_script
  ldflags += '-Wl,--version-script,@0@'.format(symbol_map)
endif

libdevhelp = shared_library(
  libdh_name,
  version: libdh_version,
  include_directories: top_inc,
  link_args: ldflags,
  link_whole: libdevhelp_core,
  link_depends: symbol_map,
  install: true
)

libdevhelp_dep = declare_dependency(
  include_directories: devhelp_inc,
  dependencies: deps,
  link_with: libdevhelp
)

pkg.generate(
  libraries: libdevhelp,
  version: dh_version,
  name: dh_name,
  description: dh_name,
  filebase: 'lib' + dh_api_name,
  subdirs: dh_api_name,
  requires: [
    'gio-2.0 ' + gio_req_version,
    'gtk+-3.0 ' + gtk_req_version,
    '@0@ @1@'.format(webkit_gtk_name, webkit_gtk_req_version)
  ],
  requires_private: 'gsettings-desktop-schemas',
  variables: 'exec_prefix=' + dh_prefix
)

if enable_introspection
  incs = [
    'Gtk-3.0',
    'WebKit2-4.0'
  ]

  gnome.generate_gir(
    libdevhelp,
    sources: sources + headers,
    includes: incs,
    #include_directories: top_inc,
    namespace: meson.project_name(),
    nsversion: dh_api_version,
    symbol_prefix: dh_namespace.to_lower(),
    identifier_prefix: dh_namespace,
    header: 'devhelp/devhelp.h',
    export_packages: 'lib' + dh_api_name,
    # FIXME: unnecessary?
    extra_args: '--identifier-prefix=' + dh_namespace.to_lower(),
    install: true
  )
endif
