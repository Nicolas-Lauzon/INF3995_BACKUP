add_library(crazyflieSim SHARED crazyflieSim.h crazyflieSim.cpp)
target_link_libraries(crazyflieSim
    BehaviourController
    BehaviourControllerSimulation
    argos3core_simulator
    argos3plugin_simulator_crazyflie
    argos3plugin_simulator_genericrobot)
