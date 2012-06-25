add_custom_target(run ./run WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_custom_target(todo which ack && ack --type=cpp TODO || grep -Hrn TODO *)
