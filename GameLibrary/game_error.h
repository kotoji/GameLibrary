#ifndef _GFW_GAME_ERROR_H
#define _GFW_GAME_ERROR_H

#include <string>
#include <exception>

namespace gfw {

enum class ErrorCode {
    FatalError,
    Warning,
};

class GameError : public std::exception {
    ErrorCode error_code_;
    std::string message_;
public:
    GameError() throw()
        : error_code_(ErrorCode::FatalError)
        , message_("Undefined Error.") {}
    GameError(const GameError& e) throw()
        : std::exception(e)
        , error_code_(e.error_code_)
        , message_(e.message_) {}
    GameError(ErrorCode code, const std::string mes) throw()
        : error_code_(code)
        , message_(mes) {}
    GameError& operator=(const GameError& rhs) throw() {
        std::exception::operator=(rhs);
        this->error_code_ = rhs.error_code_;
        this->message_ = rhs.message_;
    }
    ~GameError() override {}

    const char* what() const throw() override {
        return message_.c_str();
    }

};

} // namespace gfw

#endif