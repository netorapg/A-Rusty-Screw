#include "../include/bettlerider/Platform.h"
#include "../include/bettlerider/Animation.h"

namespace BRTC {

Platform::Platform(const Vector position, const Vector size, SDL_Renderer* renderer, const std::string& texturePath)
    : StaticObject(position, size), 
      mSprite(renderer, texturePath) // Carrega a textura
{
    // Define uma animação com um único frame (região do sprite sheet)
    Animation platformAnim;
    platformAnim.addFrame({
        { 127, 36, static_cast<int>(size.x), static_cast<int>(size.y) }, // SDL_Rect (x, y, w, h)
        1.0f, // Duração (irrelevante para um frame)
        { 0, 0 } // Offset
    });
    platformAnim.setLoop(true); // Mantém o frame estático

    mSprite.addAnimation("default", std::move(platformAnim));
    mSprite.play("default");
}

void Platform::render(SDL_Renderer* renderer, Vector cameraPosition) {
    const Vector screenPosition = mPosition - cameraPosition;
    mSprite.draw(renderer, screenPosition.x, screenPosition.y);
}

} // namespace BRTC