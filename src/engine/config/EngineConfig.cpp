//Corresponding header
#include "game_engine/engine/config/EngineConfig.h"

//C system headers

//C++ system headers

//Other libraries headers
#include "resource_utils/common/ResourceFileHeader.h"
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
}

EngineConfig getDefaultEngineConfig(
    const std::string &projectInstallPrefix,
    const std::string &projectFolderName,
    const std::string &loadingScreenResourcesPath) {
  EngineConfig cfg;
  cfg.maxFrameRate = MAX_FRAME_RATE;

  auto& drawMgrCfg = cfg.managerHandlerCfg.drawMgrBaseCfg;
  cfg.managerHandlerCfg.drawMgrBaseCfg.monitorWidth = MONITOR_WIDTH;
  cfg.managerHandlerCfg.drawMgrBaseCfg.monitorHeight = MONITOR_HEIGHT;
  cfg.managerHandlerCfg.drawMgrBaseCfg.windowDisplayMode = windowDisplayMode;
  cfg.managerHandlerCfg.drawMgrBaseCfg.windowBorderMode = windowBorderMode;

  auto& rendererCfg = drawMgrCfg.rendererConfig;
  rendererCfg.maxRuntimeRendererCommands = MAX_RUNTIME_RENDERER_COMMANDS;
  rendererCfg.maxRuntimeWidgets = MAX_RUNTIME_WIDGETS;
  rendererCfg.maxRendererBackBufferDataSize =
      MAX_RENDERER_BACK_BUFFER_DATA_SIZE;

  auto& sdlContainersCfg = cfg.managerHandlerCfg.sdlContainersCfg;
  sdlContainersCfg.maxResourceLoadingThreads = MAX_RESOURCE_LOADING_THREADS;
  sdlContainersCfg.maxRuntimeSpriteBuffers = MAX_RUNTIME_SPRITE_BUFFERS;
  sdlContainersCfg.maxRuntimeTexts = MAX_RUNTIME_TEXTS;
  sdlContainersCfg.resourcesFolderLocation.
      append(projectInstallPrefix).append("/").
      append(projectFolderName).append("/").
      append(ResourceFileHeader::getResourcesFolderName()).append("/");

  const std::string loadingScreenFolderPath =
      sdlContainersCfg.resourcesFolderLocation + loadingScreenResourcesPath;

  auto& loadingScreenCfg = sdlContainersCfg.loadingScreenCfg;
  loadingScreenCfg.monitorWidth = MONITOR_WIDTH;
  loadingScreenCfg.monitorHeight = MONITOR_HEIGHT;
  loadingScreenCfg.backgroundImagePath =
      loadingScreenFolderPath + "background.png";
  loadingScreenCfg.progressBarOnImagePath =
      loadingScreenFolderPath + "progressOn.png";
  loadingScreenCfg.progressBarOffImagePath =
      loadingScreenFolderPath + "progressOff.png";

  return cfg;
}

