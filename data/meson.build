subdir('icons')

appdata = 'org.gnome.@0@.appdata.xml'.format(meson.project_name())

i18n.merge_file(
  appdata,
  input: appdata + '.in',
  output: appdata,
  po_dir: po_dir,
  install: true,
  install_dir: join_paths(dh_datadir, 'metainfo')
)

desktop = 'org.gnome.@0@.desktop'.format(meson.project_name())

desktop_conf = configuration_data()
desktop_conf.set('VERSION', dh_version)

desktop_in = configure_file(
  input: desktop + '.in.in',
  output: desktop + '.in',
  configuration: desktop_conf
)

i18n.merge_file(
  desktop,
  type: 'desktop',
  input: desktop_in,
  output: desktop,
  po_dir: po_dir,
  install: true,
  install_dir: join_paths(dh_datadir, 'applications')
)

service_conf = configuration_data()
service_conf.set('bindir', dh_bindir)

service = 'org.gnome.@0@.service'.format(meson.project_name())

configure_file(
  input: service + '.in',
  output: service,
  configuration: service_conf,
  install: true,
  install_dir: dbus_session_dir
)

assistant_data = files(
  'assistant.js',
  'assistant.css'
)

install_data(
  assistant_data,
  install_dir: join_paths(dh_pkgdatadir, 'assistant')
)

install_data(
  'org.gnome.@0@.gschema.xml'.format(dh_name),
  install_dir: join_paths(dh_datadir, 'glib-2.0', 'schemas')
)

install_data(
  dh_name + '.1',
  install_dir: join_paths(dh_mandir, 'man1')
)
