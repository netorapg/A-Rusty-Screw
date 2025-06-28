#ifndef PUNKTAURO_H
#define PUNKTAURO_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <string>
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
        
    private:
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
        
        // Métodos privados
        void updatePhase();
        void performSpecialAttack(const Player& player, float deltaTime);
        void updateMovementPattern(const Player& player, float deltaTime);
        void phase1Behavior(const Player& player, float deltaTime);
        void phase2Behavior(const Player& player, float deltaTime);
        void phase3Behavior(const Player& player, float deltaTime);
        void loadBossAnimations(SDL_Renderer* renderer);
        void updatePunktauroHitboxes(); // Método específico para atualizar hitboxes do boss
        
        // Constantes
        static const float SPECIAL_ATTACK_COOLDOWN;
        static const float CHARGE_SPEED;
        static const int PHASE_1_DAMAGE;
        static const int PHASE_2_DAMAGE;
        static const int PHASE_3_DAMAGE;
    };
}

#endif // PUNKTAURO_H