#ifndef __ARSCREW_TOOLTIP_H__
#define __ARSCREW_TOOLTIP_H__

#include "Object.h"
#include "Sprite.h"
#include <SDL2/SDL.h>

namespace ARSCREW
{
    enum class ToolTipType
    {
        FLATHEAD,  // Ponta de fenda (para ataques cortantes)
        PHILLIPS   // Ponta Phillips (para ataques perfurantes)
    };

    class ToolTip : public StaticObject
    {
    public:
        ToolTip(const Vector& position, 
                ToolTipType type,
                SDL_Texture* texture,
                SDL_Renderer* renderer);

        virtual ~ToolTip() = default;

        virtual void render(SDL_Renderer* renderer, Vector cameraPosition) override;
        
        // Atualizar animação de flutuação
        void update(float deltaTime);

        // Coleta da ponta
        void collect() { mCollected = true; }
        bool isCollected() const { return mCollected; }

        // Getters
        ToolTipType getType() const { return mType; }
        SDL_Rect getBoundingBox() const;

        // Efeito visual
        void setFloating(bool floating) { mFloating = floating; }
        bool isFloating() const { return mFloating; }

    private:
        ToolTipType mType;
        SpritePtr mSprite;
        bool mCollected;
        
        // Efeito de flutuação
        bool mFloating;
        float mFloatTimer;
        float mFloatAmplitude;
        float mFloatSpeed;
        Vector mOriginalPosition;

        void renderToolTipCustom(SDL_Renderer* renderer, const Vector& cameraPosition, 
                                const Vector& position, const Vector& size);
    };

} // namespace ARSCREW

#endif // __ARSCREW_TOOLTIP_H__
