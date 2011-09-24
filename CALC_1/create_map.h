#ifndef CREATE_MAP_H
#define	CREATE_MAP_H

#include <map>

template <typename T, typename U>
class create_map {
private:
    std::map<T, U> _map;
public:

    create_map(const T& key, const U& val) {
        _map[key] = val;
    }

    create_map<T, U>& operator()(const T& key, const U& val) {
        _map[key] = val;
        return *this;
    }

    operator std::map<T, U>() {
        return _map;
    }
};

#endif	/* CREATE_MAP_H */

