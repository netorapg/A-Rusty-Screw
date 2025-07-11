#ifndef PUNKTAURO_H
#define PUNKTAURO_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <string>
#include <functional>
#include "Enemy.h"
#include "Player.h"

namespace ARSCREW
{
    enum class PunktauroState
    {
        PHASE_1,        // Fase normal - seguir e atacar
        PHASE_2,        // Fase intermediária - mais agressivo
        PHASE_3,        // Fase final - padrões de ataque especiais
        DEFEATED        // Derrotado
    };

    class Punktauro : public Enemy
    {
    public:
        Punktauro(Vector position, SDL_Renderer* renderer);
        virtual ~Punktauro();
        
        // Métodos sobrescritos
        void update(float deltaTime) override;
        void render(SDL_Renderer* renderer, Vector cameraPosition) override;
        void updateWithPlayer(const Player& player, float deltaTime);
        
        // Métodos específicos do Punktauro
        void takeDamage(int damage);
        bool isInPhase(PunktauroState phase) const { return mCurrentState == phase; }
        PunktauroState getCurrentPhase() const { return mCurrentState; }
        
        // Getter para hurtbox da cabeça
        SDL_Rect getHeadHurtbox() const { return mHeadHurtbox; }
        
        // Getters para escala visual
        float getVisualScale() const { return mVisualScale; }
        void setVisualScale(float scale) { mVisualScale = scale; }
        
        // Callbacks para sons específicos do Punktauro
        void setAccelerateSoundCallback(std::function<void()> callback) { mAccelerateSoundCallback = callback; }
        void setJumpSoundCallback(std::function<void()> callback) { mJumpSoundCallback = callback; }
        void setDeathSoundCallback(std::function<void()> callback) { mDeathSoundCallback = callback; }

    private:
        // Configurações visuais
        float mVisualScale;  // Escala visual do boss (1.0f = tamanho original)
        
        // Hurtbox específica da cabeça
        SDL_Rect mHeadHurtbox;
        
        // Estado específico do boss
        PunktauroState mCurrentState;
        
        // Propriedades de fases
        float mPhase1Health;    // 70% da vida
        float mPhase2Health;    // 30% da vida
        
        // Ataques especiais
        float mSpecialAttackTimer;
        float mSpecialAttackCooldown;
        bool mIsDoingSpecialAttack;
        
        // Padrões de movimento
        Vector mOriginalPosition;
        float mMovementTimer;
        bool mIsCharging;
        
        // Callbacks para sons
        std::function<void()> mAccelerateSoundCallback;
        std::function<void()> mJumpSoundCallback;
        std::function<void()> mDeathSoundCallback;
        
        // Flags para evitar repetição de sons
        bool mDeathSoundPlayed;
        bool mAccelerateSoundPlayed;
        bool mJumpSoundPlayed;

        // Métodos privados
        void updatePhase();
        void performSpecialAttack(const Player& player, float deltaTime);
        void updateMovementPattern(const Player& player, float deltaTime);
        void phase1Behavior(const Player& player, float deltaTime);
        void phase2Behavior(const Player& player, float deltaTime);
        void phase3Behavior(const Player& player, float deltaTime);
        void loadBossAnimations(SDL_Renderer* renderer);
        void updatePunktauroHitboxes(); // Método específico para atualizar hitboxes do boss
        void followPlayer(const Player& player, float deltaTime); // Override para adicionar lógica de chão
        
        // Constantes
        static const float SPECIAL_ATTACK_COOLDOWN;
        static const float CHARGE_SPEED;
        static const int PHASE_1_DAMAGE;
        static const int PHASE_2_DAMAGE;
        static const int PHASE_3_DAMAGE;
        static const float BOSS_INVULNERABILITY_DURATION;
    };
}

#endif // PUNKTAURO_H