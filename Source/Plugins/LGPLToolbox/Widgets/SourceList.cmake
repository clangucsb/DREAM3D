# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                                                               #
# Copyright (c) 2015 The Regents of the University of California, Author: William Lenthe        #
#                                                                                               #
# This program is free software: you can redistribute it and/or modify it under the terms of    #
# the GNU Lesser General Public License as published by the Free Software Foundation, either    #
# version 2 of the License, or (at your option) any later version.                              #
#                                                                                               #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     #
# See the GNU Lesser General Public License for more details.                                   #
#                                                                                               #
# You should have received a copy of the GNU Lesser General Public License                      #
# along with this program.  If not, see <http://www.gnu.org/licenses/>.                         #
#                                                                                               #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set(${PLUGIN_NAME}_Widgets_HDRS "")
set(${PLUGIN_NAME}_Widgets_SRCS "")
set(${PLUGIN_NAME}_Widgets_UIS "")


# --------------------------------------------------------------------
# List the Classes here that are QWidget Derived Classes
set(DREAM3D_WIDGETS
 #    SomeGeneralReusableWidget
)

foreach(FPW ${DREAM3D_WIDGETS})
  set(${PLUGIN_NAME}_Widgets_HDRS ${${PLUGIN_NAME}_Widgets_HDRS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/${FPW}.h
    )
  set(${PLUGIN_NAME}_Widgets_SRCS ${${PLUGIN_NAME}_Widgets_SRCS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/${FPW}.cpp
    )
  set(${PLUGIN_NAME}_Widgets_UIS ${${PLUGIN_NAME}_Widgets_UIS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/UI_Files/${FPW}.ui
    )
endforeach()

# Add in the remaining sources that are actually widgets but are completely Custom and do NOT use private
# inheritance through a .ui file
set(${PLUGIN_NAME}_Widgets_HDRS
  ${${PLUGIN_NAME}_Widgets_HDRS}
)
set(${PLUGIN_NAME}_Widgets_SRCS
  ${${PLUGIN_NAME}_Widgets_SRCS}
)


# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/Widgets" "${${PLUGIN_NAME}_Widgets_HDRS}" "${${PLUGIN_NAME}_Widgets_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PLUGIN_NAME}/Widgets/UI_Files" "${${PLUGIN_NAME}_Widgets_UIS}" "")

# --------------------------------------------------------------------
# We are using CMake's AuotMoc feature so we do not need to wrap our .cpp files with a specific call to 'moc'

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PLUGIN_NAME}_Widgets_Generated_UI_HDRS ${${PLUGIN_NAME}_Widgets_UIS} )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PLUGIN_NAME}_Widgets_Generated_UI_HDRS}" "" "0")

# --------------------------------------------------------------------
# If you are doing more advanced Qt programming where you are including resources you will have to enable this section
# with your own cmake codes to include your resource file (.qrc) and any other needed files
# QT5_ADD_RESOURCES( ${PLUGIN_NAME}_Generated_RC_SRCS ""  )
# cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${${PLUGIN_NAME}_Generated_RC_SRCS}" "" "0")




