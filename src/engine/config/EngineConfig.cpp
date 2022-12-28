//Corresponding header
#include "game_engine/engine/config/EngineConfig.h"

//System headers
#include <limits>

//Other libraries headers
#include "sdl_utils/SDLLoader.h"
#include "sdl_utils/drawing/config/RendererConfig.h"
#include "resource_utils/common/ResourceFileHeader.h"
#include "utils/file_system/FileSystemUtils.h"
#include "utils/data_type/EnumClassUtils.h"

//Own components headers

namespace {
constexpr auto windowDisplayMode = WindowDisplayMode::FULL_SCREEN;
constexpr auto windowBorderMode = WindowBorderMode::BORDERLESS;
constexpr auto MONITOR_WIDTH = 1920;
constexpr auto MONITOR_HEIGHT = 1080;
constexpr auto MAX_RUNTIME_TEXTS = 500;
constexpr auto MAX_RUNTIME_SPRITE_BUFFERS = 250;
constexpr auto MAX_RUNTIME_IMAGES = 400;
constexpr auto MAX_RUNTIME_WIDGETS = MAX_RUNTIME_TEXTS
    + MAX_RUNTIME_SPRITE_BUFFERS + MAX_RUNTIME_IMAGES;
constexpr auto MAX_RUNTIME_RENDERER_COMMANDS = MAX_RUNTIME_WIDGETS * 2;
//65k bytes
constexpr auto MAX_RENDERER_BACK_BUFFER_DATA_SIZE =
    std::numeric_limits<uint16_t>::max();

#ifdef __EMSCRIPTEN__
//emscripten port currently requires single-threaded usage on the
//whole application level

//use browser’s requestAnimationFrame mechanism
constexpr auto MAX_FRAME_RATE = 0;
constexpr auto MAX_RESOURCE_LOADING_THREADS = 0;
constexpr auto RENDERER_POLICY = RendererPolicy::SINGLE_THREADED;
constexpr auto INPUT_EVENT_HANDLER_POLICY = 
  InputEventHandlerPolicy::POLL_BLOCKING;
#else
constexpr auto MAX_FRAME_RATE = 60;
constexpr auto MAX_RESOURCE_LOADING_THREADS = 2;
constexpr auto RENDERER_POLICY = RendererPolicy::MULTI_THREADED;
constexpr auto INPUT_EVENT_HANDLER_POLICY = 
  InputEventHandlerPolicy::RUN_IN_DEDICATED_THREAD;
#endif

LoadingScreenConfig generateLoadingScreenConfig(
    const std::string &loadingScreenFolderPath) {
  LoadingScreenConfig cfg;
  cfg.loadingScreenUsage = LoadingScreenUsage::ENABLED;
  cfg.backgroundImagePath = loadingScreenFolderPath + "background.png";
  cfg.progressBarOnImagePath = loadingScreenFolderPath + "progressOn.png";
  cfg.progressBarOffImagePath = loadingScreenFolderPath + "progressOff.png";
  return cfg;
}

DebugConsoleConfig generateDebugConsoleConfig() {
  DebugConsoleConfig cfg;
  cfg.textColor = Colors::RED;
  cfg.maxFrameRate = MAX_FRAME_RATE;
  return cfg;
}

} //end anonymous namespace

EngineConfig getDefaultEngineConfig(
    const std::string &projectInstallPrefix,
    const std::string &loadingScreenResourcesPath) {
  EngineConfig cfg;
  cfg.maxFrameRate = MAX_FRAME_RATE;
  cfg.inputEventHandlerPolicy = INPUT_EVENT_HANDLER_POLICY;
  cfg.debugConsoleConfig = generateDebugConsoleConfig();

  auto &drawMgrCfg = cfg.managerHandlerCfg.drawMgrCfg;
  auto &monitorCfg = drawMgrCfg.monitorWindowConfig;
  monitorCfg.name = "Default Name";
  monitorCfg.width = MONITOR_WIDTH;
  monitorCfg.height = MONITOR_HEIGHT;
  monitorCfg.displayMode = windowDisplayMode;
  monitorCfg.borderMode = windowBorderMode;

  auto &rendererCfg = drawMgrCfg.rendererConfig;
  rendererCfg.executionPolicy = RENDERER_POLICY;
  rendererCfg.flagsMask = getEnumValue(RendererFlag::HARDWARE_RENDERER) |
      getEnumValue(RendererFlag::FBO_ENABLE);
  rendererCfg.scaleQuality = RendererScaleQuality::LINEAR;
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
  const std::vector<DependencyDescription> dependencies { { "SDL2",
      SDLLoader::initSdl2, SDLLoader::deinitSdl2 }, { "SDL2-image",
      SDLLoader::initSdl2Image, SDLLoader::deinitSdl2Image }, { "SDL2-ttf",
      SDLLoader::initSdl2Ttf, SDLLoader::deinitSdl2Ttf }, { "SDL2-mixer",
      SDLLoader::initSdl2Mixer, SDLLoader::deinitSdl2Mixer } };

  return dependencies;
}

