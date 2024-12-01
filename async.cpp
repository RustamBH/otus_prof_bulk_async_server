#include "async.h"


SingletonGetCmd& GetInstance() noexcept
{
    static SingletonGetCmd instance;
    return instance;
}

namespace async {
    handle_t connect(std::size_t bulk) {
        return GetInstance().Connect(bulk);        
    }

    void receive(handle_t handle, const char *data, std::size_t size) {
        return GetInstance().Receive(handle, data, size);
    }

    void disconnect(handle_t handle) {
        return GetInstance().Disconnect(handle);
    }

}
