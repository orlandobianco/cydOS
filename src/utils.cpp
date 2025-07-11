#include "utils.h"
#include <Arduino.h>
#include "main.h"
#include <string.h>

void torchON() {
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
}

void torchOFF() {
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
}

bool validate_path_component(const char *component) {
    if (component == nullptr || strlen(component) == 0) {
        return false;
    }
    
    // Check for dangerous patterns
    if (strstr(component, "..") != nullptr || 
        strchr(component, '/') != nullptr || 
        strchr(component, '\\') != nullptr ||
        strchr(component, ':') != nullptr) {
        return false;
    }
    
    return true;
}

bool safe_path_join(char *dest, size_t dest_size, const char *base, const char *append) {
    if (dest == nullptr || base == nullptr || append == nullptr || dest_size == 0) {
        return false;
    }
    
    // Validate the append component
    if (!validate_path_component(append)) {
        return false;
    }
    
    // Calculate required size
    size_t base_len = strlen(base);
    size_t append_len = strlen(append);
    size_t required_size = base_len + 1 + append_len + 1; // base + '/' + append + '\0'
    
    if (required_size > dest_size) {
        return false;
    }
    
    // Safely construct the path
    int result = snprintf(dest, dest_size, "%s/%s", base, append);
    
    // Check if the result was truncated
    if (result < 0 || (size_t)result >= dest_size) {
        return false;
    }
    
    return true;
}


