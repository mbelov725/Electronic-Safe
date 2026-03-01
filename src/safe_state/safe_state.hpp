// Only compile file once
#ifndef SAFE_STATE_HPP
#define SAFE_STATE_HPP

#include "Arduino.h"

class Safe_state {
    public:
        Safe_state();
        void lock();
        bool unlock(String code);
        bool locked();
        bool has_code();
        void set_code(String new_code);
    private:
        void set_lock(bool locked);
        bool locked_;
};

#endif