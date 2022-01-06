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
    const std::string &projectFolderName,
    const std::string &loadingScreenResourcesPath) {
  EngineConfig cfg;

  cfg.maxFrameRate = MAX_FRAME_RATE;

  cfg.managerHandlerCfg.drawMgrBaseCfg.rendererConfig.maxRuntimeRendererCommands =
      MAX_RUNTIME_RENDERER_COMMANDS;
  cfg.managerHandlerCfg.drawMgrBaseCfg.rendererConfig.maxRuntimeWidgets =
      MAX_RUNTIME_WIDGETS;
  cfg.managerHandlerCfg.drawMgrBaseCfg.rendererConfig.maxRendererBackBufferDataSize =
      MAX_RENDERER_BACK_BUFFER_DATA_SIZE;

  cfg.managerHandlerCfg.drawMgrBaseCfg.monitorWidth = MONITOR_WIDTH;
  cfg.managerHandlerCfg.drawMgrBaseCfg.monitorHeight = MONITOR_HEIGHT;
  cfg.managerHandlerCfg.drawMgrBaseCfg.windowDisplayMode = windowDisplayMode;
  cfg.managerHandlerCfg.drawMgrBaseCfg.windowBorderMode = windowBorderMode;

  cfg.managerHandlerCfg.sdlContainersCfg.maxResourceLoadingThreads =
      MAX_RESOURCE_LOADING_THREADS;
  cfg.managerHandlerCfg.sdlContainersCfg.maxRuntimeSpriteBuffers =
      MAX_RUNTIME_SPRITE_BUFFERS;
  cfg.managerHandlerCfg.sdlContainersCfg.maxRuntimeTexts = MAX_RUNTIME_TEXTS;
  cfg.managerHandlerCfg.sdlContainersCfg.resourcesBinLocation =
      FileSystemUtils::getBuildDirectory();
  cfg.managerHandlerCfg.sdlContainersCfg.resourcesBinLocation.append(
      projectFolderName).append("/").append(
      ResourceFileHeader::getResourcesBinFolderName()).append("/");

  cfg.managerHandlerCfg.sdlContainersCfg.loadingScreenCfg.loadingScreenUsage =
      LoadingScreenUsage::ENABLED;
  cfg.managerHandlerCfg.sdlContainersCfg.loadingScreenCfg.monitorWidth =
      MONITOR_WIDTH;
  cfg.managerHandlerCfg.sdlContainersCfg.loadingScreenCfg.monitorHeight =
      MONITOR_HEIGHT;
  const std::string loadingScreenFolderPath =
      FileSystemUtils::getRootDirectory() + loadingScreenResourcesPath;
  cfg.managerHandlerCfg.sdlContainersCfg.loadingScreenCfg.backgroundImagePath =
      loadingScreenFolderPath + "background.png";
  cfg.managerHandlerCfg.sdlContainersCfg.loadingScreenCfg.progressBarOnImagePath =
      loadingScreenFolderPath + "progressOn.png";
  cfg.managerHandlerCfg.sdlContainersCfg.loadingScreenCfg.progressBarOffImagePath =
      loadingScreenFolderPath + "progressOff.png";

  return cfg;
}

