cmake_minimum_required(VERSION 3.10)

project(CodecEFK)

# Define o padrão do C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(include)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(my_codec 
    src/main.cpp 
    src/io/VideoIO.cpp
    # Conforme criar novos modulos, adicioná-los aqui
)

target_link_libraries(my_codec ${OpenCV_LIBS})