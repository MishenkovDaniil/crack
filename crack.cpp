#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "crack.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_GIF
#include "stb/stb_image.h"

static const char *filename = "krkme.com";
static const int MMAP_ERROR = -1;
static const int FRAME_ERROR = -1;
static const int CMD_VALUE = 0;
static const char JMP_1  = 0xE9;
static const char JMP_2  = 0xA2;
static const char JMP_3  = 0x00;

int main ()
{
    if (!crack ())
    {
        return EXIT_FAILURE;
    }

    return 0;
}


int crack ()
{
    if (sf_crack ())
    {
        return EXIT_FAILURE;
    }
    
    char *bin_exe = mmap_file_rw (filename);
    if (!(bin_exe))
    {
        if (errno)
            return errno;
        else
            return MMAP_ERROR;
    }

    *(bin_exe + CMD_VALUE) = JMP_1;
    *(bin_exe + CMD_VALUE + 1) = JMP_2;
    *(bin_exe + CMD_VALUE + 2) = JMP_3;

    for (int i = 0; i < 100000; ++i)
    {
        void *buff = calloc (100000, sizeof (int));
        free (buff);
    }
    
    return 0;
}

int sf_crack ()
{
    int width  = 280;
    int height = 320;
    int frame_num = 0;
    int comp_mod = 0; 
    int *delays = (int *)calloc (1, sizeof (int));
    
    sf::Music music;
    music.openFromFile ("new_order.ogg");

    music.setVolume (100);
    music.play ();

    sf::RenderWindow window(sf::VideoMode (width,height), "CRACKING...");

    FILE *gif_file = stbi__fopen ("for_crack.gif", "rb");
    assert (gif_file);    
    stbi__context s = {};
    stbi__start_file (&s, gif_file);

    void *gif_parsed = stbi__load_gif_main (&s, &delays, &width, &height, &frame_num, &comp_mod, STBI_rgb_alpha);

    while (window.isOpen ())
    {
        sf::Event event;
        while (window.pollEvent (event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close ();
            }
        }
    
        window.clear();

        size_t step = width * height * STBI_rgb_alpha;
        
        for (int num = 0; num < frame_num; ++num)
        {
            sf::Image image;
            sf::Texture texture;
            sf::Sprite sprite;
            
            image.create (width, height, (const sf::Uint8 *)((char *)gif_parsed + step * num));
            
            texture.loadFromImage (image);

            sprite.setTexture (texture);

            window.draw (sprite);
            window.display (); 

            usleep (50000);
        }
    }
    music.stop ();

    free (delays);
    free (gif_parsed);
    fclose (gif_file);

    return EXIT_SUCCESS;
}

char *mmap_file_rw (const char *filename)
{
    int bin_file_descr = open (filename, O_RDWR);
    if (bin_file_descr == -1)
    {
        perror ("open() failed");
        return nullptr;
    }
    
    size_t filesize = get_file_size (filename);
    if (!(filesize))
    {
        return nullptr;
    }

    char *bin_exe = nullptr;
    bin_exe = (char *)mmap ((void *)bin_exe, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, bin_file_descr, 0);

    if (bin_exe == MAP_FAILED)
    {
        perror ("mmap() failed.\n");
    }

    if (close (bin_file_descr) == -1)
    {
        printf ("close() failed, file \"%s\".\n", filename);
        return nullptr;
    }

    return bin_exe;
}

size_t get_file_size (const char *filename)
{
    struct stat buf = {};

    if (stat (filename, &buf) == -1)
    {
        perror ("stat() failed.\n");

        return 0;
    }

    return buf.st_size;
}