add_custom_target(run ./run working_diretory ${CMAKE_SOURCE_DIR})
add_custom_target(todo which ack && ack --type=nomake TODO || grep -Hrn TODO *)
