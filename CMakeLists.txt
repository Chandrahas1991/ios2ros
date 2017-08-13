cmake_minimum_required(VERSION 2.8.3)
project(data_bag_wapper)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++11")
#-DEIGEN_USE_MKL_ALL")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")

find_package(catkin REQUIRED COMPONENTS
    roscpp
    std_msgs
    cv_bridge 
    )

find_package(OpenCV REQUIRED)
find_package(Eigen3  REQUIRED)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIRS}
)


catkin_package(
    )

add_executable(data_bag_wapper
    src/data_bag_wapper_node.cpp)

target_link_libraries(data_bag_wapper ${catkin_LIBRARIES} ${OpenCV_LIBS})
