cmake_minimum_required (VERSION 3.11)
project(qspi_analyzer)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

set(SOURCES 
source/qspiAnalyzer.cpp
source/qspiAnalyzer.h
source/qspiAnalyzerResults.cpp
source/qspiAnalyzerResults.h
source/qspiAnalyzerSettings.cpp
source/qspiAnalyzerSettings.h
source/qspiAnalyzerCommands.cpp 
source/qspiAnalyzerCommands.h 
# source/qspiSimulationDataGenerator.cpp
# source/qspiSimulationDataGenerator.h
)

add_analyzer_plugin(qspi_analyzer SOURCES ${SOURCES})