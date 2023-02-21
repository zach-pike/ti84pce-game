#pragma once


template <typename T>
T max(T o1, T o2) {
    if (o1 > o2) return o1;
    return o2;
}

template <typename T>
T min(T o1, T o2) {
    if (o1 < o2) return o1;
    return o2;
}