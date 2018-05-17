cmake_minimum_required( VERSION 3.0 )
project( OCRTest )
find_package( Qt5 REQUIRED Widgets )

set( Project_srcs
	main.cpp
	ocrtest.cpp
	CentralWidget.cpp
	SubWidget.cpp
	SACUtil.cpp
	PDFDocument.cpp
	)
	
set( Project_uis
	ocrtest.ui
	)
	
set( Project_moc_hdrs
	ocrtest.h
	CentralWidget.h
	SubWidget.h
	SACUtil.h
	PDFDocument.h
	)
	
Qt5_wrap_ui( Project_uis_h ${Project_uis} )
Qt5_wrap_cpp( Project_moc_src ${Project_moc_hdrs} )
include_directories("E:\\OpenSource\\SAC\\SACBenchmark\\CommonLibs\\SACBase\\Header")
include_directories("E:\\OpenSource\\SAC\\SACBenchmark\\CommonLibs\\Interpreter\\Header")
include_directories("E:\\OpenSource\\SAC\\SACBenchmark\\CommonLibs\\KernelWrapper\\Header")
include_directories("E:\\OpenSource\\SAC\\SACBenchmark\\CommonLibs\\StructureAnalysisCore\\ExportHeader")
include_directories("E:\\OpenSource\\SAC\\SACBenchmark\\__Internal\\Share\\Include")
include_directories("E:\\OpenSource\\SAC\\SACBenchmark\\__Internal\\Share\\Include\\UPBase")
include_directories("E:\\OpenSource\\SAC\\SACBenchmark\\__Internal\\Share\\Include\\XMLElf")
link_directories("E:\\OpenSource\\SAC\\SACBenchmark\\__Internal\\Share\\Libs\\Win32")
link_directories("E:\\OpenSource\\SAC\\SACBenchmark\\__Out\\Win32\\Debug\\Libs")
link_libraries(	
	SAC.lib
	SACBase.lib
	DFEParserKernel.lib
	XEKrnl.lib
	EncryptLibD.lib
	EncryptKey.lib
	CEBDHCreatorD.lib
	UPBaseD.lib
	XMLElfParserD.lib
	InterpreterD.lib
	KernelWrapper.lib
	uafxcwd.lib
	LIBCMTD.lib
)


include_directories( ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${Qt5Core_INCLUDE_DIRS} )
add_executable(OCRTest ${Project_srcs} ${Project_uis_h} ${Project_moc_src} ${Project_moc_hdrs})
target_link_libraries(OCRTest Qt5::Widgets)
