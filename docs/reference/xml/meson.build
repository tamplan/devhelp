ent_conf = configuration_data()
ent_conf.set('PACKAGE', dh_name)
ent_conf.set('PACKAGE_BUGREPORT', 'https://bugzilla.gnome.org/browse.cgi?product=' + dh_name)
ent_conf.set('PACKAGE_NAME', meson.project_name())
ent_conf.set('PACKAGE_STRING', '@0@ @1@'.format(meson.project_name(), dh_version))
ent_conf.set('PACKAGE_TARNAME', package_tarname)
ent_conf.set('PACKAGE_URL', package_url)
ent_conf.set('PACKAGE_VERSION', package_version)

ent = 'gtkdocentities.ent'

configure_file(
  input: ent + '.in',
  output: ent,
  configuration: ent_conf
)
