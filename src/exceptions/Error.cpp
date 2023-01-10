#include "exceptions/Error.h"

Error::Error(const std::string message): exception(message.c_str()) {
        
}