#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void flushDisplay();
void torchON();
void torchOFF();

// Safe path operations
bool safe_path_join(char *dest, size_t dest_size, const char *base, const char *append);
bool validate_path_component(const char *component);

#endif
