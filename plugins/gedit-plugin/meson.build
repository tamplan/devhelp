plugin_dir = join_paths(dh_libdir, 'gedit', 'plugins')

install_data(
  'devhelp.py',
  install_dir: plugin_dir
)

plugin = 'devhelp.plugin'

custom_target(
  plugin,
  input: plugin + '.desktop.in',
  output: plugin,
  command: msgfmt_plugin_cmd,
  install: true,
  install_dir: plugin_dir
)
