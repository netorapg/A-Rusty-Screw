#ifndef __ARSCREW_GATE_H__
#define __ARSCREW_GATE_H__

#include "Object.h"
#include "Sprite.h"
#include "Screw.h"
#include <SDL2/SDL.h>
#include <memory>
#include <functional>

namespace ARSCREW
{
    enum class GateState
    {
        CLOSED,
        OPENING,
        OPEN,
        CLOSING
    };

    class Gate : public StaticObject
    {
    public:
        Gate(const Vector& position, 
             const Vector& size,
             SDL_Texture* texture,
             SDL_Renderer* renderer,
             Screw* targetScrew = nullptr);

        virtual ~Gate() = default;

        virtual void render(SDL_Renderer* renderer, Vector cameraPosition) override;
        
        // Atualizar o portão (animação de abertura/fechamento)
        void update(float deltaTime);

        // Verificar se o parafuso alvo foi destruído
        void checkScrewStatus();

        // Configurar o parafuso que controla este portão
        void setTargetScrew(Screw* screw) { mTargetScrew = screw; }
        Screw* getTargetScrew() const { return mTargetScrew; }

        // Estado do portão
        GateState getState() const { return mState; }
        bool isOpen() const { return mState == GateState::OPEN; }
        bool isClosed() const { return mState == GateState::CLOSED; }
        bool isMoving() const { return mState == GateState::OPENING || mState == GateState::CLOSING; }

        // Para colisão - quando o portão está fechado, ele bloqueia passagem
        SDL_Rect getBoundingBox() const;
        bool blockCollision() const { return !isOpen(); }

        // Configurações de velocidade de abertura
        void setOpeningSpeed(float speed) { mOpeningSpeed = speed; }
        float getOpeningSpeed() const { return mOpeningSpeed; }

        // Manual open/close (para debug ou eventos especiais)
        void forceOpen() { mState = GateState::OPENING; }
        void forceClose() { mState = GateState::CLOSING; }
        
        // Callback para som
        void setOpenSoundCallback(std::function<void()> callback) { mOpenSoundCallback = callback; }

    private:
        Screw* mTargetScrew;          // Ponteiro para o parafuso que controla este portão
        GateState mState;             // Estado atual do portão
        
        SpritePtr mClosedSprite;      // Sprite do portão fechado
        SpritePtr mOpenSprite;        // Sprite do portão aberto (opcional)
        
        float mOpeningSpeed;          // Velocidade de abertura (pixels por segundo)
        float mCurrentOffset;         // Offset atual da animação de abertura
        float mMaxOffset;             // Offset máximo (altura total do portão)
        
        bool mScrewWasDestroyed;      // Flag para rastrear se o parafuso foi destruído
        
        // Callback para som de abertura
        std::function<void()> mOpenSoundCallback;

        void startOpening();
        void startClosing();
        void updateOpening(float deltaTime);
        void updateClosing(float deltaTime);
        void renderGateWithStripes(SDL_Renderer* renderer, const Vector& cameraPosition, 
                                   const Vector& position, const Vector& size);
    };

} // namespace ARSCREW

#endif // __ARSCREW_GATE_H__
