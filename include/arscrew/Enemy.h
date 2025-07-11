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
    protected:
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
        bool mIsDestroyed;  // Corrigir nome da variável
        float mAttackDuration;
        float mAttackCooldown;
        const float ATTACK_DURATION = 0.5f;
        const float ATTACK_COOLDOWN = 1.5f;
        int mDamage;        // Adicionar dano do inimigo

        int mMaxHealth = 100;
        int mCurrentHealth = 100;
        float mInvulnerabilityTimer = 0.0f;
        const float INVULNERABILITY_DURATION = 1.0f; // 1 segundo
        bool mIsFlashing = false;
        
        // Flags para controlar sons
        bool mHitSoundPlayed = false;
        bool mDeathSoundPlayed = false;
        float mFlashTimer = 0.0f;
        const float FLASH_INTERVAL = 0.1f;
        
        // Hitboxes
        SDL_Rect mAttackHitbox;
        SDL_Rect mHurtbox;
        bool mShowDebugRects;
        
        // Métodos protegidos
        void loadAnimations(SDL_Renderer* renderer);
        void updateState(const Player& player, float deltaTime);
        void followPlayer(const Player& player, float deltaTime);
        void updateAnimation(float deltaTime);
        void updateHitboxes();
        void updateAttack(float deltaTime);
        void updateInvulnerability(float deltaTime);
        
        void DrawDebugRect(SDL_Renderer* renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);
        
        public:
        Enemy(Vector position, SDL_Renderer* renderer);
        ~Enemy();
        
        // Métodos herdados
        void update(float deltaTime) override;
        void render(SDL_Renderer* renderer, Vector cameraPosition) override;
        SDL_Rect getBoundingBox() const;
        
        // Métodos específicos do inimigo
        void updateWithPlayer(const Player& player, float deltaTime);
        
        // Getters
        SDL_Rect getAttackHitbox() const { return mAttackHitbox; }
        SDL_Rect getHurtbox() const { return mHurtbox; }
        bool isAttacking() const { return mIsAttacking; }
        bool isDestroyed() const { return mIsDestroyed; }
        EnemyState getCurrentState() const { return mCurrentState; }
        int getFacingDirection() const { return mFacingDirection; }
        int getDamage() const { return mDamage; }

        void takeDamage(int damage);
        void heal(int healAmount);
        int getCurrentHealth() const { return mCurrentHealth; }
        int getMaxHealth() const { return mMaxHealth; }
        bool isDead() const { return mCurrentHealth <= 0; }
        bool isInvulnerable() const { return mInvulnerabilityTimer > 0.0f; }
        
        // Métodos para controlar sons
        bool hasHitSoundPlayed() const { return mHitSoundPlayed; }
        bool hasDeathSoundPlayed() const { return mDeathSoundPlayed; }
        void setHitSoundPlayed(bool played) { mHitSoundPlayed = played; }
        void setDeathSoundPlayed(bool played) { mDeathSoundPlayed = played; }
        
        // Adicionar métodos que faltam
        void destroy() { mIsDestroyed = true; }
        void toggleDebugDisplay() { mShowDebugRects = !mShowDebugRects; }
    };
}
#endif // ENEMY_H