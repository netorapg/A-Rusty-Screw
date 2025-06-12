#ifndef ENEMY_H
#define ENEMY_H

#include "Object.h"
#include "Animation.h"
#include "Player.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <string>

namespace ARSCREW
{
    enum class EnemyState
    {
        IDLE,
        FOLLOWING,
        ATTACKING,
        STUNNED
    };

    class Enemy : public DynamicObject
    {
    private:
        SDL_Texture* mSpriteSheetTexture;
        
        // Estado e comportamento
        EnemyState mCurrentState;
        int mFacingDirection;
        float mFollowSpeed;
        float mFollowDistance;
        float mAttackDistance;
        
        // Sistema de animação
        std::unordered_map<std::string, Animation> mAnimations;
        std::string mCurrentAnimation;
        
        // Sistema de ataque
        bool mIsAttacking;
        float mAttackDuration;
        float mAttackCooldown;
        const float ATTACK_DURATION = 0.5f;
        const float ATTACK_COOLDOWN = 1.5f;
        
        // Hitboxes
        SDL_Rect mAttackHitbox;
        SDL_Rect mHurtbox;
        bool mShowDebugRects;
        
        // Métodos privados
        void loadAnimations(SDL_Renderer* renderer);
        void updateState(const Player& player, float deltaTime);
        void followPlayer(const Player& player, float deltaTime);
        void updateAnimation(float deltaTime);
        void updateHitboxes();
        void updateAttack(float deltaTime);
        
        void DrawDebugRect(SDL_Renderer* renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);

    public:
        Enemy(Vector position, SDL_Renderer* renderer);
        ~Enemy();
        
        // Métodos herdados
        void update(float deltaTime) override;
        void render(SDL_Renderer* renderer, Vector cameraPosition) override;
        void handleEvent(SDL_Event& e) override;
        
        // Métodos específicos do inimigo
        void updateWithPlayer(const Player& player, float deltaTime);
        
        // Getters
        SDL_Rect getAttackHitbox() const { return mAttackHitbox; }
        SDL_Rect getHurtbox() const { return mHurtbox; }
        bool isAttacking() const { return mIsAttacking; }
        EnemyState getCurrentState() const { return mCurrentState; }
        int getFacingDirection() const { return mFacingDirection; }
    };
}
#endif // ENEMY_H