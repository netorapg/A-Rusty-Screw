#include "../include/arscrew/InputManager.h"
#include <iostream>

namespace ARSCREW
{
    InputManager::InputManager()
    {
        for (int i = 0; i < static_cast<int>(InputAction::DEBUG_TOGGLE) + 1; i++)
        {
            InputAction action = static_cast<InputAction>(i);
            mActionStates[action] = false;
            mPreviousActionStates[action] = false;
        }

        for (int i = 0; i < SDL_NumJoysticks(); i++)
        {
            if (SDL_IsGameController(i))
            {
                connectController(i);
                break;
            }
        }
    }

    InputManager::~InputManager()
    {     
        disconnectController();
    }

    void InputManager::handleEvent(SDL_Event& e)
    {
        switch (e.type)
        {
            case SDL_CONTROLLERDEVICEADDED:
                if (!mController && SDL_IsGameController(e.cdevice.which))
                {
                    connectController(e.cdevice.which);
                }
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                if (mController && e.cdevice.which == SDL_JoystickInstanceID(
                    SDL_GameControllerGetJoystick(mController)))
                {
                    disconnectController();
                }
                break;
        }
    }

    void InputManager::update()
    {
        mPreviousActionStates = mActionStates;
        
        // Resetar estados antes de atualizar
        for (auto& pair : mActionStates) {
            pair.second = false;
        }

        updateKeyboardInput();
        updateControllerInput();
    }

    void InputManager::updateKeyboardInput()
    {
        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

        mActionStates[InputAction::MOVE_LEFT] = keyboardState[SDL_SCANCODE_A];
        mActionStates[InputAction::MOVE_RIGHT] = keyboardState[SDL_SCANCODE_D];
        mActionStates[InputAction::JUMP] = keyboardState[SDL_SCANCODE_SPACE];
        mActionStates[InputAction::ATTACK] = keyboardState[SDL_SCANCODE_J];
        mActionStates[InputAction::DASH] = keyboardState[SDL_SCANCODE_E];
        mActionStates[InputAction::SWITCH_ATTACK] = keyboardState[SDL_SCANCODE_Q];
        mActionStates[InputAction::CROUCH] = keyboardState[SDL_SCANCODE_S];
        mActionStates[InputAction::DEBUG_TOGGLE] = keyboardState[SDL_SCANCODE_LCTRL];
    }

    void InputManager::updateControllerInput()
    {
        if (!mController) return;

        mActionStates[InputAction::JUMP] = mActionStates[InputAction::JUMP] || 
            SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_A);

        mActionStates[InputAction::ATTACK] = mActionStates[InputAction::ATTACK] ||
            SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_X);

        mActionStates[InputAction::DASH] = mActionStates[InputAction::DASH] ||
            SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_B);
        
        mActionStates[InputAction::SWITCH_ATTACK] = mActionStates[InputAction::SWITCH_ATTACK] ||
            SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_Y);
        
        mActionStates[InputAction::CROUCH] = mActionStates[InputAction::CROUCH] || 
            SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
            
        mActionStates[InputAction::DEBUG_TOGGLE] = mActionStates[InputAction::DEBUG_TOGGLE] || 
            SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_BACK);
        
            mActionStates[InputAction::MOVE_LEFT] = mActionStates[InputAction::MOVE_LEFT] ||
            SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        
        mActionStates[InputAction::MOVE_RIGHT] = mActionStates[InputAction::MOVE_RIGHT] ||
            SDL_GameControllerGetButton(mController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

        float leftStickX = getLeftStickX();
        if (leftStickX < -0.5f)
            mActionStates[InputAction::MOVE_LEFT] = true;
        else if (leftStickX > 0.5f)
            mActionStates[InputAction::MOVE_RIGHT] = true;
    }

    bool InputManager::isActionPressed(InputAction action)
    {
        return mActionStates[action];
    }

    bool InputManager::isActionJustPressed(InputAction action)
    {
        return mActionStates[action] && !mPreviousActionStates[action];
    }

    bool InputManager::isActionJustReleased(InputAction action)
    {
        return !mActionStates[action] && mPreviousActionStates[action];
    }

    float InputManager::getLeftStickX()
    {
        if (!mController) return 0.0f;

        int16_t axisValue = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTX);

        if (abs(axisValue) < ANALOG_DEADZONE)
            return 0.0f;

        return static_cast<float>(axisValue) / 32767.0f;
    }

    float InputManager::getLeftStickY()
    {
        if (!mController) return 0.0f;
        
        int16_t axisValue = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTY);
        
        if (abs(axisValue) < ANALOG_DEADZONE)
            return 0.0f;
            
        return static_cast<float>(axisValue) / 32767.0f;
    }

    void InputManager::connectController(int joystickIndex)
    {
        mController = SDL_GameControllerOpen(joystickIndex);
        if (mController)
        {
            std::cout << "Controller connected: " 
                      << SDL_GameControllerName(mController) << std::endl;
        }
    }

    void InputManager::disconnectController()
    {
        if (mController)
        {
            SDL_GameControllerClose(mController);
            mController = nullptr;
            std::cout << "Controller disconnected" << std::endl;
        }
    }
}

