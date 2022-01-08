music_sheet_lib = static_library(
	'music_sheet', 
	[
		'music_sheet.cpp'
	]
)

music_sheet = declare_dependency(
	link_with: music_sheet_lib,
	include_directories: ['.']
)

executable('main', 
			dependencies: [open_cv], 
			sources: ['main.cpp'])