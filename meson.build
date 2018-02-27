project(
  'Devhelp', 'c',
  version: '3.27.91',
  license: 'GPL2+',
  default_options: 'buildtype=debugoptimized',
  meson_version: '>= 0.43.0'
)

dh_name = meson.project_name().to_lower()

dh_version = meson.project_version()
version_array = dh_version.split('.')
dh_major_version = version_array[0].to_int()

dh_api_version = '@0@.0'.format(dh_major_version)
dh_api_name = '@0@-@1@'.format(dh_name, dh_api_version)

dh_namespace = 'Dh'

dh_gir_version = '1.0'

dh_prefix = get_option('prefix')
dh_bindir = join_paths(dh_prefix, get_option('bindir'))
dh_datadir = join_paths(dh_prefix, get_option('datadir'))
dh_includedir = join_paths(dh_prefix, get_option('includedir'))
dh_libdir = join_paths(dh_prefix, get_option('libdir'))
dh_localedir = join_paths(dh_prefix, get_option('localedir'))
dh_mandir = join_paths(dh_prefix, get_option('mandir'))

dh_pkgdatadir = join_paths(dh_datadir, dh_name)
dh_pkgincludedir = join_paths(dh_includedir, dh_api_name)

# LT Version numbers, remember to change them just *before* a release.
# A good time to update it is for the GNOME x.y.90 version, at API freeze,
# so that packagers have the time to update the package.
#   (Interfaces removed:    CURRENT++, AGE=0, REVISION=0)
#   (Interfaces added:      CURRENT++, AGE++, REVISION=0)
#   (No interfaces changed:                   REVISION++)
libdh_current = 5
libdh_revision = 0
libdh_age = 0
libdh_version = '@0@.@1@.@2@'.format(libdh_current, libdh_revision, libdh_age)

libdh_name = '@0@-@1@'.format(dh_name, dh_major_version)

# package
package_tarname = dh_name
package_url = 'https://wiki.gnome.org/Apps/Devhelp'
package_version = dh_version

cc = meson.get_compiler('c')

config_h = configuration_data()

# defines
set_defines = [
  # package
  ['PACKAGE_TARNAME', package_tarname],
  ['PACKAGE_URL', package_url],
  ['PACKAGE_VERSION', package_version],
  # i18n
  ['GETTEXT_PACKAGE', dh_name]
]

foreach define: set_defines
  config_h.set_quoted(define[0], define[1])
endforeach

# compiler flags
common_flags = []

if get_option('buildtype').contains('debug')
  test_cflags = [
    '-fno-strict-aliasing',
    '-Wcast-align',
    '-Wdeclaration-after-statement',
    '-Werror=unknown-warning-option',
    '-Wformat=2',
    '-Winit-self',
    '-Winline',
    '-Wmissing-declarations',
    '-Wmissing-format-attribute',
    '-Wmissing-include-dirs',
    '-Wmissing-noreturn',
    '-Wmissing-prototypes',
    '-Wnested-externs',
    '-Wno-error=missing-field-initializers',
    '-Wno-error=unused-parameter',
    '-Wno-missing-field-initializers',
    '-Wno-suggest-attribute=format',
    '-Wno-unused-parameter',
    '-Wold-style-definition',
    '-Wpacked',
    '-Wredundant-decls',
    '-Wshadow',
    '-Wstrict-prototypes',
    '-Wswitch-enum',
    '-Wundef',
    '-Wwrite-strings'
  ]

  common_flags += cc.get_supported_arguments(test_cflags)
endif

add_project_arguments(common_flags, language: 'c')

ldflag = '-Wl,--version-script'
have_version_script = host_machine.system().contains('linux') and cc.has_argument(ldflag)

gio_req_version = '>= 2.40'
gtk_req_version = '>= 3.22'

webkit_gtk_name = 'webkit2gtk-4.0'
webkit_gtk_req_version = '>= 2.19.2'

gio_dep = dependency('gio-2.0', version: gio_req_version)
gsettings_desktop_dep = dependency('gsettings-desktop-schemas')
gtk_dep = dependency('gtk+-3.0', version: gtk_req_version)
webkit_gtk_dep = dependency(webkit_gtk_name, version: webkit_gtk_req_version)
m_dep = cc.find_library('m')

dbus_session_dir = get_option('dbus_session_dir')
if dbus_session_dir == ''
  dbus_dep = dependency('dbus-1', required: false)
  assert(dbus_dep.found(), 'D-Bus required but not found, please provide a valid session bus services dir')
  dbus_session_dir = dbus_dep.get_pkgconfig_variable('session_bus_services_dir')
endif

enable_introspection = get_option('introspection')
if enable_introspection
  dependency('gobject-introspection-1.0', version: '>= 1.30.0')
endif

# Flatpak
enable_flatpak = get_option('flatpak')
config_h.set('FLATPAK_BUILD', enable_flatpak)

gnome = import('gnome')
i18n = import('i18n')
pkg = import('pkgconfig')

po_dir = join_paths(meson.source_root(), 'po')

# FIXME: Needed to also merge Description key in plugin files
msgfmt_plugin_cmd = [
  find_program('msgfmt'),
  '--desktop',
  '--keyword=Name',
  '--keyword=Description',
  '-d', po_dir,
  '--template=@INPUT@',
  '--output=@OUTPUT@'
]

top_inc = include_directories('.')

subdir('data')
subdir('plugins/gedit-plugin')
subdir('po')
subdir('help')
subdir('devhelp')
subdir('src')
subdir('unit-tests')

enable_gtk_doc = get_option('gtk_doc')
if enable_gtk_doc
  subdir('docs/reference')
endif

configure_file(
  output: 'config.h',
  configuration: config_h
)

meson.add_install_script(
  'meson_post_install.py',
  dh_datadir
)

# Print a summary of the configuration
output = 'Configuration:\n\n'
output += '        ' + meson.project_name() +  ' version ' + dh_version + '\n\n'
output += '        Source code location:  ' + meson.source_root() + '\n'
output += '        Prefix:                ' + dh_prefix + '\n'
output += '        Compiler:              ' + cc.get_id() + '\n'
output += '        API documentation:     ' + enable_gtk_doc.to_string() + '\n'
output += '        GObject introspection: ' + enable_introspection.to_string() + '\n'
output += '        Flatpak build mode:    ' + enable_flatpak.to_string()
message(output)
