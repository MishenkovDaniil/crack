#ifndef CRACK_H
#define CRACK_H

#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


int crack ();
int sf_crack ();
size_t get_file_size (const char *filename);
char *mmap_file_rw (const char *filename);

#endif /* CRACK_H */