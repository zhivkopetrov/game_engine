//Corresponding header
#include "game_engine/engine/config/EngineConfig.h"

//C system headers

//C++ system headers

//Other libraries headers
#include "resource_utils/common/ResourceFileHeader.h"
#include "sdl_utils/SDLLoader.h"
#include "utils/file_system/FileSystemUtils.h"

//Own components headers

namespace {
constexpr auto windowDisplayMode = WindowDisplayMode::FULL_SCREEN;
constexpr auto windowBorderMode = WindowBorderMode::BORDERLESS;
constexpr auto MONITOR_WIDTH = 1920;
constexpr auto MONITOR_HEIGHT = 1080;
constexpr auto MAX_FRAME_RATE = 75;
constexpr auto MAX_RESOURCE_LOADING_THREADS = 2;
constexpr auto MAX_RUNTIME_TEXTS = 400;
constexpr auto MAX_RUNTIME_SPRITE_BUFFERS = 50;
constexpr auto MAX_RUNTIME_IMAGES = 200;
constexpr auto MAX_RUNTIME_WIDGETS = MAX_RUNTIME_TEXTS
    + MAX_RUNTIME_SPRITE_BUFFERS + MAX_RUNTIME_IMAGES;
constexpr auto MAX_RUNTIME_RENDERER_COMMANDS = MAX_RUNTIME_WIDGETS * 2;
//65k bytes
constexpr auto MAX_RENDERER_BACK_BUFFER_DATA_SIZE =
    std::numeric_limits<uint16_t>::max();

LoadingScreenConfig generateLoadingScreenConfig(
    const std::string &loadingScreenFolderPath) {
  LoadingScreenConfig cfg;
  cfg.loadingScreenUsage = LoadingScreenUsage::ENABLED;
  cfg.backgroundImagePath = loadingScreenFolderPath + "background.png";
  cfg.progressBarOnImagePath = loadingScreenFolderPath + "progressOn.png";
  cfg.progressBarOffImagePath = loadingScreenFolderPath + "progressOff.png";
  return cfg;
}
} //end anonymous namespace

EngineConfig getDefaultEngineConfig(
    const std::string &projectInstallPrefix,
    const std::string &loadingScreenResourcesPath) {
  EngineConfig cfg;
  cfg.maxFrameRate = MAX_FRAME_RATE;

  auto &drawMgrCfg = cfg.managerHandlerCfg.drawMgrCfg;
  auto &monitorCfg = drawMgrCfg.monitorWindowConfig;
  monitorCfg.name = "Default Name";
  monitorCfg.width = MONITOR_WIDTH;
  monitorCfg.height = MONITOR_HEIGHT;
  monitorCfg.displayMode = windowDisplayMode;
  monitorCfg.borderMode = windowBorderMode;

  auto &rendererCfg = drawMgrCfg.rendererConfig;
  rendererCfg.maxRuntimeRendererCommands = MAX_RUNTIME_RENDERER_COMMANDS;
  rendererCfg.maxRuntimeWidgets = MAX_RUNTIME_WIDGETS;
  rendererCfg.maxRendererBackBufferDataSize =
      MAX_RENDERER_BACK_BUFFER_DATA_SIZE;

  auto &sdlContainersCfg = cfg.managerHandlerCfg.sdlContainersCfg;
  sdlContainersCfg.maxResourceLoadingThreads = MAX_RESOURCE_LOADING_THREADS;
  sdlContainersCfg.maxRuntimeSpriteBuffers = MAX_RUNTIME_SPRITE_BUFFERS;
  sdlContainersCfg.maxRuntimeTexts = MAX_RUNTIME_TEXTS;
  sdlContainersCfg.resourcesFolderLocation.append(projectInstallPrefix).append(
      "/").append(ResourceFileHeader::getResourcesFolderName()).append("/");

  auto &loadingScreenCfg = sdlContainersCfg.loadingScreenCfg;
  if (!loadingScreenResourcesPath.empty()) {
    const auto loadingScreenFolderPath =
        sdlContainersCfg.resourcesFolderLocation + loadingScreenResourcesPath;
    loadingScreenCfg = generateLoadingScreenConfig(loadingScreenFolderPath);
  } else {
    loadingScreenCfg.loadingScreenUsage = LoadingScreenUsage::DISABLED;
  }

  return cfg;
}

std::vector<DependencyDescription> getDefaultEngineDependencies(
    [[maybe_unused]]int32_t argc, [[maybe_unused]]char **args) {
  const DependencyDescription sdlDependencies
    { "SDL2", SDLLoader::init, SDLLoader::deinit };
  return { sdlDependencies };
}

