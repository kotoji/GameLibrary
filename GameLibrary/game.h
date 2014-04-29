#ifndef _GFW_GAME_H
#define _GFW_GAME_H

namespace gfw {

class Game {
    bool initialized_;

public:
    Game();
    virtual ~Game();

    virtual void initialize();

    virtual bool isInitialized() const { return initialized_; };

    virtual void draw();

    virtual void update();
};

} // namespace gfw

#endif