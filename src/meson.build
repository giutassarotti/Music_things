music_sheet_lib = static_library(
	'music_sheet', 
	[
		'music_sheet.cpp',
		'beat.cpp',
		'note.cpp',
		'clef.cpp',
		'scale.cpp',
		'time.cpp',
		'basic_note.cpp'
	],
	dependencies: [open_cv, json]
)

music_sheet = declare_dependency(
	link_with: music_sheet_lib,
	include_directories: ['.'],
	dependencies: [open_cv]
)

executable('main', 
			dependencies: [open_cv, music_sheet], 
			sources: ['main.cpp'])