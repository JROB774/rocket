#include "main.hpp"

cs::AppConfig csMain(int argc, char** argv)
{
    cs::AppConfig appConfig;
    appConfig.title = "Rocket";
    appConfig.window.min = cs::Vec2i(180,320);
    appConfig.screenSize = cs::Vec2i(180,320);
    appConfig.app = cs::Allocate<RocketApp>(CS_MEM_GAME);
    return appConfig;
}
