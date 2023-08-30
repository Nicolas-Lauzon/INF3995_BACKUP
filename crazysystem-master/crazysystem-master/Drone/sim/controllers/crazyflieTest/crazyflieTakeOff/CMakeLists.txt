add_library(crazyflieTakeOff SHARED crazyflieTakeOff.h crazyflieTakeOff.cpp)
target_link_libraries(crazyflieTakeOff
    BehaviourController
    BehaviourControllerSimulation
    argos3core_simulator
    argos3plugin_simulator_crazyflie
    argos3plugin_simulator_genericrobot)
