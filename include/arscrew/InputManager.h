#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <string>

namespace ARSCREW
{
    enum class InputAction
    {
        MOVE_LEFT,
        MOVE_RIGHT,
        JUMP,
        ATTACK,
        DASH,
        SWITCH_ATTACK,
        CROUCH,
        DEBUG_TOGGLE,
        DOWN_ATTACK,
    };

    class InputManager
    {
        private:
            SDL_GameController* mController = nullptr;
            std::unordered_map<InputAction, bool> mActionStates;
            std::unordered_map<InputAction, bool> mPreviousActionStates;

            const int ANALOG_DEADZONE = 8000; 
        
        public:
            InputManager();
            ~InputManager();

            void handleEvent(SDL_Event& e);
            void update();

            bool isActionPressed(InputAction action);
            bool isActionJustPressed(InputAction action);
            bool isActionJustReleased(InputAction action);

            float getLeftStickX();
            float getLeftStickY();

            bool hasController() const { return mController != nullptr; }

        private:
            void connectController(int joystickIndex);
            void disconnectController();
            void updateKeyboardInput();
            void updateControllerInput();
    };
}

#endif