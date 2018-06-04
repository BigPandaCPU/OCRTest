cmake_minimum_required( VERSION 3.0 )
project( SACViewer )
find_package( Qt5 REQUIRED Widgets  Xml)


set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
SET(CMAKE_MFC_FLAG 1)
add_definitions(-DUNICODE -D_UNICODE)  
set(ThirdParty_Dir  "../3rdParty/SACBenchmark/")

set( Project_srcs
	main.cpp
	ocrtest.cpp
	CentralWidget.cpp
	SubWidget.cpp
	SACUtil.cpp
	Document.cpp
	ImageDocument.cpp
	PDFDocument.cpp
	)
	
	
set( Project_moc_hdrs
	ocrtest.h
	CentralWidget.h
	SubWidget.h
	SACUtil.h
	Document.h
	ImageDocument.h
	PDFDocument.h
	)
	
Qt5_wrap_cpp( Project_moc_src ${Project_moc_hdrs} )
include_directories(${ThirdParty_Dir}/CommonLibs/SACBase/Header
					${ThirdParty_Dir}/CommonLibs/Interpreter/Header
					${ThirdParty_Dir}/CommonLibs/KernelWrapper/Header
					${ThirdParty_Dir}/CommonLibs/StructureAnalysisCore/ExportHeader
					${ThirdParty_Dir}/__Internal/Share/Include
					${ThirdParty_Dir}/__Internal/Share/Include/UPBase
					${ThirdParty_Dir}/__Internal/Share/Include/XMLElf
                   )
link_directories(${ThirdParty_Dir}/__Internal/Share/Libs/Win32
				 ${ThirdParty_Dir}/__Out/Win32/Debug/Libs
				 ${ThirdParty_Dir}/__Out/Win32/Release/Libs
				)				   
link_libraries(	
	SAC.lib
	SACBase.lib
	DFEParserKernel.lib
	XEKrnl.lib
	EncryptKey.lib
	KernelWrapper.lib
)






#忽略的库名称
#uafxcwd.lib;LIBCMTD.lib;libcmt


include_directories( ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${Qt5Core_INCLUDE_DIRS} )
add_executable(SACViewer ${Project_srcs} ${Project_uis_h} ${Project_moc_src} ${Project_moc_hdrs})
target_link_libraries(SACViewer  Qt5::Widgets Qt5::Xml
					  debug      EncryptLibD.lib
					  optimized  EncryptLib.lib
					  debug      CEBDHCreatorD.lib
					  optimized  CEBDHCreator.lib
					  debug      XMLElfParserD.lib
					  optimized  XMLElfParser.lib
					  debug      UPBaseD.lib
					  optimized  UPBase.lib
					  debug      InterpreterD.lib
					  optimized  Interpreter.lib
					  debug      uafxcwd.lib
					  debug      LIBCMTD.lib
					  optimized  uafxcw.lib
					  optimized  libcmt.lib)
