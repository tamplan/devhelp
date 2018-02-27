subdir('xml')

doc_module = dh_name

doc_path = join_paths(dh_prefix, gnome.gtkdoc_html_dir(libdh_name))

gnome.gtkdoc(
  doc_module,
  main_xml: doc_module + '-docs.xml',
  src_dir: devhelp_inc,
  dependencies: libdevhelp_dep,
  scan_args: [
    '--rebuild-types',
    '--ignore-headers=' + ' '.join(private_headers)
  ],
  gobject_typesfile: doc_module + '.types',
  fixxref_args: '--html-dir=' + doc_path,
  install: true,
  install_dir: doc_path
)
