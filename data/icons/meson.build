icons = [
  '16x16',
  '22x22',
  '24x24',
  '32x32',
  '48x48',
  '256x256'
]

foreach icon: icons
  install_data(
    join_paths(icon, 'devhelp.png'),
    install_dir: join_paths(dh_datadir, 'icons', 'hicolor', icon, 'apps')
  )
endforeach

install_data(
  'symbolic/devhelp-symbolic.svg',
  install_dir: join_paths(dh_datadir, 'icons', 'hicolor', 'symbolic', 'apps')
)
