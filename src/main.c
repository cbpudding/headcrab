#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "injector.h"

// TODO: Make this compatible with Windows
size_t find_script_path(uint8_t *buffer) {
    Elf64_Ehdr *header_elf = (Elf64_Ehdr *)buffer;
    Elf64_Shdr *header_sect, *header_link;
    Elf64_Sym *symbol;
    char *symbol_name;
    if(!strncmp(ELFMAG, header_elf->e_ident, SELFMAG)) {
        if(header_elf->e_type == ET_DYN) {
            for(int sect_idx = 0; sect_idx < header_elf->e_shnum; sect_idx++) {
                header_sect = (Elf64_Shdr *)((buffer + header_elf->e_shoff) + (sect_idx * header_elf->e_shentsize));
                if(header_sect->sh_type == SHT_SYMTAB) {
                    header_link = (Elf64_Shdr *)((buffer + header_elf->e_shoff) + (header_sect->sh_link * header_elf->e_shentsize));
                    for(int sym_idx = 0; sym_idx < header_sect->sh_size / header_sect->sh_entsize; sym_idx++) {
                        symbol = (Elf64_Sym *)((buffer + header_sect->sh_offset) + (sym_idx * header_sect->sh_entsize));
                        symbol_name = (buffer + header_link->sh_offset) + symbol->st_name;
                        if(!strcmp("SCRIPT_PATH", symbol_name)) {
                            return symbol->st_value - symbol->st_size;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

// TODO: Make this compatible with Windows
int inject(injector_pid_t victim, char *script_relpath) {
    uint8_t *buffer;
    char current_path[BUFSIZ];
    size_t current_path_len;
    int error_code;
    injector_t *injector;
    FILE *original;
    size_t original_size;
    FILE *payload;
    char *script_path;
    size_t script_path_len;
    size_t script_path_off;
    char *temp_path;
    char template[] = "/tmp/headcrab.XXXXXX";
    // Get the full path if the path given is relative
    if(*script_relpath == '/') {
        // Absolute path. No issues here.
        script_path = script_relpath;
    } else {
        // Relative path. Prepend with current working directory.
        getcwd(current_path, BUFSIZ);
        current_path_len = strlen(current_path);
        script_path_len = strlen(script_relpath);
        script_path = malloc(current_path_len + script_path_len);
        sprintf(script_path, "%s/%s", current_path, script_relpath);
    }
    if((original = fopen("headcrab.so", "rb")) != NULL) {
        fseek(original, 0, SEEK_END);
        original_size = ftell(original);
        fseek(original, 0, SEEK_SET);
        // TODO: Find a better replacement for mktemp
        if((temp_path = mktemp(template)) != NULL) {
            if((payload = fopen(temp_path, "wb")) != NULL) {
                if((buffer = malloc(original_size)) != NULL) {
                    if(fread(buffer, 1, original_size, original) == original_size) {
                        if((script_path_off = find_script_path(buffer)) != 0) {
                            if((script_path_len = strlen(script_path)) < 4096) {
                                memcpy(buffer + script_path_off, script_path, script_path_len);
                                if(fwrite(buffer, 1, original_size, payload) == original_size) {
                                    if((error_code = injector_attach(&injector, victim)) == INJERR_SUCCESS) {
                                        fclose(payload);
                                        payload = NULL;
                                        if((error_code = injector_inject(injector, temp_path, NULL)) == INJERR_SUCCESS) {
                                            // There should be a better way to do this
                                            injector_detach(injector);
                                            free(buffer);
                                            fclose(original);
                                            // Free script_path if we needed to convert from a relative path
                                            if(*script_relpath != '/') {
                                                free(script_path);
                                            }
                                            return 0;
                                        } else {
                                            printf("Failed to inject: %s\r\n", injector_error());
                                        }
                                        injector_detach(injector);
                                    } else {
                                        printf("Failed to attach to process: %s\r\n", injector_error());
                                    }
                                }
                            }
                        }
                    }
                    free(buffer);
                }
                if(payload != NULL) {
                    fclose(payload);
                }
            }
        }
        fclose(original);
    }
    // Free script_path if we needed to convert from a relative path
    if(*script_relpath != '/') {
        free(script_path);
    }
    return 1;
}

// TODO: Make this compatible with Windows
int main(int argc, char *argv[]) {
    injector_pid_t victim;
    if(argc > 1) {
        if(!strcmp(argv[1], "name")) {
            if(argc == 4) {
                // TODO: Find process by name
            } else {
                printf("Usage: %s name <executable name> <path to script>\r\n", argv[0]);
            }
        } else if(!strcmp(argv[1], "pid")) {
            if(argc == 4) {
                if(victim = atoi(argv[2])) {
                    return inject(victim, argv[3]);
                }
            } else {
                printf("Usage: %s pid <pid> <path to script>\r\n", argv[0]);
            }
        } else {
            printf("Usage: %s <name|pid> ...\r\n", argv[0]);
        }
    } else if(argc == 1) {
        printf("Usage: %s <name|pid> ...\r\n", argv[0]);
    } else {
        puts("Calling a program without arguments? This isn't pkexec!");
    }
    return 1;
}