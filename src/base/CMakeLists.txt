BISON_TARGET(NetaParser neta_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/neta_grammar.cc)

add_library(base STATIC
  ${BISON_NetaParser_OUTPUTS}
  atomaddress.cpp
  atom.cpp
  atom_geometry.cpp
  basisshell.cpp
  bond.cpp
  cell.cpp
  choice.cpp
  colourscale.cpp
  colourscalepoint.cpp
  datastore.cpp
  eigenvector.cpp
  element.cpp
  elementmap.cpp
  encoderdefinition.cpp
  externalcommand.cpp
  forcefieldatom.cpp
  forcefieldbound.cpp
  grid.cpp
  gridpoint.cpp
  glyph.cpp
  kvmap.cpp
  lineparser.cpp
  log.cpp
  measurement.cpp
  neta.cpp
  neta_lexer.cpp
  neta_parser.cpp
  pattern.cpp
  plane.cpp
  prefs.cpp
  ring.cpp
  site.cpp
  sysfunc.cpp
  vibration.cpp
  wrapint.cpp
  zmatrix.cpp
  zmatrixelement.cpp
  atomaddress.h
  atom.h
  basisshell.h
  bond.h
  cell.h
  choice.h
  colourscale.h
  colourscalepoint.h
  datastore.h
  eigenvector.h
  element.h
  elementmap.h
  encoderdefinition.h
  externalcommand.h
  fileparser.h
  forcefieldatom.h
  forcefieldbound.h
  glyph.h
  grid.h
  gridpoint.h
  kvmap.h
  lineparser.h
  log.h
  neta.h
  neta_parser.h
  measurement.h
  pattern.h
  plane.h
  prefs.h
  ring.h
  site.h
  sysfunc.h
  vibration.h
  wrapint.h
  zmatrix.h
  zmatrixelement.h
)
target_include_directories(base PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
)

add_library(fourierdata STATIC
  fourierdata.cpp
  fourierdata.h
)

target_include_directories(fourierdata PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
)

# Messenger

# Meta-Objects
set(messenger_MOC_HDRS
  task.hui
)
QT5_WRAP_CPP(messenger_MOC_SRCS ${messenger_MOC_HDRS})

add_library(messenger STATIC
  message.cpp
  messenger.cpp
  message.h
  messenger.h
  task_funcs.cpp
  ${messenger_MOC_SRCS}
)

target_include_directories(messenger PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
)
target_link_libraries(messenger gui)
