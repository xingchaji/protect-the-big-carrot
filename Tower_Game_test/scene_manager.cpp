#include "scene_manager.h"

#include "menu_scene.h"
#include "game_scene.h"
#include "result_scene.h"

SceneManager::SceneManager()
    : currentScene(nullptr),
      running(true)
{
}

SceneManager::~SceneManager()
{
    if (currentScene != nullptr)
    {
        currentScene->onExit();
        delete currentScene;
        currentScene = nullptr;
    }
}

void SceneManager::start(SceneType firstScene)
{
    switchScene(firstScene);
}

void SceneManager::switchScene(
    SceneType target,
    bool victory,
    int levelIndex)
{
    if (currentScene != nullptr)
    {
        currentScene->onExit();
        delete currentScene;
        currentScene = nullptr;
    }

    switch (target)
    {
    case SceneType::Menu:
        currentScene = new MenuScene();
        break;

    case SceneType::Game:
        currentScene = new GameScene(levelIndex);
        break;

    case SceneType::Result:
        currentScene = new ResultScene(victory, levelIndex);
        break;

    case SceneType::Exit:
        running = false;
        return;

    case SceneType::None:
        return;
    }

    if (currentScene != nullptr)
    {
        currentScene->onEnter();
    }
}

void SceneManager::processRequest()
{
    if (currentScene == nullptr || !currentScene->hasRequest())
    {
        return;
    }

    const SceneRequest sceneRequest = currentScene->getRequest();
    currentScene->clearRequest();

    switchScene(
        sceneRequest.target,
        sceneRequest.victory,
        sceneRequest.levelIndex);
}

void SceneManager::handleInput(const ExMessage& msg)
{
    if (currentScene == nullptr)
    {
        return;
    }

    currentScene->handleInput(msg);
    processRequest();
}

void SceneManager::update(DWORD deltaTime)
{
    if (currentScene == nullptr)
    {
        return;
    }

    currentScene->update(deltaTime);
    processRequest();
}

void SceneManager::draw() const
{
    if (currentScene != nullptr)
    {
        currentScene->draw();
    }
}

bool SceneManager::isRunning() const
{
    return running;
}
