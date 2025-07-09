#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unordered_map>
#include <set>
#include "Object.h"
#include "Animation.h"
#include "Sprite.h"
#include "Globals.h"

namespace ARSCREW
{
    // Enum para os tipos de ataque do jogador
    enum class AttackType
    {
        CUTTING,    // Ataque cortante (para parafusos FLATHEAD)
        PIERCING    // Ataque perfurante (para parafusos PHILLIPS)
    };

    class Player : public DynamicObject 
    {
        SDL_Texture* spriteSheetTexture;
        public:
            Player(Vector position, SDL_Renderer* renderer);
            ~Player() 
            {
                animations.clear();
                if (spriteSheetTexture) 
                {
                    SDL_DestroyTexture(spriteSheetTexture);
                }
            }
            void update(float deltaTime) override;
            void render(SDL_Renderer* renderer, Vector cameraPosition) override;
            void handleWallJump(Vector& velocity);
            void setPassingThroughPlatform(bool enable);
            
            // Métodos para controle direto do jogador
            void moveLeft();
            void moveRight();
            void stopHorizontalMovement();
            void jump();
            void startAttack();
            void switchAttackType();
            void startDash();
            void toggleDebugDisplay();
            void passThroughPlatform(bool enable);
            bool isFacingRight() const { return mFacingDirection == 1; }
            bool isOnGround() const { return mOnGround; }
            int getFacingDirection() const { return mFacingDirection; }
            int getWidth() const { return static_cast<int>(mSize.x); }
            int getHeight() const { return static_cast<int>(mSize.y); }

            SDL_Rect getAttackHitbox() const { return mAttackHitbox; }
            SDL_Rect getHurtbox() const { return mHurtbox; }
            bool isAttacking() const { return mIsAttacking; }
            AttackType getCurrentAttackType() const { return mCurrentAttackType; }
            
            // Sistema de vida e dano
            void takeDamage(int damage);
            void heal(int healAmount);
            void resetHealth(); // Restaura a vida ao máximo
            int getCurrentHealth() const { return mCurrentHealth; }
            int getMaxHealth() const { return mMaxHealth; }
            bool isDead() const { return mCurrentHealth <= 0; }
            bool isInvulnerable() const { return mInvulnerabilityTimer > 0.0f; }
            
            void setMovementInput(bool movingLeft, bool movingRight);
            bool isMoving() const { return mIsMovingLeft || mIsMovingRight; }
            
            // Sistema de coleta de pontas
            void collectToolTip(AttackType tipType);
            bool hasToolTip(AttackType tipType) const;
            bool canUseAttackType(AttackType attackType) const;
            void switchToAvailableAttackType(); // Trocar para um tipo disponível
            std::set<AttackType> getCollectedToolTips() const { return mCollectedToolTips; }
            
            // Controle de colisões laterais
            void setHadLateralCollision(bool hadCollision) { mHadLateralCollisionThisFrame = hadCollision; }
            bool hadLateralCollisionThisFrame() const { return mHadLateralCollisionThisFrame; }
            void setLateralCollisionBuffer(float time) { mLateralCollisionBuffer = time; }

        private:
            int mFacingDirection;
            bool mIsJumping;
            bool mIsAttacking = false;
            bool mIsFalling = false;
            bool mShowDebugRects = false; 
            bool mIsDashing = false;
            float mDashTimer = 0.0f;
            const float DASH_DURATION = 0.2f;
            const float DASH_SPEED = 500.0f;
            
            // Sistema de ataque
            AttackType mCurrentAttackType = AttackType::CUTTING;
            float mAttackDuration = 0.0f;
            const float ATTACK_DURATION = 0.2f;
            
            // Sistema de coleta de pontas
            std::set<AttackType> mCollectedToolTips; // Pontas coletadas pelo jogador
            
            // Sistema de vida e dano
            int mMaxHealth = 100;
            int mCurrentHealth = 100;
            float mInvulnerabilityTimer = 0.0f;
            const float INVULNERABILITY_DURATION = 1.0f; // 1 segundo de invulnerabilidade após tomar dano
            bool mIsFlashing = false;
            float mFlashTimer = 0.0f;
            const float FLASH_INTERVAL = 0.1f; // Piscar a cada 0.1 segundos
            
            std::unordered_map<std::string, Animation> animations;
            std::string currentAnimation;
            Vector mWeaponOffsetRight = {-1, -2};
            Vector mPunchOffset;
            Vector mStrongPunchOffset;

            SDL_Rect mAttackHitbox;
            SDL_Rect mHurtbox;
            bool mShowAttackHitbox = true;
            bool mShowHurtbox = true;

            bool mIsMovingLeft = false;
            bool mIsMovingRight = false;
            
            // Controle de colisão lateral para evitar penetração visual
            bool mHadLateralCollisionThisFrame = false;
            float mLateralCollisionBuffer = 0.0f; // Timer para evitar movimento após colisão lateral
            Vector mLastValidPosition;

            void updateWeaponPosition();
            void updateHurtbox();
            void updateAttackHitbox(); // Adicionar este método
            void updateInvulnerability(float deltaTime);
            void DrawDebugOutline(SDL_Renderer* renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, int thickness = 1);
    };
}
#endif