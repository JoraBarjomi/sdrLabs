cmake_minimum_required(VERSION 3.20)
project(sdr VERSION 1.0 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

# Ищем библиотеку SoapySDR
find_package(SoapySDR REQUIRED)

set(MAIN_SOURCE_FILES
    main.cpp
    sdr/sdr.cpp
)

# Добавляем исполняемый файл
add_executable(sdr ${MAIN_SOURCE_FILES})

# Линкуем библиотеки к исполняемому файлу
target_link_libraries(sdr ${SoapySDR_LIBRARIES})