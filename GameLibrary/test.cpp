#include "test.h"

namespace gfw {

Test::Test()
: initialized_(false) {
}


Test::~Test() {
}

void Test::initialize() {
    initialized_ = true;
}

void Test::update() {

}

void Test::draw() {

}

}