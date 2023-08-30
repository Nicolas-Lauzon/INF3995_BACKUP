add_library(crazyflieExplore SHARED crazyflieExplore.h crazyflieExplore.cpp)
target_link_libraries(crazyflieExplore
    BehaviourController
    BehaviourControllerSimulation
    argos3core_simulator
    argos3plugin_simulator_crazyflie
    argos3plugin_simulator_genericrobot)
