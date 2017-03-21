cmake_minimum_required(VERSION 2.8.3)
project(ios2ros)

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

add_executable(ios2ros
    src/ios2ros_node.cpp)

target_link_libraries(ios2ros ${catkin_LIBRARIES} ${OpenCV_LIBS})
