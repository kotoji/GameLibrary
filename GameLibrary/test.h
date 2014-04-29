#ifndef _GFW_TEST_H
#define _GFW_TEST_H
#include "game.h"

namespace gfw {

class Test : public Game {
    bool initialized_;

public:
    Test();
    virtual ~Test();

    void initialize() override;

    bool isInitialized() const override { return initialized_; };

    void update() override;

    void draw() override;
};

} // namespace gfw

#endif