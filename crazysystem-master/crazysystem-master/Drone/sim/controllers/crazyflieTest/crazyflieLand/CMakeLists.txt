add_library(crazyflieLand SHARED crazyflieLand.h crazyflieLand.cpp)
target_link_libraries(crazyflieLand
    BehaviourController
    BehaviourControllerSimulation
    argos3core_simulator
    argos3plugin_simulator_crazyflie
    argos3plugin_simulator_genericrobot)
