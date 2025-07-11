#include <SdFat.h>
#include <TFT_eSPI.h>
#include "SD_utils.h"
#include "utils.h"

// External declaration of tft object
extern TFT_eSPI tft;

SdFat sd;

bool init_sd_card() {
    if (!sd.begin(SS, SD_SCK_MHZ(32))) {
        Serial.println("Failed to initialize SD card");
        return false;
    }
    Serial.println("SD card initialized successfully");
    return true;
}

bool check_and_create_dir(const char *path) {
    SdFile dir;
    if (!dir.open(path)) {
        if (sd.mkdir(path)) {
            Serial.printf("Directory %s created\n", path);
        } else {
            Serial.printf("Failed to create directory %s\n", path);
            return false;
        }
    } else {
        dir.close();
    }
    return true;
}

bool open_dir(SdFile &dir, const char *path) {
    if (!dir.open(path)) {
        Serial.printf("Failed to open directory %s\n", path);
        return false;
    }
    Serial.printf("Opened directory %s successfully\n", path);
    return true;
}

std::vector<FileInfo> list_files_in_dir(SdFile &dir) {
    std::vector<FileInfo> files;
    dir.rewind();
    SdFile entry;
    while (entry.openNext(&dir, O_RDONLY)) {
        FileInfo info;
        entry.getName(info.name, sizeof(info.name));
        info.size = entry.fileSize();
        files.push_back(info);
        entry.close();
    }
    return files;
}

bool create_directory(const char *path, const char *dirName) {
    char full_path[256];  // Increased buffer size
    
    // Use safe path join to prevent issues
    if (!safe_path_join(full_path, sizeof(full_path), path, dirName)) {
        Serial.printf("Failed to create directory - invalid path: %s/%s\n", path, dirName);
        return false;
    }
    
    if (sd.mkdir(full_path)) {
        Serial.printf("Directory %s created\n", full_path);
        return true;
    } else {
        Serial.printf("Failed to create directory %s\n", full_path);
        return false;
    }
}