add_library(crazyflieReturnToBase SHARED crazyflieReturnToBase.h crazyflieReturnToBase.cpp)
target_link_libraries(crazyflieReturnToBase
    BehaviourController
    BehaviourControllerSimulation
    argos3core_simulator
    argos3plugin_simulator_crazyflie
    argos3plugin_simulator_genericrobot)
